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
 *   David Judkovics <djudkovi@us.ibm.com>
 *
 *
 */
#include <net-snmp/net-snmp-config.h>
#include <net-snmp/net-snmp-includes.h>

#include <net-snmp/agent/net-snmp-agent-includes.h>
#include <net-snmp/library/snmp_assert.h>
#include <net-snmp/library/check_varbind.h>

#include <SaHpi.h>
#include <oh_error.h>
#include <hpiDomain.h>


//*******************************************************
//*******************************************************
// saHpiDomainReferenceTable support fucntions
//*******************************************************
//*******************************************************
struct sa_domain_table top_drt = {
	.did = SAHPI_UNSPECIFIED_DOMAIN_ID,
	.sid = 0
};

struct sa_resource_table top_rpt = {        
        .table = NULL,
        .lock = G_STATIC_REC_MUTEX_INIT
};

static int populate_drt_call  = FALSE;


int populate_drt(void) {

	int rval = 0;
	SaHpiSessionIdT sid;

	SaHpiDomainInfoT DomainInfo;
	memset(&DomainInfo, 0, sizeof(SaHpiDomainInfoT));

	if ( SA_OK != saHpiSessionOpen(
		SAHPI_UNSPECIFIED_DOMAIN_ID, 
		&sid, NULL) ){
		dbg("ERROR: populate_drt, saHpiSessionOpen Failed!"); 
		rval = -1;
	} else {
		top_drt.did = SAHPI_UNSPECIFIED_DOMAIN_ID;
		top_drt.sid = sid;
		populate_drt_call  = TRUE;
	}
	
	return rval;

}

SaHpiSessionIdT get_session_id(SaHpiDomainIdT did) {
	if (populate_drt_call  == TRUE) 
		return top_drt.did;
	else
		return -1;
}



