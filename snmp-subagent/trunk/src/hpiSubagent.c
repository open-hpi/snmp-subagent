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

#include "alarm.h"
#include <oh_error.h>

#include <hpiB_columns.h>
#include <hpiB_enums.h>
#include <hpiB.h>

#include <saHpiDomainInfoTable.h>
#include <saHpiDomainReferenceTable.h>
#include <saHpiDomainAlarmTable.h>
#include <saHpiResourceTable.h>
#include <saHpiAutoTimeOutTable.h>
#include <saHpiHotSwapTable.h>

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

#include <saHpiRdrTable.h>
#include <saHpiCtrlTable.h>
#include <saHpiSensorTable.h>

#include <saHpiSensorReadingCurrentTable.h>
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

#include <saHpiInventoryTable.h>
#include <saHpiAreaTable.h>
#include <saHpiFieldTable.h>
#include <saHpiWatchdogTable.h>
#include <saHpiAnnunciatorTable.h>

#include <hpiSubagent.h>
#include <alarm.h>
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>

/*
 * Internal prototypes
 */
static void usage(char *applName);


/*
 * Internal data for the sub-agent.
 */
static int keep_running;
static int session_avail = AGENT_FALSE;
//static SaHpiSessionIdT session_id;
//static SaHpiRptInfoT rpt_info;
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
static int do_fork = AGENT_FALSE;


/*
 * Count of newly added RPT, RDR, Events and SEL entries since last  populate_ call
 * to be global. Reset every time the respective populate_ call is made.
 */
u_long rdr_new_entry_count;
u_long rpt_new_entry_count;
u_long event_new_entry_count;
u_long sel_new_entry_count;

static RETSIGTYPE
stop_server (int a)
{
  keep_running = 0;
}

