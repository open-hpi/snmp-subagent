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
#include <saHpiSystemEventLogTable.h>
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

int send_traps_on_startup = AGENT_TRUE;
// Check for information every x seconds.
int alarm_interval = 5;

RETSIGTYPE
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
   session_avail = TRUE;   
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

  /* initialize mib code here */
  /* Initialize tables */
  initialize_table_saHpiTable();
  initialize_table_saHpiRdrTable();
  initialize_table_saHpiSensorTable();
  initialize_table_saHpiCtrlTable();
  initialize_table_saHpiInventoryTable();
  initialize_table_saHpiWatchdogTable();

  initialize_table_saHpiSystemEventLogTable();

  if (populate_rpt() != AGENT_ERR_NOERROR) {
    //goto stop;
  } /*
  if (init_alarm() != AGENT_ERR_NOERROR) {
    goto stop;
  } */
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

