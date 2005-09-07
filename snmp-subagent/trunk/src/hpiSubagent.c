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
 *   David Judkovics  <djudkovi@us.ibm.com>
 *
 * $Id$
 *					  
 */

#include <net-snmp/net-snmp-config.h>
#include <net-snmp/net-snmp-includes.h>
#include <net-snmp/agent/net-snmp-agent-includes.h>
#include <signal.h>

#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>

#include <hpiSubagent.h>
#include <hpiEventThread.h>

#include <SaHpi.h> 
#include <oh_utils.h>

#include <hpiCheckIndice.h>
#include <session_info.h>
#include <alarm.h>

#include <saHpiDomainInfoTable.h>
#include <saHpiDomainAlarmTable.h>
#include <saHpiDomainReferenceTable.h>

#include <saHpiResourceTable.h>
#include <saHpiRdrTable.h>

#include <saHpiAnnunciatorTable.h>

#include <saHpiInventoryTable.h>
#include <saHpiFieldTable.h>
#include <saHpiAreaTable.h>

#include <saHpiSensorTable.h>
#include <saHpiCurrentSensorStateTable.h>
#include <saHpiSensorReadingMaxTable.h>
#include <saHpiSensorReadingMinTable.h>
#include <saHpiSensorReadingNominalTable.h>
#include <saHpiSensorReadingNormalMaxTable.h>
#include <saHpiSensorReadingNormalMinTable.h>
#include <saHpiSensorThdLowCriticalTable.h>
#include <saHpiSensorThdLowMajorTable.h>
#include <saHpiSensorThdLowMinorTable.h>
#include <saHpiSensorThdNegHysteresisTable.h>
#include <saHpiSensorThdPosHysteresisTable.h>
#include <saHpiSensorThdUpCriticalTable.h>
#include <saHpiSensorThdUpMajorTable.h>
#include <saHpiSensorThdUpMinorTable.h>

#include <saHpiCtrlAnalogTable.h>
#include <saHpiCtrlDigitalTable.h>
#include <saHpiCtrlDiscreteTable.h>
#include <saHpiCtrlOemTable.h>
#include <saHpiCtrlStreamTable.h>
#include <saHpiCtrlTextTable.h>

#include <saHpiWatchdogTable.h>
#include <saHpiHotSwapTable.h>
#include <saHpiAutoInsertTimeoutTable.h>

#include <saHpiEventTable.h>
#include <saHpiResourceEventTable.h>
#include <saHpiDomainEventTable.h>
#include <saHpiSensorEventTable.h>
#include <saHpiSensorEnableChangeEventTable.h>
#include <saHpiHotSwapEventTable.h>
#include <saHpiWatchdogEventTable.h>
#include <saHpiSoftwareEventTable.h>
#include <saHpiOEMEventTable.h>
#include <saHpiUserEventTable.h>
#include <saHpiAnnouncementTable.h>

#include <saHpiEventLogInfoTable.h>
#include <saHpiEventLogTable.h>
#include <saHpiResourceEventLogTable.h>
#include <saHpiDomainEventLogTable.h>
#include <saHpiSensorEventLogTable.h>
#include <saHpiSensorEnableChangeEventLogTable.h>
#include <saHpiHotSwapEventLogTable.h>
#include <saHpiWatchdogEventLogTable.h>
#include <saHpiSoftwareEventLogTable.h>
#include <saHpiOEMEventLogTable.h>
#include <saHpiUserEventLogTable.h>
#include <saHpiAnnouncementEventLogTable.h>

/*
 * Internal data for the sub-agent.
 */
static int keep_running;
static int session_avail = AGENT_FALSE;
static SaErrorT err;

static const char *version =
  "$Id$";


/*
 * Configuration options. Changed by config file.
 */

#define REDISCOVER_COUNT_MAX 10;

int send_traps = AGENT_FALSE;
static int send_traps_on_startup = AGENT_FALSE;

static int rediscover_count = 0;
int MAX_EVENT_ENTRIES = 512; // Max EVENT rows. 

//Use syslog 
static int do_syslog = AGENT_TRUE;
static int do_fork = AGENT_FALSE;

