/*
 * (C) Copyright IBM Corp. 2003
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  This
 * file and program are licensed under a BSD style license.  See
 * the Copying file included with the OpenHPI distribution for
 * full licensing terms.
 *
 * Authors:
 *   Konrad Rzeszutek <konradr@us.ibm.com>
 *
 * $Id$
 *
 */

#include <net-snmp/net-snmp-config.h>
#include <net-snmp/net-snmp-includes.h>
#include <net-snmp/agent/net-snmp-agent-includes.h>
#include <signal.h>

#include <saHpiTable.h>
#include <saHpiRdrTable.h>
#include <saHpiSensorTable.h>
#include <saHpiCtrlTable.h>
#include <saHpiInventoryTable.h>
#include <saHpiWatchdogTable.h>
#include <saHpiHotSwapTable.h>
#include <saHpiSystemEventLogTable.h>
#include <saHpiEventTable.h>

#include <hpiSubagent.h>
#include <alarm.h>
#include <stdio.h>
/*
 * Internal data for the sub-agent.
 */
static int keep_running;
static int session_avail = AGENT_FALSE;
static SaHpiSessionIdT session_id;
static SaHpiRptInfoT rpt_info;
static SaErrorT err;

/*
 * Configuration options. Changed by config file.
 */

int send_traps = AGENT_FALSE;
static int send_traps_on_startup = AGENT_FALSE;

// Check for information every x seconds.
int alarm_interval = 5;

// Max EVENT rows.
int MAX_EVENT_ENTRIES = 512;
// IBM-KR: TODO, add a configuration entry for this.

static RETSIGTYPE
stop_server(int a) {
    keep_running = 0;
}

int build_full_oid(oid *prefix, size_t prefix_len,
		   oid *column, size_t column_len,
		   netsnmp_index *index,
		   oid *out_oid, size_t in_len,size_t *out_len) {
  
  int register len = prefix_len + column_len;
  int register i = 0;

  if (index)
    len+=index->len;

  if (len > in_len)
    return AGENT_ERR_MEMORY_FAULT;

  *out_len = len;
  memcpy (out_oid, prefix, prefix_len * sizeof(oid));

  for (; i < column_len; i++) {    
    out_oid[prefix_len+i] = column[i];
  }
  len = prefix_len+i;

  if (index) {
    for (i=0; i < index->len; i++) {
      out_oid[len+i] = index->oids[i];
    }
  }
  return AGENT_ERR_NOERROR;
}


netsnmp_variable_list *build_notification(
		      const netsnmp_index *index,
		      const trap_vars *var, const size_t var_len,
		      const oid *notification_oid, const size_t notification_oid_len,
		      const oid *root_table_oid, const size_t root_table_oid_len,
		      const SaHpiDomainIdT domain_id, 
		      const oid *domain_id_oid, const size_t domain_id_oid_len,
		      const SaHpiResourceIdT resource_id, 
		      const oid *resource_id_oid, const size_t resource_id_oid_len) {

  int i,j;
  netsnmp_variable_list *notification_vars = NULL;
  oid snmptrap[] = { snmptrap_oid };
  oid full_oid[MAX_OID_LEN];
  size_t full_oid_len;



  DEBUGMSGTL((AGENT,"--- build_notification: Entry.\n"));  


  if ((root_table_oid_len + 2 + index-> len) > MAX_OID_LEN) {
    DEBUGMSGTL((AGENT,"The length of the OID exceeds MAX_OID_LEN!\n"));
    return NULL;
  }

  // First is the Notification_OID
  snmp_varlist_add_variable(&notification_vars,
			    snmptrap, OID_LENGTH(snmptrap),
			    ASN_OBJECT_ID,
			    (u_char *)notification_oid,
			    notification_oid_len* sizeof(oid));
  
  // Next the DomainID
  snmp_varlist_add_variable(&notification_vars,
			    domain_id_oid, domain_id_oid_len,
			    ASN_UNSIGNED,
			    (u_char *)&domain_id,
			    sizeof(SaHpiDomainIdT));
  // The ResourceID
  snmp_varlist_add_variable(&notification_vars,
			    resource_id_oid, resource_id_oid_len,
			    ASN_UNSIGNED,
			    (u_char *)&resource_id,
			    sizeof(SaHpiResourceIdT));
  
  // Attach the rest of information
  for (i = 0; i < var_len; i++ ) {

    // Generate the full OID.	
    memcpy(full_oid, root_table_oid, root_table_oid_len * sizeof(oid));
    full_oid_len = root_table_oid_len;
    // Get the column number.
    full_oid[full_oid_len++] = 1;
    full_oid[full_oid_len++] = var[i].column;
    // Put the index value in
    for (j = 0; j< index->len; j++) {
      full_oid[full_oid_len+j] = index->oids[j];
    }
    full_oid_len += index->len;
    
    snmp_varlist_add_variable(&notification_vars,
			      full_oid,
			      full_oid_len,
			      var[i].type,
			      var[i].value,
			      var[i].value_len);
    
  }
  DEBUGMSGTL((AGENT,"--- build_notification: Exit.\n"));    
  return notification_vars;

}  
long
calculate_hash_value(void *data, int len) {

  register long hash = 0;
  register int i = 0;
  
  // WARNING. Can seg fault!
  // Pretty simple. Can also roll-over back to zero.

  //DEBUGMSGTL((AGENT,"HASH DATA:\n"));
  for ( ; i < len; i++, hash+= ((char *)data)[i]) ;
    //{
    //printf("%X",((char *)data)[i]);
    //}
    //DEBUGMSGTL((AGENT,"HASH END\n"));
  return hash;
}

