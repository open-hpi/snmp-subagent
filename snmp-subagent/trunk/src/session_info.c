/*
 * (C) Copyright IBM Corp. 2005
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  This
 * file and program are licensed under a BSD style license.  See
 * the Copying file included with the OpenHPI distribution for
 * full licensing terms.
 *
 * Authors:
 *   David Judkovics  <djudkovi@us.ibm.com>
 *   Daniel de Araujo <ddearauj@us.ibm.com>
 *					  
 */
 
#include <stdio.h>
#include <stdlib.h>
#include <glib.h>
#include <fcntl.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>


#include <net-snmp/net-snmp-config.h>
#include <net-snmp/net-snmp-includes.h>
#include <net-snmp/agent/net-snmp-agent-includes.h>
#include <signal.h>



#include <SaHpi.h> 
#include <oh_utils.h>

#include <alarm.h>

#include <hpiSubagent.h>     
#include <session_info.h>

static SaHpiSessionIdT stored_session_id;
static SaHpiDomainIdT stored_domain_id;

/**************************************************/
/*** BEGIN: ***************************************/
/*** Manages All existing sessions and domains  ***/
/**************************************************/
/**
 * 
 * @session_id:
 * @domain_id:
 */
void store_session_info(SaHpiSessionIdT session_id, SaHpiDomainIdT domain_id)
{
	stored_session_id = session_id;
	stored_domain_id = domain_id;
}

/**
 * 
 * @domain_id:
 * 
 * @return:
 */
SaHpiSessionIdT get_session_id(SaHpiDomainIdT domain_id)
{
	return( stored_session_id );
}

/**
 * 
 * @session_id:
 * 
 * @return: 
 */
SaHpiDomainIdT get_domain_id(SaHpiSessionIdT session_id)
{
	return( stored_domain_id );
}

int subcsribe_all_sessions(void)
{
        SaErrorT rc = SA_OK;

        rc = saHpiSubscribe(stored_session_id);

        if (rc == SA_OK) {
                DEBUGMSGTL ((AGENT, "subcsribe_all_sessions() SUCCEEDED!!!!!!!\n"));
                return SNMP_ERR_NOERROR; 
        } else if (rc != SA_OK) {
                snmp_log (LOG_ERR,
                          "subcsribe_all_sessions: Call to saHpiSubscribe() failed to set Mode rc: %s.\n",
                          oh_lookup_error(rc));
                DEBUGMSGTL ((AGENT,
                             "subcsribe_all_sessions: Call to saHpiSubscribe() failed to set Mode rc: %s.\n",
                             oh_lookup_error(rc)));
                return get_snmp_error(rc);
        }

        return SA_ERR_HPI_ERROR;
}
/**************************************************/
/*** END: *****************************************/
/*** Manages All existing sessions and domains  ***/
/**************************************************/



/*
 *  int build_full_oid()
 */
int build_full_oid (oid * prefix, size_t prefix_len,
		    oid * column, size_t column_len,
		    netsnmp_index * index,
		    oid * out_oid, size_t in_len, size_t * out_len)
{

	int register len = prefix_len + column_len;
	int register i = 0;

	DEBUGMSGTL((AGENT, "build_full_oid() called\n"));

	if (index)
		len += index->len;

	if (len > in_len)
		return AGENT_ERR_MEMORY_FAULT;

	*out_len = len;
	memcpy (out_oid, prefix, prefix_len * sizeof (oid));

	for (; i < column_len; i++) {
		out_oid[prefix_len + i] = column[i];
	}
	len = prefix_len + i;

	if (index) {
		for (i = 0; i < index->len; i++) {
			out_oid[len + i] = index->oids[i];
		}
	}
	return AGENT_ERR_NOERROR;
}

/**************************************************/
/*** BEGIN: ***************************************/
/*** Translates from HPI to SNMP Error Codes s  ***/
/**************************************************/
/**************************************************/
/*
 * int get_snmp_error(SaErrorT val) 
 */