/*
 * For polling (non-threaded) option
 */
// Check for information every x seconds.
int alarm_interval = 5;

/*
 * Internal prototypes
 */
static void usage(char *applName);
static RETSIGTYPE stop_server (int a);

static RETSIGTYPE stop_server (int a)
{
        //DMJ TODO  Really need to signal event thread to break out first
        set_run_threaded(FALSE);
        keep_running = 0;
}
   
void hpiSubagent_parse_config_traps (const char *token, char *cptr)
{
  int x = -1;
  char buf[BUFSIZ];

  if (!strncasecmp (cptr, "on", 2) ||
      !strncasecmp (cptr, "yes", 3) || !strncasecmp (cptr, "true", 4))
    {
      x = AGENT_TRUE;
      snmp_log (LOG_INFO, "Sending EVENTS during startup.\n");
    }
  else if (!strncasecmp (cptr, "off", 3) ||
	   !strncasecmp (cptr, "no", 2) || !strncasecmp (cptr, "false", 5))
    {
      x = AGENT_FALSE;
      snmp_log (LOG_INFO, "Not sending events during startup.\n");
    }

  if ((x != AGENT_TRUE) && (x != AGENT_FALSE))
    {
	  snprintf(buf, 3, "hpiSubagent: hpiSubagent_parse_config_traps, '%s' unrecognized", cptr);
      config_perror (buf);
    }
  else
    {

      send_traps_on_startup = x;
    }
}

void hpiSubagent_parse_config_max_event (const char *token, char *cptr)
{
  int x = atoi (cptr);
  char buf[BUFSIZ];

  if (x < -1)
    {

	  snprintf(buf, 3, "hpiSubagent: hpiSubagent_parse_config_max_event, '%s' unrecognized", cptr);
      config_perror (buf);
    }
  else
    {
      snmp_log (LOG_INFO, "Max Event rows %d.\n", x);
      MAX_EVENT_ENTRIES = x;
    }
}

static void usage(char *applName)
{
  printf("Usage: %s [OPTION]...\n", applName);
  printf("\n");
  printf("Options:\n");
  printf("  -d            enables debug mode\n");
  printf("  -f            enables forking\n");
  printf("  -s            disables logging via syslog facility\n");
  printf("  -C            do not read default SNMP configuration files\n");
  printf("  -x ADDRESS    use ADDRESS as AgentX address\n");  
  printf("  -h            print this help and exit\n");

  return;
}

