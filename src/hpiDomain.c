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
#include <SaHpi.h>
#include <oh_error.h>
#include <hpiDomain.h>
			 
struct sa_domain_table top_drt = {
	.did = SAHPI_UNSPECIFIED_DOMAIN_ID,
	.sid = 0
};

struct sa_resource_table top_rpt = {        
        .table = NULL,
        .lock = G_STATIC_REC_MUTEX_INIT
};


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
	}

	/* get the info of the one and only domain */
	if (SA_OK != saHpiDomainInfoGet(top_drt.sid, &DomainInfo) ) {
		dbg("ERROR: populate_drt, saHpiDomainInfoGet Failed!"); 
		rval = -1;
	} else {
add data to rows here!!!!!!!!!
	}

	return rval;

}

SaHpiSessionIdT get_session_id(SaHpiDomainIdT did) {
	return top_drt.did;
}