#if 0


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
static state_category_string state_string[] = {
  {SAHPI_EC_UNSPECIFIED, SAHPI_ES_UNSPECIFIED, "UNSPECIFIED"},
  {SAHPI_EC_THRESHOLD, SAHPI_ES_LOWER_MINOR, "LOWER_MINOR"},
  {SAHPI_EC_THRESHOLD, SAHPI_ES_LOWER_MAJOR, "LOWER_MAJOR"},
  {SAHPI_EC_THRESHOLD, SAHPI_ES_LOWER_CRIT, "LOWER_CRIT"},
  {SAHPI_EC_THRESHOLD, SAHPI_ES_UPPER_MINOR, "UPPER_MINOR"},
  {SAHPI_EC_THRESHOLD, SAHPI_ES_UPPER_MAJOR, "UPPER_MAJOR"},
  {SAHPI_EC_THRESHOLD, SAHPI_ES_UPPER_CRIT, "UPPER_CRIT"},
  {SAHPI_EC_USAGE, SAHPI_ES_IDLE, "IDLE"},
  {SAHPI_EC_USAGE, SAHPI_ES_ACTIVE, "ACTIVE"},
  {SAHPI_EC_USAGE, SAHPI_ES_BUSY, "BUSY"},
  {SAHPI_EC_STATE, SAHPI_ES_STATE_DEASSERTED, "STATE_DEASSERTED"},
  {SAHPI_EC_STATE, SAHPI_ES_STATE_ASSERTED, "STATE_ASSERTED"},
  {SAHPI_EC_PRED_FAIL, SAHPI_ES_PRED_FAILURE_DEASSERT,
   "PRED_FAILURE_DEASSERT"},
  {SAHPI_EC_PRED_FAIL, SAHPI_ES_PRED_FAILURE_ASSERT, "PRED_FAILURE_ASSERT"},
  {SAHPI_EC_LIMIT, SAHPI_ES_LIMIT_NOT_EXCEEDED, "LIMIT_NOT_EXCEEDED"},
  {SAHPI_EC_LIMIT, SAHPI_ES_LIMIT_EXCEEDED, "LIMIT_EXCEEDED"},
  {SAHPI_EC_PERFORMANCE, SAHPI_ES_PERFORMANCE_MET, "PERFORMANCE_MET"},
  {SAHPI_EC_PERFORMANCE, SAHPI_ES_PERFORMANCE_LAGS, "PERFORMANCE_LAGS"},
  {SAHPI_EC_SEVERITY, SAHPI_ES_OK, "OK"},
  {SAHPI_EC_SEVERITY, SAHPI_ES_MINOR_FROM_OK, "MINOR_FROM_OK"},
  {SAHPI_EC_SEVERITY, SAHPI_ES_MAJOR_FROM_LESS, "MAJOR_FROM_LESS"},
  {SAHPI_EC_SEVERITY, SAHPI_ES_CRITICAL_FROM_LESS, "CRITICAL_FROM_LESS"},
  {SAHPI_EC_SEVERITY, SAHPI_ES_MINOR_FROM_MORE, "MINOR_FROM_MORE"},
  {SAHPI_EC_SEVERITY, SAHPI_ES_MAJOR_FROM_CRITICAL, "MAJOR_FROM_CRITICAL"},
  {SAHPI_EC_SEVERITY, SAHPI_ES_CRITICAL, "CRITICAL"},
  {SAHPI_EC_SEVERITY, SAHPI_ES_MONITOR, "MONITOR"},
  {SAHPI_EC_SEVERITY, SAHPI_ES_INFORMATIONAL, "INFORMATIONAL"},
  {SAHPI_EC_PRESENCE, SAHPI_ES_ABSENT, "ABSENT"},
  {SAHPI_EC_PRESENCE, SAHPI_ES_PRESENT, "PRESENT"},
  {SAHPI_EC_ENABLE, SAHPI_ES_DISABLED, "DISABLED"},
  {SAHPI_EC_ENABLE, SAHPI_ES_ENABLED, "ENABLED"},
  {SAHPI_EC_AVAILABILITY, SAHPI_ES_RUNNING, "RUNNING"},
  {SAHPI_EC_AVAILABILITY, SAHPI_ES_TEST, "TEST"},
  {SAHPI_EC_AVAILABILITY, SAHPI_ES_POWER_OFF, "POWER_OFF"},
  {SAHPI_EC_AVAILABILITY, SAHPI_ES_ON_LINE, "ON_LINE"},
  {SAHPI_EC_AVAILABILITY, SAHPI_ES_OFF_LINE, "OFF_LINE"},
  {SAHPI_EC_AVAILABILITY, SAHPI_ES_OFF_DUTY, "OFF_DUTY"},
  {SAHPI_EC_AVAILABILITY, SAHPI_ES_DEGRADED, "DEGRADED"},
  {SAHPI_EC_AVAILABILITY, SAHPI_ES_POWER_SAVE, "POWER_SAVE"},
  {SAHPI_EC_AVAILABILITY, SAHPI_ES_INSTALL_ERROR, "INSTALL_ERROR"},
  {SAHPI_EC_REDUNDANCY, SAHPI_ES_FULLY_REDUNDANT, "FULLY_REDUNDANT"},
  {SAHPI_EC_REDUNDANCY, SAHPI_ES_REDUNDANCY_LOST, "REDUNDANCY_LOST"},
  {SAHPI_EC_REDUNDANCY, SAHPI_ES_REDUNDANCY_DEGRADED, "REDUNDANCY_DEGRADED"},
  {SAHPI_EC_REDUNDANCY, SAHPI_ES_REDUNDANCY_LOST_SUFFICIENT_RESOURCES,
   "REDUNDANCY_LOST_SUFFICIENT_RESOURCES"},
  {SAHPI_EC_REDUNDANCY, SAHPI_ES_NON_REDUNDANT_SUFFICIENT_RESOURCES,
   "NON_REDUNDANT_SUFFICIENT_RESOURCES"},
  {SAHPI_EC_REDUNDANCY, SAHPI_ES_NON_REDUNDANT_INSUFFICIENT_RESOURCES,
   "NON_REDUNDANT_INSUFFICIENT_RESOURCES"},
  {SAHPI_EC_REDUNDANCY, SAHPI_ES_REDUNDANCY_DEGRADED_FROM_FULL,
   "REDUNDANCY_DEGRADED_FROM_FULL"},
  {SAHPI_EC_REDUNDANCY, SAHPI_ES_REDUNDANCY_DEGRADED_FROM_NON,
   "REDUNDANCY_DEGRADED_FROM_NON"},
  {SAHPI_EC_GENERIC, SAHPI_ES_STATE_00, "STATE_00"},
  {SAHPI_EC_GENERIC, SAHPI_ES_STATE_01, "STATE_01"},
  {SAHPI_EC_GENERIC, SAHPI_ES_STATE_02, "STATE_02"},
  {SAHPI_EC_GENERIC, SAHPI_ES_STATE_03, "STATE_03"},
  {SAHPI_EC_GENERIC, SAHPI_ES_STATE_04, "STATE_04"},
  {SAHPI_EC_GENERIC, SAHPI_ES_STATE_05, "STATE_05"},
  {SAHPI_EC_GENERIC, SAHPI_ES_STATE_06, "STATE_06"},
  {SAHPI_EC_GENERIC, SAHPI_ES_STATE_07, "STATE_07"},
  {SAHPI_EC_GENERIC, SAHPI_ES_STATE_08, "STATE_08"},
  {SAHPI_EC_GENERIC, SAHPI_ES_STATE_09, "STATE_09"},
  {SAHPI_EC_GENERIC, SAHPI_ES_STATE_10, "STATE_10"},
  {SAHPI_EC_GENERIC, SAHPI_ES_STATE_11, "STATE_11"},
  {SAHPI_EC_GENERIC, SAHPI_ES_STATE_12, "STATE_12"},
  {SAHPI_EC_GENERIC, SAHPI_ES_STATE_13, "STATE_13"},
  {SAHPI_EC_GENERIC, SAHPI_ES_STATE_14, "STATE_14"}
};