int
main (int argc, char **argv)
{
	
	  	int agentx_subagent = AGENT_TRUE;
	  	int c;
	  	int rc = 0;
	  
	  	SaErrorT 	rv = SA_OK;
		SaHpiVersionT	hpiVer;
		SaHpiSessionIdT sessionid;
                SaHpiDomainInfoT        domain_info;  		
		SaHpiBoolT      run_threaded = TRUE;	
								
	  	pid_t child;
		
		char * env;
	  		  	
	  	/* change this if you want to be a SNMP master agent */
	  	
	  	debug_register_tokens (AGENT);
                snmp_enable_stderrlog ();
                snmp_set_do_debugging (1);

	  	while ((c = getopt (argc, argv, "fdsCx:h?")) != EOF) {
	    switch (c) {
	   	case 'f':
			do_fork = AGENT_TRUE;
		   	break;
	
	    case 'd':
			debug_register_tokens (AGENT);
			snmp_enable_stderrlog ();
			snmp_set_do_debugging (1);
	      	break;
	
	  	case 's':
			do_syslog = AGENT_FALSE;
	      	break;
	
	   	case 'C':
			netsnmp_ds_set_boolean(NETSNMP_DS_LIBRARY_ID,
				       NETSNMP_DS_LIB_DONT_READ_CONFIGS,
				       1);
	     	break;
	
	  	case 'x':
			netsnmp_ds_set_string(NETSNMP_DS_APPLICATION_ID,
				      NETSNMP_DS_AGENT_X_SOCKET,
				      optarg);
	      	break;
	
	 	case 'h':
	    default:
			usage(argv[0]);
			exit(1);
	      	break;
	    }
	  }
	
	  if (do_syslog == AGENT_TRUE) {
	      snmp_enable_calllog ();
	      snmp_enable_syslog_ident (AGENT, LOG_DAEMON);
	  }
	  snmp_log (LOG_INFO, "Starting %s\n", version);
	  /* we're an agentx subagent? */
	  if (agentx_subagent) {
	      /* make us a agentx client. */
	      rc = netsnmp_ds_set_boolean (NETSNMP_DS_APPLICATION_ID,
					   NETSNMP_DS_AGENT_ROLE, 1);
	  }
	
	  /* initialize the agent library */
	  rc = init_agent (AGENT);
	  if (rc != 0) {
	      snmp_log (LOG_ERR, "Could not initialize connection to SNMP daemon. \n"
			"Perhaps you are running %s as non-root?\n", argv[0]);
	      exit (rc);
	  }

	  /* Read configuration information here, before we initialize */
	
	  snmpd_register_config_handler (TRAPS_TOKEN,
					 hpiSubagent_parse_config_traps,
					 NULL,
					 "hpiSubagent on/off switch for sending events upon startup");
	
	  snmpd_register_config_handler (MAX_EVENT_TOKEN,
					 hpiSubagent_parse_config_max_event,
					 NULL,
					 "hpiSubagent MAX number of rows for Events.");
					 
	init_snmp (AGENT); 	
	
	/* 
	 * Initialize HPI library
	 */
	hpiVer = saHpiVersionGet();
	DEBUGMSGTL ((AGENT, "Hpi Version %d Implemented.\n", hpiVer));   	 

	rv = saHpiSessionOpen( SAHPI_UNSPECIFIED_DOMAIN_ID, &sessionid, NULL );
	if (rv != SA_OK) {
		DEBUGMSGTL ((AGENT, "saHpiSessionOpen Error: returns %s\n",
			oh_lookup_error(rv)));
		exit(-1);
	}
   	DEBUGMSGTL ((AGENT, "saHpiSessionOpen returns with SessionId %d\n", 
   		sessionid)); 

        /* Get the DomainInfo structur,  This is how we get theDomainId for this Session */
	rv = saHpiDomainInfoGet(sessionid, &domain_info);
	if (rv != SA_OK) {
                DEBUGMSGTL ((AGENT, "saHpiSessionOpen Error: returns %s\n",
                        oh_lookup_error(rv)));
                exit(-1);
	}

        /* store session numbers */
   	store_session_info(sessionid, domain_info.DomainId);	
   				
	/* subscribe all sessions/events */
        subcsribe_all_sessions();

	/* Resource discovery */
	rv = saHpiDiscover(sessionid);
	
	if (rv != SA_OK) {
		DEBUGMSGTL ((AGENT, "saHpiDiscover Error: returns %s\n",oh_lookup_error(rv)));
		exit(-1);
	}
	DEBUGMSGTL ((AGENT, "saHpiDiscover Success!\n"));	

	/* Initialize subagent tables */		
	init_saHpiDomainInfoTable(); 
	init_saHpiDomainAlarmTable();
	init_saHpiDomainReferenceTable();
	
	init_saHpiResourceTable();
	init_saHpiRdrTable();
	init_saHpiCtrlDigitalTable();
	init_saHpiCtrlDiscreteTable();
	init_saHpiCtrlAnalogTable();
	init_saHpiCtrlStreamTable();
	init_saHpiCtrlTextTable();
	init_saHpiCtrlOemTable();
	init_saHpiSensorTable();
	init_saHpiCurrentSensorStateTable();
	init_saHpiSensorReadingMaxTable();
	init_saHpiSensorReadingMinTable();
	init_saHpiSensorReadingNominalTable();
	init_saHpiSensorReadingNormalMaxTable();
	init_saHpiSensorReadingNormalMinTable();
	init_saHpiSensorThdLowCriticalTable();
	init_saHpiSensorThdLowMajorTable();
	init_saHpiSensorThdLowMinorTable();
	init_saHpiSensorThdUpCriticalTable();
	init_saHpiSensorThdUpMajorTable();
	init_saHpiSensorThdUpMinorTable();
	init_saHpiSensorThdPosHysteresisTable();
	init_saHpiSensorThdNegHysteresisTable();
	init_saHpiInventoryTable();
	init_saHpiWatchdogTable();
	init_saHpiAnnunciatorTable();
	init_saHpiAreaTable();
	init_saHpiFieldTable();

	init_saHpiEventTable();
	init_saHpiResourceEventTable();
	init_saHpiDomainEventTable();
	init_saHpiSensorEventTable();
	init_saHpiOEMEventTable();
	init_saHpiHotSwapEventTable();
	init_saHpiWatchdogEventTable();		
	init_saHpiSoftwareEventTable();
	init_saHpiSensorEnableChangeEventTable();
	init_saHpiUserEventTable();

	init_saHpiEventLogInfoTable();
	init_saHpiEventLogTable();
	init_saHpiResourceEventLogTable();
	init_saHpiSensorEventLogTable();
	
	init_saHpiHotSwapEventLogTable();
	init_saHpiWatchdogEventLogTable();
	init_saHpiSoftwareEventLogTable();
	init_saHpiOEMEventLogTable();
	init_saHpiUserEventLogTable();
	init_saHpiSensorEnableChangeEventLogTable();
	init_saHpiDomainEventLogTable();
	
	init_saHpiHotSwapTable();
        init_saHpiAutoInsertTimeoutTable();
	init_saHpiAutoInsertTimeoutTable();
	init_saHpiAnnouncementTable();
	init_saHpiAnnouncementEventLogTable();


	if (send_traps_on_startup == AGENT_TRUE)
		send_traps = AGENT_TRUE;
	/* after initialization populate tables */
	populate_saHpiDomainInfoTable(sessionid);

	populate_saHpiDomainAlarmTable(sessionid);

	poplulate_saHpiDomainReferenceTable(sessionid);	

	populate_saHpiResourceTable(sessionid);
	    /* populate_saHpiResourceTable() calls:
	     *     populate_saHpiRdrTable(); calls:
	     *         populate_saHpiCtrlDigitalTable();		
	     *	       populate_saHpiCtrlDiscreteTable();		
	     *	       populate_saHpiCtrlAnalogTable();		
	     *	       populate_saHpiCtrlStreamTable();		
	     *	       populate_saHpiCtrlTextTable();		
	     *	       populate_saHpiCtrlOemTable();		
	     *	       populate_saHpiSensorTable();		
	     *	           populate_saHpiSesnorReadingMaxTable();		
	     *	           populate_saHpiSesnorReadingMinTable();		
	     *	           populate_saHpiSesnorReadingNominalTable();		
	     *	           populate_saHpiSesnorReadingNormalMaxTable();		
	     *	           populate_saHpiSesnorReadingNormalMinTable();		
	     *	           populate_saHpiSensorThdLowCriticalTable();		
	     *	           populate_saHpiSensorThdLowMajorTable();		
	     *	           populate_saHpiSensorThdLowMinorTable();		
	     *	           populate_saHpiSensorThdUpCriticalTable();		
	     *	           populate_saHpiSensorThdUpMajorTable();		
	     *	           populate_saHpiSensorThdUpMinorTable();		
	     *	           populate_saHpiSensorThdPosHysteresisTable();		
	     *	           populate_saHpiSensorThdNegHysteresisTable();		
	     *	       populate_saHpiCurrentSensorStateTable();		
	     *	       populate_saHpiInventoyTable();		
	     *	       populate_saHpiWatchdogTable();		
	     *	       populate_saHpiAnnunciatorTable();		
	     *	       populate_saHpiAreaTable();		
	     *	           populate_saHpiFieldTable(); 
             *         populate_saHpiHotSwapTable();
             *             populate_saHpiAutoInsertTimeoutTable();             
  	     *         populate_saHpiAnnouncementTable();
	     */
            
	populate_saHpiEventTable(sessionid);
            /* populate_saHpiResourceEventTable();
	     * populate_saHpiDomainEventTable();
	     * populate_saHpiSensorEventTable();
	     * populate_saHpiOemEventTable();
	     * populate_saHpiHotSwapEventTable();
	     * populate_saHpiWatchdogEventTable();
	     * populate_saHpiSoftwareEventTable();
	     * populate_saHpiSensorEnableChangeEventTable();
	     * populate_saHpiUserEventTable();
	     */
        populate_saHpiEventLogInfo(sessionid);
	    /* populate_saHpiEventLog (sessionid);
	     */
            /*
             * populate_saHpiResourceEventLogTable();
             * populate_saHpiSensorEventLogTable();
	     * populate_saHpiHotSwapEventLogTable();
	     * populate_saHpiWatchdogEventLogTable();
	     * populate_saHpiSoftwareEventLogTable();
	     * populate_saHpiOemEventLogTable();
	     * populate_saHpiUserEventLogTable();
	     * populate_saHpiSensorEnableChangeEventLogTable();
	     * populate_saHpiDomainEventLogTable();	     
             */

        /* Determine whether or not we're in threaded mode */
	env = getenv("OPENHPI_THREADED");
	if ((env == (char *)NULL) || (strcmp(env, "NO") == 0)) {
                DEBUGMSGTL ((AGENT, "Running in nonthreaded mode.  Configuring polling mechanism\n"));  
                run_threaded = SAHPI_FALSE;
		if (init_alarm() != AGENT_ERR_NOERROR) {
                    snmp_log (LOG_ERR, "Could not initialize polling mechanism. Exiting\n.");
                    rc = -1;
                    goto stop;
                }
	}	
	else {
                DEBUGMSGTL ((AGENT, "Running in threaded mode.  Spawing thread\n"));
		/* start event thread */
                set_run_threaded(TRUE);
                if (start_event_thread(&sessionid) != AGENT_ERR_NOERROR) {
                        snmp_log (LOG_ERR, "Could not start our internal loop . Exiting\n.");
                        rc = -1;
                        goto stop;
                }
	}	

        send_traps = AGENT_TRUE;
        /* If we're going to be a snmp master agent, initial the ports */

        if (!agentx_subagent)
                init_master_agent ();	/* open the port to listen on (defaults to udp:161) */
                
        if (do_fork == AGENT_TRUE) {
                if ((child = fork ()) < 0) {
                        snmp_log (LOG_ERR, "Could not fork!\n");
                        exit (-1);
                }
                if (child != 0)
                        exit (0);
        }

        /* In case we recevie a request to stop (kill -TERM or kill -INT) */
        keep_running = 1;
        signal (SIGTERM, stop_server);
        signal (SIGINT, stop_server);

                /* you're main loop here... */
        while (keep_running) {
                /* if you use select(), see snmp_select_info() in snmp_api(3) */
                /*     --- OR ---  */		
                rc = agent_check_and_process (1);			
		
        }
stop:
        DEBUGMSGTL ((AGENT,
        "WARNING: closeSaHpiSession: hpiSubagent.c: nolong implemented!"));
        //closeSaHpiSession();
        /* at shutdown time */
        snmp_log (LOG_INFO, "Stopping %s\n", version);
        snmp_shutdown (AGENT);
  
  return rc;
}