SaErrorT
rcSaHpi() {
  return err;
}

int
closeSaHpiSession() {

  DEBUGMSGTL((AGENT,"--- closeSaHpiSession: Entry. "));
  if (session_avail == AGENT_TRUE) {
    err = saHpiFinalize();
    if (SA_OK != err) {    
      DEBUGMSGTL((AGENT,"saHpiFinalize error: %d ", err));
      return AGENT_ERR_SESSION_CLOSE;
    }
    session_avail = AGENT_FALSE;
  }
  DEBUGMSGTL((AGENT,"--- closeSaHpiSession: Exit. "));

  return AGENT_ERR_NOERROR;
}

int 
getSaHpiSession(SaHpiSessionIdT *out) {

  SaHpiVersionT version;

  DEBUGMSGTL((AGENT,"--- getSaHpiSession: Entry. "));

  if (session_avail == AGENT_FALSE) {
    // IBM-KR: TODO
    // These DEBUGMSGLT should be turned to snmp_log
    err = saHpiInitialize(&version);
    if (SA_OK != err) {
      DEBUGMSGTL((AGENT,"saHpiInitialize error: %d ", err));
      return AGENT_ERR_INIT;
    }
    err = saHpiSessionOpen(SAHPI_DEFAULT_DOMAIN_ID, &session_id, NULL);
    if (SA_OK != err) {
      DEBUGMSGTL((AGENT,"saHpiSessionOpen error: %d ", err));
      return AGENT_ERR_SESSION_OPEN;
   }
    // Discover 
    err = saHpiResourcesDiscover(session_id);
    if (SA_OK != err) {    
      DEBUGMSGTL((AGENT,"saHpiResourcesDiscover error: %d ", err));
      return AGENT_ERR_DISCOVER;
   }
    //Subscribe to the Events

    err = saHpiSubscribe( session_id, SAHPI_FALSE);
    if (SA_OK != err) {
      DEBUGMSGTL((AGENT,"saHpiSubscribe failed. Return code: %d", err));
      return AGENT_ERR_SUBSCRIBE;
    }
   session_avail = AGENT_TRUE;   
  }

  if (out)
    *out = session_id;

  DEBUGMSGTL((AGENT,"--- getSaHpiSession: Exit\n"));
  return AGENT_ERR_NOERROR;
}

int
didSaHpiChanged(int *answer, SaHpiRptInfoT *info ) {

  int rc = AGENT_ERR_NOERROR;
  SaHpiRptInfoT rpt_info_new;
  SaErrorT err;

  DEBUGMSGTL((AGENT,"--- didSaHpiChanged: Entry. "));

  if (session_avail == AGENT_FALSE) {
    if ((rc = getSaHpiSession(NULL)) != AGENT_ERR_NOERROR) {
      // Something is screwy. Bail out
      return rc;
    }    
  }
   
  err = saHpiRptInfoGet(session_id, &rpt_info_new);
  if (SA_OK != err) {
    DEBUGMSGTL((AGENT,"saHpiRptInfoGet error: %d ", err));
    return AGENT_ERR_RPTGET;
  }

  if ((rpt_info_new.UpdateCount != rpt_info.UpdateCount) ||
      (rpt_info_new.UpdateTimestamp != rpt_info.UpdateTimestamp)) {
    // Something new.
    if (answer) {
      *answer = AGENT_TRUE;
      DEBUGMSGTL((AGENT,"New UpdateCount: %d, TimeStamp: %d\n ", 
		  rpt_info.UpdateCount,
		  rpt_info.UpdateTimestamp));
    }
    rpt_info = rpt_info_new;
  } else {
    if (answer)
      *answer = AGENT_FALSE;
  }
  
  // Change regardless of answer.
  if (info) 
    *info = rpt_info;

  DEBUGMSGTL((AGENT,"--- didSaHpiChanged: Exit.\n"));
  return rc;

}