int get_snmp_error(SaErrorT val) 
{
	switch (val) {
	case SA_ERR_HPI_ERROR:
		return SNMP_ERR_GENERR;
		break;
	case SA_ERR_HPI_UNSUPPORTED_API:
		return SNMP_ERR_GENERR;
		break;
	case SA_ERR_HPI_BUSY:
		return SNMP_ERR_GENERR;
		break;
	case SA_ERR_HPI_INTERNAL_ERROR:
		return SNMP_ERR_GENERR;
		break;
	case SA_ERR_HPI_INVALID_CMD:
		return SNMP_ERR_GENERR;
		break;
	case SA_ERR_HPI_TIMEOUT:
		return SNMP_ERR_GENERR;
		break;
	case SA_ERR_HPI_OUT_OF_SPACE:
		return SNMP_ERR_GENERR;
		break;
	case SA_ERR_HPI_OUT_OF_MEMORY:
		return SNMP_ERR_GENERR;
		break;
	case SA_ERR_HPI_INVALID_PARAMS:
		return SNMP_ERR_GENERR;
		break;
	case SA_ERR_HPI_INVALID_DATA:
		return SNMP_ERR_WRONGVALUE;
		break;
	case SA_ERR_HPI_NOT_PRESENT:
		return SNMP_ERR_INCONSISTENTNAME;
		break;
	case SA_ERR_HPI_NO_RESPONSE:
		return SNMP_ERR_GENERR;
		break;
	case SA_ERR_HPI_DUPLICATE:
		return SNMP_ERR_GENERR;
		break;
	case SA_ERR_HPI_INVALID_SESSION:
		return SNMP_ERR_GENERR;
		break;
	case SA_ERR_HPI_INVALID_DOMAIN:
		return SNMP_ERR_INCONSISTENTNAME;
		break;
	case SA_ERR_HPI_INVALID_RESOURCE:
		return SNMP_ERR_INCONSISTENTNAME;
		break;
	case SA_ERR_HPI_INVALID_REQUEST:
		return SNMP_ERR_GENERR;
		break;
	case SA_ERR_HPI_ENTITY_NOT_PRESENT:
		return SNMP_ERR_INCONSISTENTNAME;
		break;
	case SA_ERR_HPI_READ_ONLY:
		return SNMP_ERR_NOTWRITABLE;
		break;
	case SA_ERR_HPI_CAPABILITY:
		return SNMP_ERR_GENERR;
		break;
	case SA_ERR_HPI_UNKNOWN:
		return SNMP_ERR_GENERR;
		break;
	default:
		return SNMP_ERR_GENERR;
		break;
	}
}


/**************************************************/
/*** BEGIN: ***************************************/
/*** Hash Table Used for generating and         ***/
/*** tracking unique indices when required      ***/
/**************************************************/
/**************************************************/
#define HASH_FILE "session_info.c"
/* for hash table usage */
guint domain_resource_pair_hash(gconstpointer key);
gboolean domain_resource_pair_equal(gconstpointer a, gconstpointer b);


/*
 * oh_entity_path_hash: used by g_hash_table_new() 
 * in oh_uid_initialize(). See glib library for
 * further details.
 */
guint domain_resource_pair_hash(gconstpointer key)
{
	const char *p = key;
	guint h = *p;

	int i;

	int dr_pair_len;

	dr_pair_len = sizeof(SaHpiDomainIdResourceIdArrayT);

	p += 1;

	for ( i=0; i < dr_pair_len - 1; i++ ) {
		h = (h * 131) + *p;
		p++;                          
	}

	/* don't change the 1009, its magic */
	return( h % 1009 );

}

/*
 * oh_entity_path_equal: used by g_hash_table_new() 
 * in oh_uid_initialize(). See glib library for
 * further details.
 */
gboolean domain_resource_pair_equal(gconstpointer a, gconstpointer b)
{
	if (!memcmp(a, b, sizeof(SaHpiDomainIdResourceIdArrayT))) {
		return 1;
	} else {
		return 0;
	}
}