/**********************************************************************/
/**********************************************************************/
/**********************************************************************/
/**********************************************************************/
/**********************************************************************/
/**********************************************************************/
#if 0

/*
 * Count of newly added RPT, RDR, Events and SEL entries since last  populate_ call
 * to be global. Reset every time the respective populate_ call is made.
 */
u_long rdr_new_entry_count;
u_long rpt_new_entry_count;
u_long event_new_entry_count;
u_long sel_new_entry_count;

int build_full_oid (oid * prefix, size_t prefix_len,
		oid * column, size_t column_len,
		netsnmp_index * index,
		oid * out_oid, size_t in_len, size_t * out_len)
{

  int register len = prefix_len + column_len;
  int register i = 0;

  if (index)
    len += index->len;

  if (len > in_len)
    return AGENT_ERR_MEMORY_FAULT;

  *out_len = len;
  memcpy (out_oid, prefix, prefix_len * sizeof (oid));

  for (; i < column_len; i++)
    {
      out_oid[prefix_len + i] = column[i];
    }
  len = prefix_len + i;

  if (index)
    {
      for (i = 0; i < index->len; i++)
	{
	  out_oid[len + i] = index->oids[i];
	}
    }
  return AGENT_ERR_NOERROR;
}


netsnmp_variable_list *
build_notification (const netsnmp_index * index,
		    const trap_vars * var, const size_t var_len,
		    const oid * notification_oid,
		    const size_t notification_oid_len,
		    const oid * root_table_oid,
		    const size_t root_table_oid_len,
		    const SaHpiDomainIdT domain_id, const oid * domain_id_oid,
		    const size_t domain_id_oid_len,
		    const SaHpiResourceIdT resource_id,
		    const oid * resource_id_oid,
		    const size_t resource_id_oid_len)
{

  int i, j;
  netsnmp_variable_list *notification_vars = NULL;
  oid snmptrap[] = { snmptrap_oid };
  oid full_oid[MAX_OID_LEN];
  size_t full_oid_len;



  DEBUGMSGTL ((AGENT, "--- build_notification: Entry.\n"));


  if ((root_table_oid_len + 2 + index->len) > MAX_OID_LEN)
    {
      DEBUGMSGTL ((AGENT, "The length of the OID exceeds MAX_OID_LEN!\n"));
      return NULL;
    }

  // First is the Notification_OID
  snmp_varlist_add_variable (&notification_vars,
			     snmptrap, OID_LENGTH (snmptrap),
			     ASN_OBJECT_ID,
			     (const u_char *) (const oid *) notification_oid,
			     notification_oid_len * sizeof (oid));

  // Next the DomainID
  snmp_varlist_add_variable (&notification_vars,
			     domain_id_oid, domain_id_oid_len,
			     ASN_UNSIGNED,
			     (const u_char *) &domain_id,
			     sizeof (SaHpiDomainIdT));
  // The ResourceID
  snmp_varlist_add_variable (&notification_vars,
			     resource_id_oid, resource_id_oid_len,
			     ASN_UNSIGNED,
			     (const u_char *) &resource_id,
			     sizeof (SaHpiResourceIdT));

  // Attach the rest of information
  for (i = 0; i < var_len; i++)
    {

      // Generate the full OID.   
      memcpy (full_oid, root_table_oid, root_table_oid_len * sizeof (oid));
      full_oid_len = root_table_oid_len;
      // Get the column number.
      full_oid[full_oid_len++] = 1;
      full_oid[full_oid_len++] = var[i].column;
      // Put the index value in
      for (j = 0; j < index->len; j++)
	{
	  full_oid[full_oid_len + j] = index->oids[j];
	}
      full_oid_len += index->len;

      snmp_varlist_add_variable (&notification_vars,
				 full_oid,
				 full_oid_len,
				 var[i].type, var[i].value, var[i].value_len);

    }
  DEBUGMSGTL ((AGENT, "--- build_notification: Exit.\n"));
  return notification_vars;

}

