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

#include <stdio.h>
#include <stdlib.h>
#include <glib.h>
#include <fcntl.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <signal.h>
#include <SaHpi.h> 

#include <hpiAnnunciatorMapping.h>

#include <net-snmp/net-snmp-config.h>
#include <net-snmp/net-snmp-includes.h>
#include <net-snmp/agent/net-snmp-agent-includes.h>

#include <net-snmp/library/snmp_assert.h>


//static int initialized = FALSE;		      
//static GHashTable *hash_table;

guint domain_resource_entry_hash(gconstpointer key);
gboolean domain_resource_entry_equal(gconstpointer a, gconstpointer b);


/**
 * 
 * @key
 * 
 * @return 
 */
guint domain_resource_entry_hash(gconstpointer key)
{
	const char *p = key;
	guint h = *p;

	int i;

	int dre_len;

	dre_len = sizeof(key_tuple);

	p += 1;

	for ( i=0; i < dre_len - 1; i++ ) {
		h = (h * 131) + *p;
		p++;                          
	}

	/* don't change the 1009, its magic */
	return( h % 1009 );

}

/**
 * 
 * @a
 * @b
 * 
 * @return 
 */
gboolean domain_resource_entry_equal(gconstpointer a, gconstpointer b)
{
	if (!memcmp(a, b, sizeof(key_tuple))) {
		return 1;
	} else {
		return 0;
	}
}

/**
 * 
 * @initialized
 * @hash_table
 * 
 * @return 
 */
SaErrorT domain_resource_entry_initialize(int *initialized, GHashTable **hash_table) 
{
	SaErrorT rval = SA_OK;

	if (!*initialized) {

		/* initialize hash tables */
		*hash_table = g_hash_table_new(domain_resource_entry_hash, 
                                               domain_resource_entry_equal);

		*initialized = TRUE;

	} else {
		rval = SA_ERR_HPI_ERROR;
	}

	return(rval);

}

/**
 * 
 * @dre
 * @hash_table
 * 
 * @return 
 */
DRE_XREF *domain_resource_entry_get(key_tuple *dre, GHashTable **hash_table) 
{
	gpointer key;
	gpointer value;

	DRE_XREF *dre_xref;

	if (!dre) return 0;

	key = dre;

	/* check for presence of DRE_XREF and */
	/* previously assigned uid      */        
	dre_xref = (DRE_XREF *)g_hash_table_lookup (*hash_table, key);        
	if (dre_xref) {
		return dre_xref;
	}

	/* allocate storage for DRE_XREF cross reference data structure*/
	dre_xref = (DRE_XREF *)g_malloc0(sizeof(DRE_XREF));
	if (!dre_xref) {
		DEBUGMSGTL ((MAPPING_FILE, "malloc failed"));
		return NULL;                
	}
	memcpy(&dre_xref->dre_pair, dre, sizeof(key_tuple));

	/* entity path based key */   
	key = (gpointer)&dre_xref->dre_pair; 
	value = (gpointer)dre_xref;
	g_hash_table_insert(*hash_table, key, value);

	return dre_xref;
}               

/**
 * 
 * @param dre
 * @param hash_table
 * 
 * @return 
 */
DRE_XREF *domain_resoruce_entry_lookup(key_tuple *dre, GHashTable **hash_table)
{
	DRE_XREF *dre_xref;

	gpointer key;

	if (!dre) return 0;

	key = dre;

	/* check hash table for entry in hash_table */
	dre_xref = (DRE_XREF *)g_hash_table_lookup (*hash_table, key);
	if (!dre_xref) {
		DEBUGMSGTL ((MAPPING_FILE, "DRE_XREF index not found!"));
		return NULL;
	}

	return dre_xref;
}







