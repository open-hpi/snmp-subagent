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

#include <unistd.h>
#include <hpiLock.h>
#include <hpiSubagent.h>
#include <net-snmp/library/snmp_debug.h>

hpi_lock_type hpi_lock_data = {
        .thread_mutex = G_STATIC_REC_MUTEX_INIT,
        .lockcount = 0
};


void subagent_lock( hpi_lock_type * hpi_lock_data)                                                                           
{                                                                                        
        
	if (!g_static_rec_mutex_trylock(&hpi_lock_data->thread_mutex)) {                                    
                
		DEBUGMSGTL ((AGENT, "********Going to block for a lock now. %p - lockcount %d\n", 
		                                                   g_thread_self(), hpi_lock_data->lockcount));
								   
		g_static_rec_mutex_lock(&hpi_lock_data->thread_mutex);
                
		hpi_lock_data->lockcount++;
                
		DEBUGMSGTL ((AGENT,"*********Got the lock after blocking, %p - lockcount %d\n", 
		                                                   g_thread_self(), hpi_lock_data->lockcount));
		
        } else {  
                
		hpi_lock_data->lockcount++; 
                
		DEBUGMSGTL ((AGENT,"Got the lock because no one had it. %p - lockcount %d\n", 
		                                                   g_thread_self(), hpi_lock_data->lockcount)); 
        }                                                                                   
}

void subagent_unlock( hpi_lock_type * hpi_lock_data)
{
        						
	//sleep(1);
	
	hpi_lock_data->lockcount--;
        
	g_static_rec_mutex_unlock(&hpi_lock_data->thread_mutex);
        
	DEBUGMSGTL ((AGENT,"Released the lock, %p - lockcount %d\n", 
	                           g_thread_self(), hpi_lock_data->lockcount));
        
}