long
calculate_hash_value (void *data, int len)
{

  register long hash = 0;
  register int i = 0;

  // WARNING. Can seg fault!
  // Pretty simple. Can also roll-over back to zero.

  //DEBUGMSGTL((AGENT,"HASH DATA:\n"));
  for (; i < len; i++, hash += ((char *) data)[i]);
  //{
  //printf("%X",((char *)data)[i]);
  //}
  //DEBUGMSGTL((AGENT,"HASH END\n"));
  return hash;
}

SaErrorT
rcSaHpi ()
{
  return err;
}

int
closeSaHpiSession ()
{

  DEBUGMSGTL ((AGENT, "--- closeSaHpiSession: Entry. "));
  if (session_avail == AGENT_TRUE)
    {
      err = saHpiUnsubscribe (session_id);
      if (SA_OK != err)
	{
	  snmp_log (LOG_ERR, "saHpiUnsubscirbe error: %s.\n",
                    oh_lookup_error(err));
	  return AGENT_ERR_SESSION_CLOSE;
	}
      err = saHpiFinalize ();
      if (SA_OK != err)
	{
	  snmp_log (LOG_ERR, "saHpiFinalize error: %s\n",
		    oh_lookup_error(err));
	  return AGENT_ERR_SESSION_CLOSE;
	}
      session_avail = AGENT_FALSE;
    }
  DEBUGMSGTL ((AGENT, "--- closeSaHpiSession: Exit. "));

  return AGENT_ERR_NOERROR;
}