/**
 * oh_uid_initialize
 *
 * UID utils initialization routine
 * This functions must be called before any other uid_utils
 * are made. 
 * 
 * Returns: success 0, failure -1.
 **/
SaErrorT domain_resource_pair_initialize(int *initialized, GHashTable **oh_ep_table) 
{
	SaErrorT rval = SA_OK;

	if (!*initialized) {

		/* initialize hash tables */
		*oh_ep_table = g_hash_table_new(domain_resource_pair_hash, 
						domain_resource_pair_equal);

		*initialized = TRUE;

	} else {
		rval = SA_ERR_HPI_ERROR;
	}

	return(rval);

}

/**
 * oh_uid_from_entity_path
 * @ep: value to be removed from used
 *
 * This function returns an unique value to be used as
 * an uid/resourceID base upon a unique entity path specified
 * by @ep.  If the entity path already exists, the already assigned 
 * resource id is returned.   
 * 
 * Returns: 
 **/
DR_XREF *domain_resource_pair_get(SaHpiDomainIdResourceIdArrayT *dr, GHashTable **oh_ep_table) 
{
	gpointer key;
	gpointer value;

	DR_XREF *dr_xref;

	if (!dr) return 0;

	key = dr;

	/* check for presence of EP and */
	/* previously assigned uid      */        
	dr_xref = (DR_XREF *)g_hash_table_lookup (*oh_ep_table, key);        
	if (dr_xref) {
		return dr_xref;
	}

	/* allocate storage for EP cross reference data structure*/
	dr_xref = (DR_XREF *)g_malloc0(sizeof(DR_XREF));
	if (!dr_xref) {
		DEBUGMSGTL ((HASH_FILE, "malloc failed"));
		return NULL;                
	}
	memcpy(&dr_xref->dr_pair, dr, sizeof(SaHpiDomainIdResourceIdArrayT));

	/* entity path based key */   
	key = (gpointer)&dr_xref->dr_pair; 
	value = (gpointer)dr_xref;
	g_hash_table_insert(*oh_ep_table, key, value);

	return dr_xref;
}               

/**
 * oh_uid_lookup
 * @ep: pointer to entity path used to identify resourceID/uid
 *
 * Fetches resourceID/uid based on entity path in @ep.
 *  
 * Returns: success returns resourceID/uid, failure is 0.
 **/
DR_XREF *domain_resoruce_pair_lookup(SaHpiDomainIdResourceIdArrayT *dr, GHashTable **oh_ep_table)
{
	DR_XREF *dr_xref;

	gpointer key;

	if (!dr) return 0;

	key = dr;

	/* check hash table for entry in oh_ep_table */
	dr_xref = (DR_XREF *)g_hash_table_lookup (*oh_ep_table, key);
	if (!dr_xref) {
		DEBUGMSGTL ((HASH_FILE, "error looking up EP to get uid"));
		return NULL;
	}

	return dr_xref;
}
/**************************************************/
/*** END: *****************************************/
/*** Hash Table Used for generating and         ***/
/*** tracking unique indices when required      ***/
/**************************************************/
/**************************************************/

/**************************************************/
/*** BEGIN: ***************************************/
/*** decode_sensor_range_flags		        ***/
/**************************************************/
/**************************************************/
typedef struct  {
	SaHpiSensorRangeFlagsT 	flag;
	const unsigned char* 	str;
} sensor_range_flags;

static sensor_range_flags range_flags[] = {
	{SAHPI_SRF_MIN,(unsigned char *)"MIN, "},
	{SAHPI_SRF_MAX,(unsigned char *) "MAX, "},
	{SAHPI_SRF_NORMAL_MIN, (unsigned char *)"NORMAL_MIN, "},
	{SAHPI_SRF_NORMAL_MAX, (unsigned char *)"NORMAL_MAX, "},
	{SAHPI_SRF_NOMINAL,(unsigned char *)"NOMINAL, "}};
