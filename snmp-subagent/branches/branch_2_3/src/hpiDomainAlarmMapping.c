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

#include <stdio.h>
#include <stdlib.h>
#include <glib.h>
#include <fcntl.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <signal.h>
#include <SaHpi.h> 

#include <hpiDomainAlarmMapping.h>

#include <net-snmp/net-snmp-config.h>
#include <net-snmp/net-snmp-includes.h>
#include <net-snmp/agent/net-snmp-agent-includes.h>

#include <net-snmp/library/snmp_assert.h>


//static int initialized = FALSE;		      
//static GHashTable *hash_table;

guint domain_alarm_entry_hash(gconstpointer key);
gboolean domain_alarm_entry_equal(gconstpointer a, gconstpointer b);


/**
 * 
 * @key
 * 
 * @return 
 */
guint domain_alarm_entry_hash(gconstpointer key)
{
	const char *p = key;
	guint h = *p;

	int i;

	int de_len;

	de_len = sizeof(doma_keys);

	p += 1;

	for ( i=0; i < de_len - 1; i++ ) {
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
gboolean domain_alarm_entry_equal(gconstpointer a, gconstpointer b)
{
	if (!memcmp(a, b, sizeof(doma_keys))) {
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
SaErrorT domain_alarm_entry_initialize(int *initialized, GHashTable **hash_table) 
{
	SaErrorT rval = SA_OK;

	if (!*initialized) {

		/* initialize hash tables */
		*hash_table = g_hash_table_new(domain_alarm_entry_hash, 
                                               domain_alarm_entry_equal);

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
DE_XREF *domain_alarm_entry_get(doma_keys *de, GHashTable **hash_table) 
{
	gpointer key;
	gpointer value;

	DE_XREF *de_xref;

	if (!de) return 0;

	key = de;

	/* check for presence of DRE_XREF and */
	/* previously assigned uid      */        
	de_xref = (DE_XREF *)g_hash_table_lookup (*hash_table, key);        
	if (de_xref) {
		return de_xref;
	}

	/* allocate storage for DRE_XREF cross reference data structure*/
	de_xref = (DE_XREF *)g_malloc0(sizeof(DE_XREF));
	if (!de_xref) {
		DEBUGMSGTL ((MAPPING_FILE, "malloc failed"));
		return NULL;                
	}
	memcpy(&de_xref->de_pair, de, sizeof(doma_keys));

	/* entity path based key */   
	key = (gpointer)&de_xref->de_pair; 
	value = (gpointer)de_xref;
	g_hash_table_insert(*hash_table, key, value);

	return de_xref;
}               

/**
 * 
 * @param dre
 * @param hash_table
 * 
 * @return 
 */
DE_XREF *domain_alarm_entry_lookup(doma_keys *de, GHashTable **hash_table)
{
	DE_XREF *de_xref;

	gpointer key;

	if (!de) return 0;

	key = de;

	/* check hash table for entry in hash_table */
	de_xref = (DE_XREF *)g_hash_table_lookup (*hash_table, key);
	if (!de_xref) {
		DEBUGMSGTL ((MAPPING_FILE, "DE_XREF index not found!"));
		return NULL;
	}

	return de_xref;
}







