/*
* (C) Copyright IBM Corp. 2004
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
#ifndef _HPI_DOMAIN_
#define _HPI_DOMAIN_

#include <SaHpi.h>
#include <glib.h>

//*******************************************************
//*******************************************************
// saHpiDomainReferenceTable support fucntions
//*******************************************************
//*******************************************************
struct sa_domain_table {
	SaHpiDomainIdT 	did;
	SaHpiSessionIdT sid;
};
			

struct sa_resource_table {
        GHashTable *table;
        GStaticRecMutex lock;
};

int populate_drt(void);
SaHpiSessionIdT get_session_id(SaHpiDomainIdT did);



#endif /*_HPI_DOMAIN_ */
