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
#include <sys/types.h>
#include <unistd.h>

/*
 * Internal data for the sub-agent.
 */
static int keep_running;
static int session_avail = AGENT_FALSE;
static SaHpiSessionIdT session_id;
static SaHpiRptInfoT rpt_info;
static SaErrorT err;

static const char *version =
  "$Id$";
/*
 * Configuration options. Changed by config file.
 */

int send_traps = AGENT_FALSE;
static int send_traps_on_startup = AGENT_FALSE;

// Check for information every x seconds.
int alarm_interval = 10;

#define REDISCOVER_COUNT_MAX 10;
static int rediscover_count = 0;
// Max EVENT rows.
int MAX_EVENT_ENTRIES = 512;

//Use syslog 
static int do_syslog = AGENT_TRUE;



static RETSIGTYPE
stop_server (int a)
{
  keep_running = 0;
}

int
build_full_oid (oid * prefix, size_t prefix_len,
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
			     (u_char *) (oid *) notification_oid,
			     notification_oid_len * sizeof (oid));

  // Next the DomainID
  snmp_varlist_add_variable (&notification_vars,
			     domain_id_oid, domain_id_oid_len,
			     ASN_UNSIGNED,
			     (u_char *) & domain_id, sizeof (SaHpiDomainIdT));
  // The ResourceID
  snmp_varlist_add_variable (&notification_vars,
			     resource_id_oid, resource_id_oid_len,
			     ASN_UNSIGNED,
			     (u_char *) & resource_id,
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


const char *
get_error_string (SaErrorT error)
{
  switch (error)
    {
    case SA_ERR_HPI_ERROR:
      return "SA_ERR_HPI_ERROR";
    case SA_ERR_HPI_UNSUPPORTED_API:
      return "SA_ERR_UNSUPPORTED_API";
    case SA_ERR_HPI_BUSY:
      return "SA_ERR_HPI_BUSY";
    case SA_ERR_HPI_INVALID:
      return "SA_ERR_HPI_INVALID";
    case SA_ERR_HPI_INVALID_CMD:
      return "SA_ERR_HPI_INVALID_CMD";
    case SA_ERR_HPI_TIMEOUT:
      return "SA_ERR_HPI_TIMEOUT";
    case SA_ERR_HPI_OUT_OF_SPACE:
      return "SA_ERR_HPI_OUT_OF_SPACE";
    case SA_ERR_HPI_DATA_TRUNCATED:
      return "SA_ERR_HPI_DATA_TRUNCATED";
    case SA_ERR_HPI_DATA_LEN_INVALID:
      return "SA_ERR_HPI_DATA_LEN_INVALID";
    case SA_ERR_HPI_DATA_EX_LIMITS:
      return "SA_ERR_HPI_DATA_EX_LIMITS";
    case SA_ERR_HPI_INVALID_PARAMS:
      return "SA_ERR_HPI_INVALID_PARAMS";
    case SA_ERR_HPI_INVALID_DATA:
      return "SA_ERR_HPI_INVALID_DATA";
    case SA_ERR_HPI_NOT_PRESENT:
      return "SA_ERR_HPI_NOT_PRESENT";
    case SA_ERR_HPI_INVALID_DATA_FIELD:
      return "SA_ERR_HPI_INVALID_DATA_FIELD";
    case SA_ERR_HPI_INVALID_SENSOR_CMD:
      return "SA_ERR_HPI_INVALID_SENSOR_CMD";
    case SA_ERR_HPI_NO_RESPONSE:
      return "SA_ERR_HPI_NO_RESPONSE";
    case SA_ERR_HPI_DUPLICATE:
      return "SA_ERR_HPI_DUPLICATE";
    case SA_ERR_HPI_UPDATING:
      return "SA_ERR_HPI_UPDATING";
    case SA_ERR_HPI_INITIALIZING:
      return "SA_ERR_HPI_INITIALIZING";
    case SA_ERR_HPI_UNKNOWN:
      return "SA_ERR_HPI_UNKNOWN";
    case SA_ERR_HPI_INVALID_SESSION:
      return "SA_ERR_HPI_INVALID_SESSION";
    case SA_ERR_HPI_INVALID_DOMAIN:
      return "SA_ERR_HPI_INVALID_DOMAIN";
    case SA_ERR_HPI_INVALID_RESOURCE:
      return "SA_ERR_HPI_INVALID_RESOURCE";
    case SA_ERR_HPI_INVALID_REQUEST:
      return "SA_ERR_HPI_INVALID_REQUEST";
    case SA_ERR_HPI_ENTITY_NOT_PRESENT:
      return "SA_ERR_HPI_ENTITY_NOT_PRESENT";
    case SA_ERR_HPI_UNINITIALIZED:
      return "SA_ERR_HPI_UNINITIALIZED";
    default:
      DEBUGMSGTL ((AGENT, "Invalid error code=%d\n", error));
      return "(Invalid error code)";
    }
}


static gchar *eshort_names[] = {
  "UNSPECIFIED",
  "OTHER",
  "UNKNOWN",
  "PROCESSOR",
  "DISK_BAY",
  "PERIPHERAL_BAY",
  "SYS_MGMNT_MODULE",
  "SYSTEM_BOARD",
  "MEMORY_MODULE",
  "PROCESSOR_MODULE",
  "POWER_SUPPLY",
  "ADD_IN_CARD",
  "FRONT_PANEL_BOARD",
  "BACK_PANEL_BOARD",
  "POWER_SYSTEM_BOARD",
  "DRIVE_BACKPLANE",
  "SYS_EXPANSION_BOARD",
  "OTHER_SYSTEM_BOARD",
  "PROCESSOR_BOARD",
  "POWER_UNIT",
  "POWER_MODULE",
  "POWER_MGMNT",
  "CHASSIS_BACK_PANEL_BOARD",
  "SYSTEM_CHASSIS",
  "SUB_CHASSIS",
  "OTHER_CHASSIS_BOARD",
  "DISK_DRIVE_BAY",
  "PERIPHERAL_BAY_2",
  "DEVICE_BAY",
  "COOLING_DEVICE",
  "COOLING_UNIT",
  "INTERCONNECT",
  "MEMORY_DEVICE",
  "SYS_MGMNT_SOFTWARE",
  "BIOS",
  "OPERATING_SYSTEM",
  "SYSTEM_BUS",
  "GROUP",
  "REMOTE",
  "EXTERNAL_ENVIRONMENT",
  "BATTERY",
  "CHASSIS_SPECIFIC",		/* Jumps to 144 */
  "BOARD_SET_SPECIFIC",		/* Jumps to 176 */
  "OEM_SYSINT_SPECIFIC",	/* Jumps to 208 */
  "ROOT",			/* Jumps to 65535 and continues from there... */
  "RACK",
  "SUBRACK",
  "COMPACTPCI_CHASSIS",
  "ADVANCEDTCA_CHASSIS",
  "SYSTEM_SLOT",
  "SBC_BLADE",
  "IO_BLADE",
  "DISK_BLADE",
  "DISK_DRIVE",
  "FAN",
  "POWER_DISTRIBUTION_UNIT",
  "SPEC_PROC_BLADE",
  "IO_SUBBOARD",
  "SBC_SUBBOARD",
  "ALARM_MANAGER",
  "ALARM_MANAGER_BLADE",
  "SUBBOARD_CARRIER_BLADE",
};

#define EPATHSTRING_START_DELIMITER "{"
#define EPATHSTRING_END_DELIMITER "}"
#define EPATHSTRING_VALUE_DELIMITER ","
#define ESHORTNAMES_BEFORE_JUMP 40
#define ESHORTNAMES_FIRST_JUMP 41
#define ESHORTNAMES_SECOND_JUMP 42
#define ESHORTNAMES_THIRD_JUMP 43
#define ESHORTNAMES_LAST_JUMP 44


static unsigned int
entitytype2index (unsigned int i)
{
  if (i <= ESHORTNAMES_BEFORE_JUMP)
    {
      return i;
    }
  else if (i == (unsigned int) SAHPI_ENT_CHASSIS_SPECIFIC)
    {
      return ESHORTNAMES_FIRST_JUMP;
    }
  else if (i == (unsigned int) SAHPI_ENT_BOARD_SET_SPECIFIC)
    {
      return ESHORTNAMES_SECOND_JUMP;
    }
  else if (i == (unsigned int) SAHPI_ENT_OEM_SYSINT_SPECIFIC)
    {
      return ESHORTNAMES_THIRD_JUMP;
    }
  else
    {
      return (i - (unsigned int) SAHPI_ENT_ROOT + ESHORTNAMES_LAST_JUMP);
    }
}

/***********************************************************************
 * entitypath2string
 *  
 * Parameters:
 *   epathptr IN   Pointer to HPI's entity path structure
 *   epathstr OUT  Pointer to canonical entity path string
 *   strsize  IN   Canonical string length
 *
 * Returns:
 *   >0  Number of characters written to canonical entity path string
 *   -1  Error return
 ***********************************************************************/
int
entitypath2string (const SaHpiEntityPathT * epathptr, gchar * epathstr,
		   const gint strsize)
{

  gchar *instance_str, *catstr, *tmpstr;
  gint err, i, strcount = 0, rtncode = 0;

  if (epathstr == NULL || strsize <= 0)
    {
      DEBUGMSGTL ((AGENT, "Null string or invalid string size"));
      return (-1);
    }

  if (epathptr == NULL)
    {
      epathstr = "";
      return 0;
    }

  instance_str = (gchar *) g_malloc0 (MAX_INSTANCE_DIGITS + 1);
  tmpstr = (gchar *) g_malloc0 (strsize);
  if (instance_str == NULL || tmpstr == NULL)
    {
      DEBUGMSGTL ((AGENT, "Out of memory"));
      rtncode = -1;
      goto CLEANUP;
    }

  for (i = (SAHPI_MAX_ENTITY_PATH - 1); i >= 0; i--)
    {
      guint num_digits, work_instance_num;

      /* Find last element of structure; Current choice not good,
         since type=instance=0 is valid */
      if (epathptr->Entry[i].EntityType == 0)
	{
	  continue;
	}

      /* Validate and convert data */
      work_instance_num = epathptr->Entry[i].EntityInstance;
      for (num_digits = 1; (work_instance_num = work_instance_num / 10) > 0;
	   num_digits++);

      if (num_digits > MAX_INSTANCE_DIGITS)
	{
	  rtncode = -1;
	  goto CLEANUP;
	}
      memset (instance_str, 0, MAX_INSTANCE_DIGITS + 1);
      err = snprintf (instance_str, MAX_INSTANCE_DIGITS + 1,
		      "%d", epathptr->Entry[i].EntityInstance);

      strcount = strcount +
	strlen (EPATHSTRING_START_DELIMITER) +
	strlen (eshort_names
		[entitytype2index (epathptr->Entry[i].EntityType)]) +
	strlen (EPATHSTRING_VALUE_DELIMITER) + strlen (instance_str) +
	strlen (EPATHSTRING_END_DELIMITER);

      if (strcount > strsize - 1)
	{
	  rtncode = -1;
	  goto CLEANUP;
	}

      catstr = g_strconcat (EPATHSTRING_START_DELIMITER,
			    eshort_names[entitytype2index
					 (epathptr->Entry[i].EntityType)],
			    EPATHSTRING_VALUE_DELIMITER, instance_str,
			    EPATHSTRING_END_DELIMITER, NULL);

      strcat (tmpstr, catstr);
      g_free (catstr);
    }
  rtncode = strcount;
  /* Write string */
  memset (epathstr, 0, strsize);
  strcpy (epathstr, tmpstr);

CLEANUP:
  g_free (instance_str);
  g_free (tmpstr);

  return (rtncode);
}				/* End entitypath2string */

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
		    get_error_string (err));
	  return AGENT_ERR_SESSION_CLOSE;
	}
      err = saHpiFinalize ();
      if (SA_OK != err)
	{
	  snmp_log (LOG_ERR, "saHpiFinalize error: %s\n",
		    get_error_string (err));
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
		    get_error_string (err));
	  return AGENT_ERR_INIT;
	}
      err = saHpiSessionOpen (SAHPI_DEFAULT_DOMAIN_ID, &session_id, NULL);
      if (SA_OK != err)
	{
	  snmp_log (LOG_ERR, "saHpiSessionOpen error: %s\n",
		    get_error_string (err));
	  return AGENT_ERR_SESSION_OPEN;
	}

      err = saHpiSubscribe (session_id, SAHPI_FALSE);
      if (SA_OK != err)
	{
	  snmp_log (LOG_ERR, "saHpiSubscribe failed. error: %s\n",
		    get_error_string (err));
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
		    get_error_string (err));
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
     get_error_string (err));
     return AGENT_ERR_DISCOVER;
     }
   */
  err = saHpiRptInfoGet (session_id, &rpt_info_new);
  if (SA_OK != err)
    {
      snmp_log (LOG_ERR, "saHpiRptInfoGet error: %s\n",
		get_error_string (err));
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


void
hpiSubagent_parse_config_traps (const char *token, char *cptr)
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
      sprintf (buf, "hpiSubagent: '%s' unrecognized", cptr);
      config_perror (buf);
    }
  else
    {

      send_traps_on_startup = x;
    }
}