#define RANGE_FLAGS_LEN 5

/**
 * 
 * @buffer:
 * @sensor_range_flags:
 * 
 * Returns: 
 */
SaErrorT decode_sensor_range_flags(SaHpiTextBufferT *buffer, 
				   SaHpiSensorRangeFlagsT sensor_range_flags)
{

	int i = 0;
	int len = 0;

	memset(buffer, 0, sizeof(*buffer));

	for (i = 0; i < RANGE_FLAGS_LEN; i++) {

		if ((sensor_range_flags & range_flags[i].flag) == range_flags[i].flag) {

			len = strlen((const char *)range_flags[i].str);

			if (len + buffer->DataLength  > SAHPI_MAX_TEXT_BUFFER_LENGTH)
				return SA_ERR_HPI_OUT_OF_SPACE;

			memcpy(buffer->Data + buffer->DataLength,
			       range_flags[i].str,
			       len);

			buffer->DataLength += len;
		}

	}

	return SA_OK;
}



/**
 * 
 * @netsnmp_variable_list:
 * @saHpiSensorThdLowCriticalTable_context:
 * 
 * Checks length of var.string val is correct for data type supported. 
 * 
 * Returns: 
 */
int check_sensor_reading_value(size_t val_len, long type)
{
	switch (type - 1) {                        
	case SAHPI_SENSOR_READING_TYPE_INT64:
		if(val_len <= sizeof(SaHpiInt64T))
			return SNMP_ERR_NOERROR;
		break;
	case SAHPI_SENSOR_READING_TYPE_UINT64:
		if(val_len <= sizeof(SaHpiUint64T))
			return SNMP_ERR_NOERROR;
		break;
	case SAHPI_SENSOR_READING_TYPE_FLOAT64:
		if(val_len <= sizeof(SaHpiFloat64T))
			return SNMP_ERR_NOERROR;
		break;
	case SAHPI_SENSOR_READING_TYPE_BUFFER:
		if(val_len <= (SAHPI_SENSOR_BUFFER_LENGTH * sizeof(SaHpiUint8T)))
			return SNMP_ERR_NOERROR;
		break;
	default:
		return SNMP_ERR_WRONGLENGTH;
	}

	return SNMP_ERR_WRONGLENGTH;
}

/**
 * 
 * @reading:
 * @saHpiCurrentSensorStateValue:
 * 
 * Copies reading into text buffer and returns number of 
 * bytes copied.
 * 
 * Returns: 
 */
int set_sensor_reading_value(SaHpiSensorReadingT *reading, 
			     unsigned char *saHpiCurrentSensorStateValue)
{
	
	
	
	switch (reading->Type) {
	case SAHPI_SENSOR_READING_TYPE_INT64:
		
		sprintf((char *)saHpiCurrentSensorStateValue, "%lld", reading->Value.SensorInt64);

		return sizeof(SaHpiInt64T);
		break;
	case SAHPI_SENSOR_READING_TYPE_UINT64:
		
		sprintf((char *)saHpiCurrentSensorStateValue, "%llu", reading->Value.SensorUint64);

		return sizeof(SaHpiUint64T);
		break;
	case SAHPI_SENSOR_READING_TYPE_FLOAT64:

		sprintf((char *)saHpiCurrentSensorStateValue, "%g", reading->Value.SensorFloat64);
		
		return sizeof(SaHpiFloat64T);
		break;
	case SAHPI_SENSOR_READING_TYPE_BUFFER:
		memcpy(saHpiCurrentSensorStateValue, 
		       reading->Value.SensorBuffer, 
		       SAHPI_SENSOR_BUFFER_LENGTH * sizeof(SaHpiUint8T));
		       
		       
		       
		return SAHPI_SENSOR_BUFFER_LENGTH * sizeof(SaHpiUint8T);
		break;
	default:
		return 0;
	}

	return 0;
}

/**
 * 
 * @value:
 * @type:
 * @val:
 * @val_len:
 * 
 * @returns: 
 */
