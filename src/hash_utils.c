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
#include <stdio.h>
#include <stdlib.h>
#include <glib.h>
#include <fcntl.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>

#include <hash_utils.h>
#include <net-snmp/net-snmp-config.h>
#include <net-snmp/net-snmp-includes.h>
#include <net-snmp/agent/net-snmp-agent-includes.h>


/**************************************************/
/*** BEGIN: ***************************************/
/*** Hash Table Used for generating and         ***/
/*** tracking unique indices when required      ***/
/**************************************************/
/**************************************************/
#define HASH_FILE "hash_utils.c"
/* for hash table usage */
guint domain_resource_idr_hash(gconstpointer key);
gboolean domain_resource_idr_equal(gconstpointer a, gconstpointer b);


/**
 * 
 * @param key
 * 
 * @return 
 */
guint domain_resource_idr_hash(gconstpointer key)
{
	const char *p = key;
	guint h = *p;

	int i;

	int dr_pair_len;

	dr_pair_len = sizeof(SaHpiDomainIdResourceIdInventoryIdArrayT);

	p += 1;

	for ( i=0; i < dr_pair_len - 1; i++ ) {
		h = (h * 131) + *p;
		p++;                          
	}

	/* don't change the 1009, its magic */
	return( h % 1009 );

}

/**
 * 
 * @param a
 * @param b
 * 
 * @return 
 */
gboolean domain_resource_idr_equal(gconstpointer a, gconstpointer b)
{
	if (!memcmp(a, b, sizeof(SaHpiDomainIdResourceIdInventoryIdArrayT))) {
		return 1;
	} else {
		return 0;
	}
}

/**
 * 
 * @param initialized
 * @param oh_ep_table
 * 
 * @return 
 */
SaErrorT domain_resource_idr_initialize(int *initialized, GHashTable **oh_ep_table) 
{
	SaErrorT rval = SA_OK;

	if (!*initialized) {

		/* initialize hash tables */
		*oh_ep_table = g_hash_table_new(domain_resource_idr_hash, 
						domain_resource_idr_equal);

		*initialized = TRUE;

	} else {
		rval = SA_ERR_HPI_ERROR;
	}

	return(rval);

}

/**
 * 
 * @param dri
 * @param oh_ep_table
 * 
 * @return 
 */
DRI_XREF *domain_resource_idr_get(SaHpiDomainIdResourceIdInventoryIdArrayT *dri, GHashTable **oh_ep_table) 
{
	gpointer key;
	gpointer value;

	DRI_XREF *dri_xref;

	if (!dri) return 0;

	key = dri;

	/* check for presence of DRI_XREF and */
	/* previously assigned uid      */        
	dri_xref = (DRI_XREF *)g_hash_table_lookup (*oh_ep_table, key);        
	if (dri_xref) {
		return dri_xref;
	}

	/* allocate storage for DRI_XREF cross reference data structure*/
	dri_xref = (DRI_XREF *)g_malloc0(sizeof(DRI_XREF));
	if (!dri_xref) {
		DEBUGMSGTL ((HASH_FILE, "malloc failed"));
		return NULL;                
	}
	memcpy(&dri_xref->dri_pair, dri, sizeof(SaHpiDomainIdResourceIdInventoryIdArrayT));

	/* entity path based key */   
	key = (gpointer)&dri_xref->dri_pair; 
	value = (gpointer)dri_xref;
	g_hash_table_insert(*oh_ep_table, key, value);

	return dri_xref;
}               

/**
 * oh_uid_lookup
 * @ep: pointer to entity path used to identify resourceID/uid
 *
 * Fetches resourceID/uid based on entity path in @ep.
 *  
 * Returns: success returns resourceID/uid, failure is 0.
 **/
DRI_XREF *domain_resoruce_idr_lookup(SaHpiDomainIdResourceIdInventoryIdArrayT *dri, GHashTable **oh_ep_table)
{
	DRI_XREF *dri_xref;

	gpointer key;

	if (!dri) return 0;

	key = dri;

	/* check hash table for entry in oh_ep_table */
	dri_xref = (DRI_XREF *)g_hash_table_lookup (*oh_ep_table, key);
	if (!dri_xref) {
		DEBUGMSGTL ((HASH_FILE, "error looking up DRI_XREF to get uid"));
		return NULL;
	}

	return dri_xref;
}
/**************************************************/
/*** END: *****************************************/
/*** Hash Table Used for generating and         ***/
/*** tracking unique indices when required      ***/
/**************************************************/
/**************************************************/