#define STATESTRING_VALUE_DELIMITER ", "
#define STATESTRING_VALUE_DELIMITER_LENGTH 2
#define STATESTRING_MAX_LENGTH 1000
#define STATESTRING_MAX_ENTRIES 63

int
build_state_string (SaHpiEventCategoryT category,
		    SaHpiEventStateT state,
		    unsigned char *str, size_t * len, size_t max_len)
{

  char *temp;
  size_t idx = 0;
  size_t temp_len;
  int i;

  int rc = AGENT_ERR_NOERROR;
  *len = 0;
  idx = 0;
  temp = (char *) malloc (STATESTRING_MAX_LENGTH);
  if (temp == NULL)
    return AGENT_ERR_MEMORY_FAULT;

  if (category == SAHPI_EC_USER)
    category = SAHPI_EC_GENERIC;
  for (i = 0; i < STATESTRING_MAX_ENTRIES; i++)
    {
      /* snmp_log (LOG_INFO, "How about this one? : %d %d\n", state_string[i].category,
         category); */
      if (state_string[i].category == category)
	{
	  /*
	     snmp_log (LOG_INFO, "Matched : %d %d\n", state_string[i].category,
	     category);
	   */
	  /* Found category, time to match states. */
	  /* Match the right states */
	  if ((state_string[i].state & state) == state_string[i].state)
	    {
	      /* Found it 
	         snmp_log (LOG_INFO, "Matched state: %d %d\n", state_string[i].state,state);
	       */
	      temp_len = strlen (state_string[i].str);
	      if (idx + temp_len + STATESTRING_VALUE_DELIMITER_LENGTH >
		  max_len)
		{
		  rc = AGENT_ERR_MEMORY_FAULT;
		  break;
		}
	      memcpy (temp + idx, state_string[i].str, temp_len);
	      idx = idx + temp_len;
	      memcpy (temp + idx, STATESTRING_VALUE_DELIMITER,
		      STATESTRING_VALUE_DELIMITER_LENGTH);
	      idx = idx + STATESTRING_VALUE_DELIMITER_LENGTH;
	    }
	}
    }

  if (idx > 0)
    idx = idx - STATESTRING_VALUE_DELIMITER_LENGTH;

  if (idx < max_len)
    temp[idx] = 0x00;
  else
    temp[max_len] = 0x00;


  memcpy (str, temp, idx);
  *len = idx;

  free (temp);
  temp = NULL;
  return rc;
}