SaErrorT set_sen_thd_value(SaHpiSensorReadingUnionT *value, 
			       SaHpiSensorReadingTypeT type,
			       u_char *val, size_t val_len)
{
	switch (type) {
	case SAHPI_SENSOR_READING_TYPE_INT64:
		if (val_len > sizeof(SaHpiInt64T)) 
			return SA_ERR_HPI_INVALID_DATA;
		
		sprintf((char *)val, "%lld", value->SensorInt64);
		sprintf((char *)val, "%s",   val);				
		break;
			
	case SAHPI_SENSOR_READING_TYPE_UINT64:
		if (val_len > sizeof(SaHpiUint64T)) 
			return SA_ERR_HPI_INVALID_DATA;
							
		sprintf((char *)val, "%llu", value->SensorUint64);
		sprintf((char *)val, "%s",   val);	
		break;
		
	case SAHPI_SENSOR_READING_TYPE_FLOAT64:
		if (val_len > sizeof(SaHpiFloat64T)) 
			return SA_ERR_HPI_INVALID_DATA;
			
		sprintf((char *)val, "%g",  value->SensorFloat64);
		sprintf((char *)val, "%s",  val);					
		break;	
		
	case SAHPI_SENSOR_READING_TYPE_BUFFER:
		if (val_len > SAHPI_SENSOR_BUFFER_LENGTH * sizeof(SaHpiUint8T)) 
			return SA_ERR_HPI_INVALID_DATA;
		memcpy(&value->SensorBuffer, val, val_len);
		break;
	default:
		return SA_ERR_HPI_UNKNOWN;
	}

	return SA_OK;

}



/**
 * 
 * @buffer
 * 
 * Changes '|' to a ','
 */
void oh_decode_char(SaHpiTextBufferT *buffer)
{
	int i = 0;

	for (i = 0; i < buffer->DataLength; i++) {
		if ( buffer->Data[i] == '|' )
			buffer->Data[i] = ',';
	}
}

/**
 * 
 * @buffer
 * 
 * Changes ',' to a '|'
 */
void oh_encode_char(SaHpiTextBufferT *buffer)
{
	int i = 0;

	for (i = 0; i < buffer->DataLength; i++) {
		if ( buffer->Data[i] == ',' )
			buffer->Data[i] = '|';
	}
}

typedef struct {
    SaHpiWatchdogExpFlagsT exp_flag;
    unsigned char *str;
} watchdog_exp_flags;

static watchdog_exp_flags watchdog_string[] = {
  {SAHPI_WATCHDOG_EXP_BIOS_FRB2, (unsigned char *)"BIOS_FRB2"},
  {SAHPI_WATCHDOG_EXP_BIOS_POST, (unsigned char *)"BIOS_POST"},
  {SAHPI_WATCHDOG_EXP_OS_LOAD,   (unsigned char *)"OS_LOAD"},
  {SAHPI_WATCHDOG_EXP_SMS_OS,    (unsigned char *)"SMS_OS"},
  {SAHPI_WATCHDOG_EXP_OEM,       (unsigned char *)"Oem"}
};

#define MAX_EXP_FLAGS_STRINGS 5

SaErrorT oh_decode_exp_flags(SaHpiWatchdogExpFlagsT exp_flags,
			     SaHpiTextBufferT *buffer)
{
	int i, found;
	SaErrorT err;
	SaHpiTextBufferT working;

	err = oh_init_textbuffer(&working);
	if (err != SA_OK) 
                return(err);

	found = 0;
	/* Look for category's event states */
	for (i=0; i < MAX_EXP_FLAGS_STRINGS; i++) {
                if ((watchdog_string[i].exp_flag & exp_flags) == watchdog_string[i].exp_flag) {
				found++;
				err = oh_append_textbuffer(&working, (char *)watchdog_string[i].str);
				if (err != SA_OK) 
                                        return(err);
				err = oh_append_textbuffer(&working, OH_ENCODE_DELIMITER);
				if (err != SA_OK) 
                                        return(err);
                }
	}

	/* Remove last delimiter */
	if (found) {
		for (i=0; i < OH_ENCODE_DELIMITER_LENGTH + 1; i++) {
			working.Data[working.DataLength - i] = 0x00;
		}
		working.DataLength = working.DataLength - OH_ENCODE_DELIMITER_LENGTH;
	}

	err = oh_copy_textbuffer(buffer, &working);

        oh_decode_char(buffer);

	return(SA_OK);
}

