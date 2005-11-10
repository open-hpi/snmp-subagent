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

#include <hpiLock.h>
#include <hpiSubagent.h>
#include <net-snmp/library/snmp_debug.h>

GStaticRecMutex thread_mutex = G_STATIC_REC_MUTEX_INIT;;
int lockcount = 0;

void subagent_lock(GStaticRecMutex * thread_mutex, int * lockcount)                                                                           
{                                                                                        
        if (!g_static_rec_mutex_trylock(thread_mutex)) {                                    
                
		DEBUGMSGTL ((AGENT, "Going to block for a lock now. lockcount %d\n", *lockcount));                                      
                
		g_static_rec_mutex_lock(thread_mutex);                                      
                
		*lockcount++;                                                             
                
		DEBUGMSGTL ((AGENT,"Got the lock after blocking, lockcount %d\n", *lockcount));
		
        } else {                                                                            
                
		*lockcount++;                                                             
                
		DEBUGMSGTL ((AGENT,"Got the lock because no one had it. lockcount %d\n", *lockcount));                             
        }                                                                                   
}

void subagent_unlock(GStaticRecMutex * thread_mutex, int * lockcount)                                                      \
{
        									
	*lockcount--;						      
        
	g_static_rec_mutex_unlock(thread_mutex);				
        
	DEBUGMSGTL ((AGENT,"Released the lock, lockcount %d\n", *lockcount));	
        
}
