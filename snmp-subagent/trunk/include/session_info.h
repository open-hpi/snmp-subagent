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
 *
 *					  
 */

#ifndef _SESSION_INFO_H_
#define _SESSION_INFO_H_

#include <net-snmp/net-snmp-config.h>
#include <net-snmp/net-snmp-includes.h>
#include <net-snmp/agent/net-snmp-agent-includes.h>
//#include <net-snmp/agent/table_array.h>
#include <signal.h>

#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>

#include <hpiSubagent.h>

#include <SaHpi.h> 
#include <oh_utils.h>

#include <alarm.h>

/**************************************************/
/*** BEGIN: ***************************************/
/*** Manages All existing sessions and domains  ***/
/**************************************************/
void store_session_info(SaHpiSessionIdT session_id, SaHpiDomainIdT domain_id);
SaHpiSessionIdT get_session_id(SaHpiDomainIdT domain_id);
SaHpiDomainIdT get_domain_id(SaHpiSessionIdT session_id);
int subcsribe_all_sessions(void);
/**************************************************/
/*** END: *****************************************/
/*** Manages All existing sessions and domains  ***/
/**************************************************/


/**************************************************/
/*** BEGIN: ***************************************/
/*** Creates full oid, used for such things as  ***/
/*** RowPointers and Notifications              ***/
/**************************************************/
/**************************************************/
int build_full_oid (oid * prefix, size_t prefix_len,
		    oid * column, size_t column_len,
		    netsnmp_index * index,
		    oid * out_oid, size_t in_len, size_t * out_len);

/**************************************************/
/*** BEGIN: ***************************************/
/*** Translates from HPI to SNMP Error Codes s  ***/
/**************************************************/
/**************************************************/
int get_snmp_error(SaErrorT val);

/**************************************************/
/*** BEGIN: ***************************************/
/*** Hash Table Used for generating and         ***/
/*** tracking unique indices when required      ***/
/**************************************************/
/**************************************************/
typedef struct {
	SaHpiEntryIdT domainId_resourceId_arry[2];
} SaHpiDomainIdResourceIdArrayT;


typedef struct {
        SaHpiEntryIdT entry_id;
        SaHpiDomainIdResourceIdArrayT dr_pair;
} DR_XREF;

/* hpi internal apis */
SaErrorT domain_resource_pair_initialize(int *initialized, GHashTable **oh_ep_table); 
DR_XREF *domain_resource_pair_get(SaHpiDomainIdResourceIdArrayT *ep, GHashTable **oh_ep_table); 
DR_XREF *domain_resoruce_pair_lookup(SaHpiDomainIdResourceIdArrayT *ep, GHashTable **oh_ep_table);

/**************************************************/
/*** BEGIN: ***************************************/
/*** Hash Table Used for generating and         ***/
/*** tracking unique indices when required      ***/
/**************************************************/
/**************************************************/
SaErrorT decode_sensor_range_flags(SaHpiTextBufferT *buffer, 
				   SaHpiSensorRangeFlagsT sensor_range_flags);

int check_sensor_reading_value(size_t val_len, long type);

int set_sensor_reading_value(SaHpiSensorReadingT *reading, 
			     unsigned char *saHpiCurrentSensorStateValue);

SaErrorT set_sen_thd_value(SaHpiSensorReadingUnionT *value, 
			   SaHpiSensorReadingTypeT type,
			   u_char *val, size_t val_len);


void oh_decode_char(SaHpiTextBufferT *buffer);


void oh_encode_char(SaHpiTextBufferT *buffer);

SaErrorT oh_decode_exp_flags(SaHpiWatchdogExpFlagsT exp_flags,
			     SaHpiTextBufferT *buffer);

int oh_encode_exp_flags(SaHpiTextBufferT *buffer, 
                             SaHpiWatchdogExpFlagsT *exp_flags);

void res_cap_map(SaHpiUint16T *rs_cap, SaHpiRptEntryT *rpt_entry);
void hotswap_cap_map(SaHpiUint8T *hs_cap, SaHpiRptEntryT *rpt_entry);

void assign_timestamp(SaHpiTimeT *evt_timestamp, struct counter64 *row_timestamp);

#endif //_SESSION_INFO_H_