int
build_state_value (unsigned char *str, size_t len, SaHpiEventStateT * state)
{

  int rc = AGENT_ERR_NOERROR;
  char *s = NULL;
  char *delim = NULL;
  char *tok = NULL;
  int i = 0;

  s = (char *) malloc (len);
  if (s == NULL)
    return AGENT_ERR_MEMORY_FAULT;

  delim = (char *) malloc (STATESTRING_VALUE_DELIMITER_LENGTH);
  if (delim == NULL)
    {
      free (s);
      return AGENT_ERR_MEMORY_FAULT;
    }

  memcpy (s, str, len);
  s[len] = 0x00;
  memcpy (delim, STATESTRING_VALUE_DELIMITER,
	  STATESTRING_VALUE_DELIMITER_LENGTH);
  delim[STATESTRING_VALUE_DELIMITER_LENGTH] = 0x00; 
  tok = strtok (s, delim);
  while (tok != NULL)
    {

      /*  snmp_log(LOG_INFO,"Tok: [%s]\n", tok); */
      for (i = 0; i < STATESTRING_MAX_ENTRIES; i++)
	{
	  if (strncasecmp
	      (tok, state_string[i].str, strlen (state_string[i].str)) == 0)
	    {
	      /*
	         snmp_log(LOG_INFO,"Matched: %X [%s] = [%s]\n", 
	         state_string[i].state,
	         state_string[i].str,
	         tok);
	       */
	      *state = *state + state_string[i].state;
	    }
	}
      tok = strtok (NULL, delim);
    }

  free (s);
  free (delim);
  return rc;
}