int oh_encode_exp_flags(SaHpiTextBufferT *buffer, SaHpiWatchdogExpFlagsT *exp_flags)
{
        int i,j;

        int rv = SNMP_ERR_BADVALUE;

        gchar *gstr = NULL;
	gchar **eventdefs = NULL;

	if (!buffer || !exp_flags || 
            buffer->Data == NULL || buffer->Data[0] == '\0') {
                DEBUGMSGTL ((AGENT, "oh_encode_exp_flags: SA_ERR_HPI_INVALID_PARAMS"));
		return SA_ERR_HPI_INVALID_PARAMS;
	}

        oh_encode_char(buffer);

	/* Split out event definitions */
	if (buffer->DataLength < SAHPI_MAX_TEXT_BUFFER_LENGTH) {
		buffer->Data[buffer->DataLength] = '\0';
	}
	gstr = g_strstrip(g_strndup((gchar *)buffer->Data, SAHPI_MAX_TEXT_BUFFER_LENGTH));
	if (gstr == NULL || gstr[0] == '\0') {
                DEBUGMSGTL ((AGENT, "g_strstrip failed"));
		return SA_ERR_HPI_INTERNAL_ERROR;
	}

	eventdefs = g_strsplit(gstr, OH_ENCODE_DELIMITER_CHAR, -1);
	if (eventdefs == NULL) {
                DEBUGMSGTL ((AGENT, "No event definitions"));
		return SA_ERR_HPI_INVALID_PARAMS;
	}

	for (i=0; eventdefs[i] != NULL && eventdefs[i][0] != '\0'; i++) {
		eventdefs[i] = g_strstrip(eventdefs[i]);

		for (j=0; j < MAX_EXP_FLAGS_STRINGS; j++) {
			if (strcasecmp(eventdefs[i], (char *)watchdog_string[j].str) == 0) {
                                *exp_flags += watchdog_string[j].exp_flag;
                                rv = SNMP_ERR_NOERROR;
			}
		}
	}
	
        g_free(gstr);
        g_strfreev(eventdefs);

        return rv;
}

void res_cap_map(SaHpiUint16T *rs_cap, SaHpiRptEntryT *rpt_entry)
{
        *rs_cap = 0x0000;                

	if (rpt_entry->ResourceCapabilities & SAHPI_CAPABILITY_SENSOR)
		*rs_cap = 0x4000;
	if (rpt_entry->ResourceCapabilities & SAHPI_CAPABILITY_RDR)
		*rs_cap |= 0x2000;
	if (rpt_entry->ResourceCapabilities & SAHPI_CAPABILITY_EVENT_LOG)
		*rs_cap |= 0x1000;
	if (rpt_entry->ResourceCapabilities & SAHPI_CAPABILITY_INVENTORY_DATA)
		*rs_cap |= 0x0800;

	if (rpt_entry->ResourceCapabilities & SAHPI_CAPABILITY_RESET)
		*rs_cap |= 0x0400;
	if (rpt_entry->ResourceCapabilities & SAHPI_CAPABILITY_POWER)
		*rs_cap |= 0x0200;
	if (rpt_entry->ResourceCapabilities & SAHPI_CAPABILITY_ANNUNCIATOR)
		*rs_cap |= 0x0100;
	if (rpt_entry->ResourceCapabilities & SAHPI_CAPABILITY_FRU)
		*rs_cap |= 0x0080;

	if (rpt_entry->ResourceCapabilities & SAHPI_CAPABILITY_CONTROL)
		*rs_cap |= 0x0040;
	if (rpt_entry->ResourceCapabilities & SAHPI_CAPABILITY_WATCHDOG)
		*rs_cap |= 0x0020;
	if (rpt_entry->ResourceCapabilities & SAHPI_CAPABILITY_MANAGED_HOTSWAP)
		*rs_cap |= 0x0010;
	if (rpt_entry->ResourceCapabilities & SAHPI_CAPABILITY_CONFIGURATION)
		*rs_cap |= 0x0008;

	if (rpt_entry->ResourceCapabilities & SAHPI_CAPABILITY_AGGREGATE_STATUS)
		*rs_cap |= 0x0004;
	if (rpt_entry->ResourceCapabilities & SAHPI_CAPABILITY_EVT_DEASSERTS)
		*rs_cap |= 0x0002;
	if (rpt_entry->ResourceCapabilities & SAHPI_CAPABILITY_RESOURCE)
		*rs_cap |= 0x0001;
}