int
getSaHpiSession (SaHpiSessionIdT * out)
{

  SaHpiVersionT version;

  DEBUGMSGTL ((AGENT, "--- getSaHpiSession: Entry. "));

  if (session_avail == AGENT_FALSE)
    {
      err = saHpiInitialize (&version);
      if (SA_OK != err)
	{
	  snmp_log (LOG_ERR, "saHpiInitialize error: %s\n",
		    oh_lookup_error(err));
	  return AGENT_ERR_INIT;
	}
      err = saHpiSessionOpen (SAHPI_DEFAULT_DOMAIN_ID, &session_id, NULL);
      if (SA_OK != err)
	{
	  snmp_log (LOG_ERR, "saHpiSessionOpen error: %s\n",
		    oh_lookup_error(err));
	  return AGENT_ERR_SESSION_OPEN;
	}

      err = saHpiSubscribe (session_id, SAHPI_FALSE);
      if (SA_OK != err)
	{
	  snmp_log (LOG_ERR, "saHpiSubscribe failed. error: %s\n",
		    oh_lookup_error(err));
	  return AGENT_ERR_SUBSCRIBE;
	}
      session_avail = AGENT_TRUE;
      //Subscribe to the Events
    }


  rediscover_count--;
  if (rediscover_count <= 0)
    {
      // Re-set the  rediscover_count;
      DEBUGMSGTL ((AGENT, "Calling 'saHpiResourceDiscover()'.\n"));
      rediscover_count = REDISCOVER_COUNT_MAX;
      err = saHpiResourcesDiscover (session_id);
      if (SA_OK != err)
	{
	  snmp_log (LOG_ERR, "saHpiResourcesDiscover error: %s\n",
		    oh_lookup_error(err));
	  return AGENT_ERR_DISCOVER;
	}
    }

  if (out)
    *out = session_id;

  DEBUGMSGTL ((AGENT, "--- getSaHpiSession: Exit\n"));
  return AGENT_ERR_NOERROR;
}