int
build_reading_strings (SaHpiSensorReadingT * reading,
		       SaHpiEventCategoryT sensor_category,
		       long *values_present,
		       long *raw_reading,
		       unsigned char *interpreted_reading,
		       size_t * interpreted_reading_len,
		       size_t interpreted_reading_max,
		       long *sensor_status,
		       unsigned char *event_status,
		       size_t * event_status_len, size_t event_status_max)
{

  char format[SENSOR_READING_MAX_LEN];
  size_t len;

  if (values_present) {
    *values_present = reading->ValuesPresent + 1;
    if (*values_present == 1) 
	*values_present = 0;
  }
  if (raw_reading) {
    if (reading->ValuesPresent & SAHPI_SRF_RAW)
      *raw_reading = reading->Raw;
    else
      *raw_reading = 0;
  }
  if (interpreted_reading_len) 
    *interpreted_reading_len = 0;

  if (event_status_len)
    *event_status_len = 0;
  if (event_status)
    *event_status = 0;
  

  /* 
   *       SaHpiSensorInterpretedT     Interpreted;
   * is always converted to a string buffer.
   */

  if (reading->ValuesPresent & SAHPI_SRF_INTERPRETED)
    {

      if (reading->Interpreted.Type == SAHPI_SENSOR_INTERPRETED_TYPE_BUFFER)
	{
	  if (interpreted_reading) {
	    memcpy (interpreted_reading,
		    &reading->Interpreted.Value.SensorBuffer,
		    SAHPI_SENSOR_BUFFER_LENGTH);
	    /*
	     * Old code: 
	    *interpreted_reading_len = SAHPI_SENSOR_BUFFER_LENGTH;
	     * Check to see how much of the string is actually 0x00 and 
	     * do not count those
	     */
	    len = strlen(reading->Interpreted.Value.SensorBuffer);
	    *interpreted_reading_len = ( len < SAHPI_SENSOR_BUFFER_LENGTH ) ? len : SAHPI_SENSOR_BUFFER_LENGTH;
	  }
	}

      else
	{
	  memset (&format, 0x00, SENSOR_READING_MAX_LEN);
	  /* Setting up the format  - %d or %u or %f .. etc */
	  switch (reading->Interpreted.Type)
	    {
	    case SAHPI_SENSOR_INTERPRETED_TYPE_INT8:
	    case SAHPI_SENSOR_INTERPRETED_TYPE_INT16:
	    case SAHPI_SENSOR_INTERPRETED_TYPE_INT32:
	      strncpy (format,
		       SENSOR_READING_SIGNED_INT,
		       SENSOR_READING_SIGNED_INT_LEN);
	      break;
	    case SAHPI_SENSOR_INTERPRETED_TYPE_UINT8:
	    case SAHPI_SENSOR_INTERPRETED_TYPE_UINT16:
	    case SAHPI_SENSOR_INTERPRETED_TYPE_UINT32:
	      strncpy (format,
		       SENSOR_READING_UNSIGNED_INT,
		       SENSOR_READING_UNSIGNED_INT_LEN);

	      break;

	    case SAHPI_SENSOR_INTERPRETED_TYPE_FLOAT32:
	      strncpy (format,
		       SENSOR_READING_FLOAT, SENSOR_READING_FLOAT_LEN);
	      break;

	    default:
	      break;
	    }
	  /* Done with setting up the format. Parsing the value.
	   * This could be done using a void pointer, but where 
	   * would I put the type information? Cast it back to its type?
	   *
	   * Any ideas?
	   */
	  if (interpreted_reading) {
	    switch (reading->Interpreted.Type)
	      {
	      case SAHPI_SENSOR_INTERPRETED_TYPE_INT8:
		*interpreted_reading_len =
		  snprintf (interpreted_reading,
			    interpreted_reading_max,
			    format, reading->Interpreted.Value.SensorInt8);
		break;
	      case SAHPI_SENSOR_INTERPRETED_TYPE_INT16:
		*interpreted_reading_len =
		snprintf (interpreted_reading,
			  interpreted_reading_max,
			  format, reading->Interpreted.Value.SensorInt16);
	      break;
	    case SAHPI_SENSOR_INTERPRETED_TYPE_INT32:
	      *interpreted_reading_len =
		snprintf (interpreted_reading,
			  interpreted_reading_max,
			  format, reading->Interpreted.Value.SensorInt32);
	      break;
	    case SAHPI_SENSOR_INTERPRETED_TYPE_UINT8:
	      *interpreted_reading_len =
		snprintf (interpreted_reading,
			  interpreted_reading_max,
			  format, reading->Interpreted.Value.SensorUint8);
	      break;
	    case SAHPI_SENSOR_INTERPRETED_TYPE_UINT16:
	      *interpreted_reading_len =
		snprintf (interpreted_reading,
			  interpreted_reading_max,
			  format, reading->Interpreted.Value.SensorUint16);
	      break;
	    case SAHPI_SENSOR_INTERPRETED_TYPE_UINT32:
	      *interpreted_reading_len =
		snprintf (interpreted_reading,
			  interpreted_reading_max,
			  format, reading->Interpreted.Value.SensorUint32);
	      break;
	    case SAHPI_SENSOR_INTERPRETED_TYPE_FLOAT32:
	      *interpreted_reading_len =
		snprintf (interpreted_reading,
			  interpreted_reading_max,
			  format, reading->Interpreted.Value.SensorFloat32);
	      break;

	    default:
	      break;
	    }

	  *interpreted_reading_len =
	    (*interpreted_reading_len >
	     interpreted_reading_max) ? interpreted_reading_max :
	    *interpreted_reading_len;

	  }
	}
    }
  if (reading->ValuesPresent & SAHPI_SRF_EVENT_STATE)
    {
      if (sensor_status)
	*sensor_status = reading->EventStatus.SensorStatus + 1;
      if (event_status || event_status_len) 
	build_state_string (sensor_category,
			    reading->EventStatus.EventStatus,
			    event_status, event_status_len, event_status_max);
    }
  return AGENT_ERR_NOERROR;
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

static unsigned int eshort_num_names = sizeof( eshort_names ) / sizeof(gchar * );
static int entitytype2index(unsigned int i)
{
        if(i <= ESHORTNAMES_BEFORE_JUMP)
                return i;
        else if (i == (unsigned int)SAHPI_ENT_CHASSIS_SPECIFIC)
                return ESHORTNAMES_FIRST_JUMP;
        else if (i == (unsigned int)SAHPI_ENT_BOARD_SET_SPECIFIC)
                return ESHORTNAMES_SECOND_JUMP;
        else if (i == (unsigned int)SAHPI_ENT_OEM_SYSINT_SPECIFIC)
                return ESHORTNAMES_THIRD_JUMP;
        else if (i >= (unsigned int)SAHPI_ENT_ROOT && i - (unsigned int)SAHPI_ENT_ROOT
                   < eshort_num_names - ESHORTNAMES_LAST_JUMP )
                return i - (unsigned int)SAHPI_ENT_ROOT + ESHORTNAMES_LAST_JUMP;

        return -1;
}

/**
 * entitypath2string
 * @epathptr: IN. Pointer to HPI's entity path structure
 * @epathstr: OUT. Pointer to canonical entity path string
 * @strsize: IN. Canonical string length
 *
 * Converts an entity path structure into its
 * canonical string version. 
 *
 * Returns: >0 Number of characters written to canonical entity path string, 
 * -1 Error return
 */
int entitypath2string(const SaHpiEntityPathT *epathptr, gchar *epathstr, const gint strsize)
{
     
	gchar  *instance_str, *catstr, *tmpstr;
	gint   err, i, strcount = 0, rtncode = 0;
        int tidx;
        gchar *type_str;
        gchar type_str_buffer[20];

	if (epathstr == NULL || strsize <= 0) { 
		DEBUGMSGTL((AGENT,"Null string or invalid string size")); 
		return -1;
	}

        if (epathptr == NULL) {
                *epathstr = '\0';
                return 0;
        }

	instance_str = (gchar *)g_malloc0(MAX_INSTANCE_DIGITS + 1);
	tmpstr = (gchar *)g_malloc0(strsize);
	if (instance_str == NULL || tmpstr == NULL) { 
		DEBUGMSGTL((AGENT,"Out of memory")); 
		rtncode = -1; 
		goto CLEANUP;
	}
	
	for (i = (SAHPI_MAX_ENTITY_PATH - 1); i >= 0; i--) {
                guint num_digits, work_instance_num;

		/* Find last element of structure; Current choice not good,
		   since type=instance=0 is valid */
		if (epathptr->Entry[i].EntityType == 0) {
			continue;
		}

		/* Validate and convert data */
                work_instance_num = epathptr->Entry[i].EntityInstance;
                for (num_digits = 1; (work_instance_num = work_instance_num/10) > 0; num_digits++);
		
		if (num_digits > MAX_INSTANCE_DIGITS) { 
                        DEBUGMSGTL((AGENT,"Instance value too big"));
                        rtncode = -1; 
			goto CLEANUP;
		}
                memset(instance_str, 0, MAX_INSTANCE_DIGITS + 1);
                err = snprintf(instance_str, MAX_INSTANCE_DIGITS + 1,
                               "%d", epathptr->Entry[i].EntityInstance);

                // check if we can convert the entity type to a string
                tidx = entitytype2index(epathptr->Entry[i].EntityType);

                if ( tidx >= 0 )
                        type_str = eshort_names[tidx];
                else {
                        err = snprintf(type_str_buffer, 20,
                                       "%d", epathptr->Entry[i].EntityType);
                        type_str = type_str_buffer;
                }

		strcount = strcount + 
			strlen(EPATHSTRING_START_DELIMITER) + 
			strlen(type_str) + 
			strlen(EPATHSTRING_VALUE_DELIMITER) + 
			strlen(instance_str) + 
			strlen(EPATHSTRING_END_DELIMITER);

		if (strcount > strsize - 1) {
			DEBUGMSGTL((AGENT,"Not enough space allocated for string"));
			rtncode = -1;
			goto CLEANUP;
		}

		catstr = g_strconcat(EPATHSTRING_START_DELIMITER,
				     type_str,
				     EPATHSTRING_VALUE_DELIMITER,
				     instance_str, 
				     EPATHSTRING_END_DELIMITER, 
				     NULL);

		strcat(tmpstr, catstr);
		g_free(catstr);
	}
        rtncode = strcount;
	/* Write string */
	memset(epathstr, 0 , strsize);
	strcpy(epathstr, tmpstr);

 CLEANUP:
	g_free(instance_str);
	g_free(tmpstr);

	return(rtncode);
} /* End entitypath2string */
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

#endif

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
	  snprintf(buf, 3, "hpiSubagent: hpiSubagent_parse_config_traps, '%s' unrecognized", cptr);
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

	  snprintf(buf, 3, "hpiSubagent: hpiSubagent_parse_config_interval, '%s' unrecognized", cptr);
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

	  snprintf(buf, 3, "hpiSubagent: hpiSubagent_parse_config_max_event, '%s' unrecognized", cptr);
      config_perror (buf);
    }
  else
    {
      snmp_log (LOG_INFO, "Max Event rows %d.\n", x);
      MAX_EVENT_ENTRIES = x;
    }
}