void hotswap_cap_map(SaHpiUint8T *hs_cap, SaHpiRptEntryT *rpt_entry)
{
        *hs_cap = 0x00;          
	if (rpt_entry->HotSwapCapabilities & 
            SAHPI_HS_CAPABILITY_AUTOEXTRACT_READ_ONLY)
		*hs_cap = 0x40;
        if (rpt_entry->HotSwapCapabilities & 
	    SAHPI_HS_CAPABILITY_INDICATOR_SUPPORTED)
		*hs_cap |= 0x20;
}


/**
 * 
 * @evt_timestamp - pointer to the timestamp HPI uses and assigns.
 * @row_timestamp - pointer to the timestamp column in the row.
 * 
 * Based on endiness, assigns the timestamp referenced by evt_timestamp to
 * row_timestamp.
 */
void assign_timestamp(SaHpiTimeT *evt_timestamp, unsigned char *row_timestamp)
{
      
	int counter;

	if (!evt_timestamp) {
		return;
	}         

	for (counter = 0; counter != sizeof(SaHpiTimeT); ++counter) {
              
	      
	        if( counter < sizeof(unsigned long int)) { 
	        	size_t cnt = CHAR_BIT * (sizeof (SaHpiTimeT) - counter - 1);

                        /*********************************************************
			 * Take the BYTE_MASK (0xFF) and start ANDING the MSB 
			 * through the LSB (hence the shifting by cnt).  Then 
			 * we shift back so that we have a single char
			 * e.g., 0x012345678 where counter == 2
			 *      0) 0xFF << 8 = 0x0000FF00
			 *      1) 0x012345678 & 0x0000FF00 = 0x00005600
			 *      2) 0x00005600 >> 8 = 0x56.
			 ********************************************************/
                	row_timestamp[counter] = (unsigned char)
	                            (((unsigned long int)(*evt_timestamp) & (BYTE_MASK <<cnt))>>cnt);

                }
		
		/***************************************************************
	 	* Since net-snmp defines var->val.integer as a "long" type and 
		* HPI defines the timeout value as a "long long" type, we must
		* grab the next 4 bytes of the actual timeout value.
		**************************************************************/
		else {
	        	size_t cnt = CHAR_BIT * (sizeof (SaHpiTimeT) - counter - 1);

                	row_timestamp[counter] = (unsigned char)
	                            (((unsigned long int)
				     *(unsigned long int *)((long int)evt_timestamp + sizeof(long int)) & (BYTE_MASK <<cnt))>>cnt);

		}		    		
      
	}
	
}


/**
 * 
 * @evt_timestamp - pointer to the timestamp HPI uses and assigns.
 * @row_timestamp - pointer to the timestamp column in the row.
 * 
 * Based on endiness, compares the timestamp referenced by evt_timestamp to
 * row_timestamp. 0 for equal, 1 for not equal.
 */