void
hpiSubagent_parse_config_traps(const char *token, char *cptr)
{
  int x = -1;
  char buf[BUFSIZ];

  if (!strncasecmp(cptr,"on",2) || 
      !strncasecmp(cptr,"yes", 3) || 
      !strncasecmp(cptr, "true", 4)) {
    x = AGENT_TRUE;
    snmp_log(LOG_INFO,"Sending EVENTS during startup.\n");
  } else if (!strncasecmp(cptr,"off",3) ||
	     !strncasecmp(cptr,"no",2) ||
	     !strncasecmp(cptr,"false",5)) {
    x = AGENT_FALSE;
    snmp_log(LOG_INFO,"No sending events during startup.\n");
  }

  if ((x != AGENT_TRUE) && (x != AGENT_FALSE)) {
    sprintf(buf, "hpiSubagent: '%s' unrecognized", cptr);
    config_perror(buf); 
  } else {

    send_traps_on_startup = x;
  }
}

void
hpiSubagent_parse_config_interval(const char *token, char *cptr)
{
  int x = atoi(cptr);
  char buf[BUFSIZ];
  
  if (x < -1) {
    sprintf(buf, "hpiSubagent: '%s' unrecognized", cptr);
    config_perror(buf);
  } else {
    snmp_log(LOG_INFO,"Checking HPI infrastructure every %d seconds.\n", x);
    alarm_interval = x;
  }
}
void
hpiSubagent_parse_config_max_event(const char *token, char *cptr)
{
  int x = atoi(cptr);
  char buf[BUFSIZ];
  
  if (x < -1) {
    sprintf(buf, "hpiSubagent: '%s' unrecognized", cptr);
    config_perror(buf);
  } else {
    snmp_log(LOG_INFO,"Max Event rows%d.\n", x);
    MAX_EVENT_ENTRIES = x;
  }
}



int
main (int argc, char **argv) {
  int agentx_subagent=AGENT_TRUE;

  /* change this if you want to be a SNMP master agent */

  /* print log errors to AGENT );stderr */
  debug_register_tokens(AGENT);
  snmp_enable_stderrlog();
  snmp_set_do_debugging(1);
  /* we're an agentx subagent? */
  if (agentx_subagent) {
    /* make us a agentx client. */
    netsnmp_ds_set_boolean(NETSNMP_DS_APPLICATION_ID, NETSNMP_DS_AGENT_ROLE, 1);
  }

  /* initialize the agent library */
  init_agent(AGENT);



  /* Read configuration information here, before we initialize */
  
  snmpd_register_config_handler(TRAPS_TOKEN,
				hpiSubagent_parse_config_traps,
				NULL,
				"hpiSubagent on/off switch for sending events upon startup");

  snmpd_register_config_handler(INTERVAL_TOKEN,
				hpiSubagent_parse_config_interval,
				NULL,
				"hpiSubagent time in seconds before HPI API is queried for information.");

  snmpd_register_config_handler(MAX_EVENT_TOKEN,
				hpiSubagent_parse_config_max_event,
				NULL,
				"hpiSubagent MAX number of rows for Events.");

  init_snmp(AGENT);
  /* Initialize tables */
  initialize_table_saHpiTable();
  initialize_table_saHpiRdrTable();
  initialize_table_saHpiSensorTable();
  initialize_table_saHpiCtrlTable();
  initialize_table_saHpiInventoryTable();
  initialize_table_saHpiWatchdogTable();
  initialize_table_saHpiHotSwapTable();

  initialize_table_saHpiSystemEventLogTable();
  initialize_table_saHpiEventTable();
  
  if (send_traps_on_startup == AGENT_TRUE)
    send_traps = AGENT_TRUE;

  send_traps = AGENT_TRUE;
  if (populate_rpt() != AGENT_ERR_NOERROR) {
    //goto stop;
    } 
  populate_event();

  if (init_alarm() != AGENT_ERR_NOERROR) {
    //    goto stop;
  }
  /* If we're going to be a snmp master agent, initial the ports */

  if (!agentx_subagent)
    init_master_agent();  /* open the port to listen on (defaults to udp:161) */


  /* In case we recevie a request to stop (kill -TERM or kill -INT) */
  keep_running = 1;
  signal(SIGTERM, stop_server);
  signal(SIGINT, stop_server);

  /* you're main loop here... */
  while(keep_running) {
    /* if you use select(), see snmp_select_info() in snmp_api(3) */
    /*     --- OR ---  */
    agent_check_and_process(1); /* 0 == don't block */
  }
  //stop:
  /* at shutdown time */
  snmp_shutdown(AGENT);
  return 1;
}