void
usage(char *applName)
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

  pid_t child;
  /* change this if you want to be a SNMP master agent */


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
	init_saHpiDomainInfoTable();
	init_saHpiDomainReferenceTable();
	init_saHpiDomainAlarmTable();
	init_saHpiResourceTable();
	init_saHpiAutoTimeOutTable();
	init_saHpiHotSwapTable();
	init_saHpiEventTable();
	init_saHpiResourceEventTable();
	init_saHpiDomainEventTable();
	init_saHpiSensorEventTable();
	init_saHpiSensorEnableChangeEventTable();
	init_saHpiHotSwapEventTable();
	init_saHpiWatchdogEventTable();
	init_saHpiSoftwareEventTable();
	init_saHpiOEMEventTable();
	init_saHpiUserEventTable();
	init_saHpiAnnouncementTable();			    
	
	init_saHpiEventLogInfoTable();
	init_saHpiEventLogTable();
	
	init_saHpiRdrTable();
	init_saHpiCtrlTable();
	init_saHpiSensorTable();
	
	init_saHpiSensorReadingCurrentTable();
	init_saHpiSensorReadingMaxTable();
	init_saHpiSensorReadingMinTable();
	
	init_saHpiSensorReadingNominalTable();
	init_saHpiSensorReadingNormalMaxTable();
	init_saHpiSensorReadingNormalMinTable();
	
	init_saHpiSensorThdLowCriticalTable();
	init_saHpiSensorThdLowMajorTable();
	init_saHpiSensorThdLowMinorTable();
	init_saHpiSensorThdNegHysteresisTable();
	init_saHpiSensorThdPosHysteresisTable();
	init_saHpiSensorThdUpCriticalTable();
	init_saHpiSensorThdUpMajorTable();
	init_saHpiSensorThdUpMinorTable();
	
	init_saHpiInventoryTable();
	init_saHpiAreaTable();
	init_saHpiFieldTable();
	init_saHpiWatchdogTable();
	init_saHpiAnnunciatorTable();

  if (send_traps_on_startup == AGENT_TRUE)
    send_traps = AGENT_TRUE;

  dbg("WARNING: populate_rpt: hpiSubagent.c: nolong implemented!");