int compare_timestamp(SaHpiTimeT evt_timestamp, unsigned char row_timestamp[])
{        
	unsigned char bigEndianTime[sizeof(SaHpiTimeT)];
	
	hpitime_to_snmptime(evt_timestamp, bigEndianTime);
	
	return (memcmp(bigEndianTime, row_timestamp, sizeof(evt_timestamp)));
	 
}

/**
 * 
 * @var - pointer to netsnmp variables - in.
 * @timeout - resulting time/timeout value
 * 
 * Regardless of endianess, the function takes the value in var and assigns it to the 
 * timeout array.
 */
void assign_timeout(netsnmp_variable_list * var, unsigned char timeout[])
{
	int counter;

	for (counter = 0; counter != sizeof(SaHpiTimeT); ++counter) {
              
	      
	        if( counter < sizeof(unsigned long int)) { 
	        	size_t cnt = CHAR_BIT * (sizeof (var->val.integer) - counter - 1);

                        /*********************************************************
			 * Take the BYTE_MASK (0xFF) and start ANDING the MSB 
			 * through the LSB (hence the shifting by cnt).  Then 
			 * we shift back so that we have a single char
			 * e.g., 0x012345678 where counter == 2
			 *      0) 0xFF << 8 = 0x0000FF00
			 *      1) 0x012345678 & 0x0000FF00 = 0x00005600
			 *      2) 0x00005600 >> 8 = 0x56.
			 ********************************************************/
                	timeout[counter] = (unsigned char)
	                            (((unsigned long int)(*var->val.integer) & (BYTE_MASK <<cnt))>>cnt);

                }
		
		/***************************************************************
	 	* Since net-snmp defines var->val.integer as a "long" type and 
		* HPI defines the timeout value as a "long long" type, we must
		* grab the next 4 bytes of the actual timeout value.
		**************************************************************/
		else {
	        	size_t cnt = CHAR_BIT * (sizeof (var->val.integer) - counter - 1);

                	timeout[counter] = (unsigned char)
	                            (((unsigned long int)
				     *(unsigned long int *)((long int)var->val.integer + sizeof(long int)) & (BYTE_MASK <<cnt))>>cnt);

		}		    		
      
	}
	
}	

/**
 * 
 * @timeIn - timeout held the snmp table (which is in network-byte order) - in.
 * 
 * @return - returns the timeout from the snmp table based on endianess of the machine.
 *           If the machine was little-endian and the data was 0x1122334455667788 (big endian)
 *           in the table, the actual timeout value would be represented (if no conversion 
 *           was made) as 0x8877665544332211 because little endian machines read the 
 *           MSB in the higer-order memory location first.  As a result, we actually 
 *           need to assign 0x1122334455667788 so the data represented in memory is 
 *           actually 0x8877665544332211.
 */
SaHpiInt64T snmptime_to_hpitime(unsigned char timeIn[])
{

	SaHpiInt64T time = 0;	
		
	int count, factor = 0;	

        if (__BYTE_ORDER == __LITTLE_ENDIAN) {		
		for (count = sizeof(SaHpiInt64T) - 1; count >= 0; count--, factor++)
		{
	
		    time |=  (long long int)timeIn[count] << (factor * CHAR_BIT); 	
	
		}
	
	}
	else {
		memcpy(&time, timeIn, sizeof(SaHpiInt64T));
	}
		
        return time;

	   
}

/**
 * 
 * @time    - 
 * @outTime - 
 * 
 * @return - 
 */
void hpitime_to_snmptime(SaHpiInt64T time, unsigned char outTime[])
{
	
	int count, factor = 0;
	
	if (__BYTE_ORDER == __LITTLE_ENDIAN) {	
	
		for(count = sizeof(SaHpiInt64T) - 1; count >= 0; count--, factor++)
		{
				
	              outTime[count] = (unsigned char)((time & ((long long int)BYTE_MASK << (factor * CHAR_BIT))) >> (factor * CHAR_BIT));
		}      
	}
	else {
	
		memcpy(outTime, &time, sizeof(SaHpiInt64T));
	}
		
}		
	
	