int
didSaHpiChanged (int *answer, SaHpiRptInfoT * info)
{

  int rc = AGENT_ERR_NOERROR;
  SaHpiRptInfoT rpt_info_new;
  SaErrorT err;

  DEBUGMSGTL ((AGENT, "--- didSaHpiChanged: Entry. "));

  if (session_avail == AGENT_FALSE)
    {
      if ((rc = getSaHpiSession (NULL)) != AGENT_ERR_NOERROR)
	{
	  // Something is screwy. Bail out
	  return rc;
	}
    }
  /* 
     err = saHpiResourcesDiscover (session_id);
     if (SA_OK != err)
     {
     snmp_log (LOG_ERR, "saHpiResourcesDiscover error: %s\n",
     oh_lookup_error(err));
     return AGENT_ERR_DISCOVER;
     }
   */
  err = saHpiRptInfoGet (session_id, &rpt_info_new);
  if (SA_OK != err)
    {
      snmp_log (LOG_ERR, "saHpiRptInfoGet error: %s\n",
		oh_lookup_error(err));
      return AGENT_ERR_RPTGET;
    }

  if ((rpt_info_new.UpdateCount != rpt_info.UpdateCount) ||
      (rpt_info_new.UpdateTimestamp != rpt_info.UpdateTimestamp))
    {
      // Something new.
      if (answer)
	{
	  *answer = AGENT_TRUE;
	  DEBUGMSGTL ((AGENT, "New UpdateCount: %d, TimeStamp: %d\n ",
		       rpt_info.UpdateCount, rpt_info.UpdateTimestamp));
	}
      rpt_info = rpt_info_new;
    }
  else
    {
      if (answer)
	*answer = AGENT_FALSE;
    }

  // Change regardless of answer.
  if (info)
    *info = rpt_info;

  DEBUGMSGTL ((AGENT, "--- didSaHpiChanged: Exit.\n"));
  return rc;

}
#endif