void
hpiSubagent_parse_config_interval (const char *token, char *cptr)
{
  int x = atoi (cptr);
  char buf[BUFSIZ];

  if (x < -1)
    {
      sprintf (buf, "hpiSubagent: '%s' unrecognized", cptr);
      config_perror (buf);
    }
  else
    {
      snmp_log (LOG_INFO, "Checking HPI infrastructure every %d seconds.\n",
		x);
      alarm_interval = x;
    }
}
void
hpiSubagent_parse_config_max_event (const char *token, char *cptr)
{
  int x = atoi (cptr);
  char buf[BUFSIZ];

  if (x < -1)
    {
      sprintf (buf, "hpiSubagent: '%s' unrecognized", cptr);
      config_perror (buf);
    }
  else
    {
      snmp_log (LOG_INFO, "Max Event rows %d.\n", x);
      MAX_EVENT_ENTRIES = x;
    }
}



int
main (int argc, char **argv)
{
  int agentx_subagent = AGENT_TRUE;
  char c;
  int rc = 0;
  /* change this if you want to be a SNMP master agent */

  while ((c = getopt (argc, argv, "ds?")) != EOF)
    switch (c)
      {
      case 'd':
	debug_register_tokens (AGENT);
	snmp_enable_stderrlog ();
	snmp_set_do_debugging (1);
	break;
	break;
      case 's':
	do_syslog = AGENT_FALSE;
	break;
      default:
	printf ("Usage %s [-dfs]\n", argv[0]);
	printf ("where -d enables debug mode\n");
	printf ("where -s disables logging via syslog facility.\n");
	exit (1);
      }
  init_snmp_logging ();
  if (do_syslog == AGENT_TRUE)
    {
      snmp_enable_calllog ();
      snmp_enable_syslog_ident (AGENT, LOG_DAEMON);
    }
  snmp_log (LOG_INFO, "Starting %s\n", version);
  /* we're an agentx subagent? */
  if (agentx_subagent)
    {
      /* make us a agentx client. */
      rc = netsnmp_ds_set_boolean (NETSNMP_DS_APPLICATION_ID,
				   NETSNMP_DS_AGENT_ROLE, 1);
    }

  /* initialize the agent library */
  rc = init_agent (AGENT);
  if (rc != 0)
    {
      snmp_log (LOG_ERR, "Could not initialize connection to SNMP daemon. \n"
		"Perhaps you are running %s as non-root?\n", argv[0]);
      exit (rc);
    }

  /* Read configuration information here, before we initialize */

  snmpd_register_config_handler (TRAPS_TOKEN,
				 hpiSubagent_parse_config_traps,
				 NULL,
				 "hpiSubagent on/off switch for sending events upon startup");

  snmpd_register_config_handler (INTERVAL_TOKEN,
				 hpiSubagent_parse_config_interval,
				 NULL,
				 "hpiSubagent time in seconds before HPI API is queried for information.");

  snmpd_register_config_handler (MAX_EVENT_TOKEN,
				 hpiSubagent_parse_config_max_event,
				 NULL,
				 "hpiSubagent MAX number of rows for Events.");

  init_snmp (AGENT);
  /* Initialize tables */
  initialize_table_saHpiTable ();
  initialize_table_saHpiRdrTable ();
  initialize_table_saHpiSensorTable ();
  initialize_table_saHpiCtrlTable ();
  initialize_table_saHpiInventoryTable ();
  initialize_table_saHpiWatchdogTable ();
  initialize_table_saHpiHotSwapTable ();

  initialize_table_saHpiSystemEventLogTable ();
  initialize_table_saHpiEventTable ();

  if (send_traps_on_startup == AGENT_TRUE)
    send_traps = AGENT_TRUE;

  if (populate_rpt () != AGENT_ERR_NOERROR)
    {
      snmp_log (LOG_ERR, "Could not retrieve RPT entries. Exiting\n.");
      rc = -1;
      goto stop;
    }
  populate_event ();

  if (init_alarm () != AGENT_ERR_NOERROR)
    {
      snmp_log (LOG_ERR, "Could not start our internal loop . Exiting\n.");
      rc = -1;
      goto stop;
    }
  /* If we're going to be a snmp master agent, initial the ports */

  if (!agentx_subagent)
    init_master_agent ();	/* open the port to listen on (defaults to udp:161) */


  /* In case we recevie a request to stop (kill -TERM or kill -INT) */
  keep_running = 1;
  signal (SIGTERM, stop_server);
  signal (SIGINT, stop_server);

  /* you're main loop here... */
  while (keep_running)
    {
      /* if you use select(), see snmp_select_info() in snmp_api(3) */
      /*     --- OR ---  */
      rc = agent_check_and_process (1);	/* 0 == don't block */
    }
stop:
  /* at shutdown time */
  snmp_log (LOG_INFO, "Stopping %s\n", version);
  snmp_shutdown (AGENT);

  return rc;
}