#if 0 /* TODO DMJ */
  if (populate_rpt () != AGENT_ERR_NOERROR)
    {
      snmp_log (LOG_ERR, "Could not retrieve RPT entries. Exiting\n.");
      rc = -1;
      goto stop;
    }
#endif 

  dbg("WARNING: populate_event: hpiSubagent.c: nolong implemented!");
#if 0 /* TODO DMJ */
  //populate_event ();
#endif 

  if (init_alarm () != AGENT_ERR_NOERROR)
    {
      snmp_log (LOG_ERR, "Could not start our internal loop . Exiting\n.");
      rc = -1;
      goto stop;
    }

  send_traps = AGENT_TRUE;
  /* If we're going to be a snmp master agent, initial the ports */

  if (!agentx_subagent)
    init_master_agent ();	/* open the port to listen on (defaults to udp:161) */

  if (do_fork == AGENT_TRUE)
    {
      if ((child = fork ()) < 0)
	{
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
  while (keep_running)
    {
      /* if you use select(), see snmp_select_info() in snmp_api(3) */
      /*     --- OR ---  */
      rc = agent_check_and_process (1);	/* 0 == don't block */
    }
stop:

  dbg("WARNING: closeSaHpiSession: hpiSubagent.c: nolong implemented!");
  //closeSaHpiSession();
  /* at shutdown time */
  snmp_log (LOG_INFO, "Stopping %s\n", version);
  snmp_shutdown (AGENT);
  
  return rc;
}
