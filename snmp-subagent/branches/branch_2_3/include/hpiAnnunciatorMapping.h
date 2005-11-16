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
 * $Id$
 *					  
 */

#ifndef _HPI_ANNUNCIATOR_MAPPING_
#define _HPI_ANNUNCIATOR_MAPPING_

#define MAPPING_FILE "hpiAnnunciatorMapping"
typedef struct {
	SaHpiEntryIdT key_tuple_array[3];
} key_tuple;


typedef struct {
        SaHpiEntryIdT   hpi_entry_id;
        key_tuple       dre_pair;
} DRE_XREF;

/* hpi internal apis */
SaErrorT domain_resource_entry_initialize(int *initialized, GHashTable **hash_table); 
DRE_XREF *domain_resource_entry_get(key_tuple *ep, GHashTable **hash_table); 
DRE_XREF *domain_resoruce_entry_lookup(key_tuple *ep, GHashTable **hash_table);

#endif /* _HPI_ANNUNCIATOR_MAPPING_ */
