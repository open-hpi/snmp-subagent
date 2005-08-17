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
 * $Id$
 *					  
 */

#ifndef _HPI_DOMAINALARM_MAPPING_
#define _HPI_DOMAINALARM_MAPPING_

#define MAPPING_FILE "hpiDomainAlarmMapping"

typedef struct {
	SaHpiEntryIdT key_array[2];
} doma_keys; //Domain-Alarm Keys


typedef struct {
        SaHpiEntryIdT   hpi_entry_id;
        doma_keys       de_pair;
} DE_XREF; //Domain-Entry Cross Reference.

/* hpi internal apis */
SaErrorT domain_alarm_entry_initialize(int *initialized, GHashTable **hash_table); 
DE_XREF *domain_alarm_entry_get(doma_keys *ep, GHashTable **hash_table); 
DE_XREF *domain_alarm_entry_lookup(doma_keys *ep, GHashTable **hash_table);

#endif /* _HPI_DOMAINALARM_MAPPING_ */
