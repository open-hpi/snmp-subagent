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
 *					  
 */
#ifndef _HASH_UTILS_H_
#define _HASH_UTILS_H_

#include <hash_utils.h>
#include <SaHpi.h>


/**************************************************/
/*** BEGIN: ***************************************/
/*** Hash Table Used for generating and         ***/
/*** tracking unique indices when required      ***/
/**************************************************/
/**************************************************/
typedef struct {
	SaHpiEntryIdT domainId_resourceId_idr_arry[3];
} SaHpiDomainIdResourceIdInventoryIdArrayT;


typedef struct {
        SaHpiEntryIdT entry_id;
        SaHpiDomainIdResourceIdInventoryIdArrayT dri_pair;
} DRI_XREF;

/* hpi internal apis */
SaErrorT domain_resource_idr_initialize(int *initialized, GHashTable **oh_ep_table); 
DRI_XREF *domain_resource_idr_get(SaHpiDomainIdResourceIdInventoryIdArrayT *ep, GHashTable **oh_ep_table); 
DRI_XREF *domain_resoruce_idr_lookup(SaHpiDomainIdResourceIdInventoryIdArrayT *ep, GHashTable **oh_ep_table);

/**************************************************/
/*** BEGIN: ***************************************/
/*** Hash Table Used for generating and         ***/
/*** tracking unique indices when required      ***/
/**************************************************/

/**************************************************/
/*** BEGIN: ***************************************/
/*** Hash Table Used for generating and         ***/
/*** tracking unique indices when required      ***/
/*** Domain, Resource, Idr, Area                ***/
/**************************************************/
/**************************************************/
typedef struct {
	SaHpiEntryIdT domainId_resourceId_idr_area_arry[4];
} SaHpiDomainIdResourceIdInventoryIdAreaIdArrayT;


typedef struct {
        SaHpiEntryIdT entry_id;
        SaHpiDomainIdResourceIdInventoryIdAreaIdArrayT dria_tuple;
        SaHpiEntryIdT sa_hpi_area_id_index;
} DRIA_XREF;

/* hpi internal apis */
SaErrorT domain_resource_idr_area_initialize(int *initialized, GHashTable **oh_ep_table); 
DRIA_XREF *domain_resource_idr_area_get(SaHpiDomainIdResourceIdInventoryIdAreaIdArrayT *ep, 
                                        GHashTable **oh_ep_table); 
DRIA_XREF *domain_resoruce_idr_area_lookup(SaHpiDomainIdResourceIdInventoryIdAreaIdArrayT *ep, 
                                           GHashTable **oh_ep_table);
/**************************************************/
/*** BEGIN: ***************************************/
/*** Hash Table Used for generating and         ***/
/*** tracking unique indices when required      ***/
/**************************************************/

#endif /* _HASH_UTILS_H_ */
