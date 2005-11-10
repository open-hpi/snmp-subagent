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
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <glib.h>

#include <net-snmp/net-snmp-config.h>
#include <net-snmp/net-snmp-includes.h>
#include <net-snmp/agent/net-snmp-agent-includes.h>
#include <signal.h>

#include <SaHpi.h>
#include <oh_utils.h>
#include <session_info.h>
#include <hpiSubagent.h>
#include <hpiEventThread.h>
#include <saHpiEventTable.h>
#include <saHpiEventLogInfoTable.h>
#include <saHpiAnnouncementTable.h>
#include <saHpiAdministration.h>


GThread *event_thread = NULL;
GError *event_thread_error = NULL;
static gboolean run_threaded;
int rediscover;

SaHpiTimeoutT timeout = 2000000000;


static gpointer event_thread_loop(gpointer data);

gboolean get_run_threaded(void)
{
        return run_threaded;
}

void set_run_threaded(gboolean val)
{
        run_threaded = val;
}

static gpointer event_thread_loop(gpointer data)
{
        SaErrorT rv;
        SaHpiEventT          event;
        SaHpiRdrT            rdr;
        SaHpiRptEntryT       rpt_entry;
        SaHpiEvtQueueStatusT event_queue_status;
	
	int counter = 0;

        SaHpiSessionIdT sessionid = *(SaHpiSessionIdT *)data;

        while(get_run_threaded()) {
	
		memset(&event, 0, sizeof(event));
                DEBUGMSGTL ((AGENT, "event_thread_loop started ---- sessionid [%d]\n", sessionid));
                rv = saHpiEventGet (get_session_id(SAHPI_UNSPECIFIED_DOMAIN_ID),
                                    timeout,
                                    &event,
                                    &rdr,
                                    &rpt_entry,
                                    &event_queue_status);

                DEBUGMSGTL ((AGENT, "rv [%s]\n", oh_lookup_error(rv)));

                counter++;

                /* serialize access */

		if (rv == SA_OK) { // NEW
		
		 	DEBUGMSGTL ((AGENT, "Event Type [%s]\n", 
                                     oh_lookup_eventtype(event.EventType)));
               	 	oh_print_event(&event, 0);
                	
			switch (event.EventType) {
                		case SAHPI_ET_RESOURCE:
                        		DEBUGMSGTL ((AGENT, "SAHPI_ET_RESOURCE, [%s]\n",
                                     	oh_lookup_resourceeventtype(
                                             	event.EventDataUnion.ResourceEvent.ResourceEventType)));
                        		break;
                		case SAHPI_ET_DOMAIN:
                      		DEBUGMSGTL ((AGENT, "SAHPI_ET_DOMAIN, [%s]\n",
                                   oh_lookup_domaineventtype(
                                           event.EventDataUnion.DomainEvent.Type)));
                       			break;
                		case SAHPI_ET_SENSOR:              
                        		break;
                		case SAHPI_ET_SENSOR_ENABLE_CHANGE:
                        		break;
               		 	case SAHPI_ET_HOTSWAP:
                       			break;
                		case SAHPI_ET_WATCHDOG:            
                        		break;
                		case SAHPI_ET_HPI_SW:  
                        		DEBUGMSGTL ((AGENT, "SAHPI_ET_HPI_SW, [%s]\n",
                                     		oh_lookup_sweventtype(
                                             		event.EventDataUnion.HpiSwEvent.Type)));
                        		break;
                		case SAHPI_ET_OEM:              
                        		DEBUGMSGTL ((AGENT, "SAHPI_ET_HPI_SW, [%s]\n",
                                     		oh_lookup_sweventtype(
                                             		event.EventDataUnion.HpiSwEvent.Type)));
                        		break;
                		case SAHPI_ET_USER: 
                        		break;
                		default:
                       		 	break;
               	 	}

                	rv = async_event_add(sessionid, &event, &rdr, &rpt_entry);
						

		} // NEW
                /* serialize access */
		
		// Now check for updates to the event logs
		rv = event_log_info_update(sessionid);
		
		if (rediscover == SAHPI_TRUE)
		{
		     repopulate_tables(get_session_id(SAHPI_UNSPECIFIED_DOMAIN_ID));
		     rediscover = SAHPI_FALSE;
		}     
		
		
		if (counter == 30) { //check for new announcements every minute.
              		update_announcements(get_session_id(SAHPI_UNSPECIFIED_DOMAIN_ID));
			counter = 0;
		}

        }
        g_thread_exit(0);
        return data;
}


int start_event_thread(SaHpiSessionIdT *sessionid)
{
        DEBUGMSGTL ((AGENT, "Attempting to init event"));
        if (!g_thread_supported()) {
                DEBUGMSGTL ((AGENT, "Initializing thread support"));
                g_thread_init(NULL);
        } else {
                DEBUGMSGTL ((AGENT, "Already supporting threads"));
        }

        event_thread = g_thread_create(event_thread_loop,
                                       (gpointer)sessionid, 
                                       FALSE, 
                                       &event_thread_error);
        
	rediscover = SAHPI_FALSE;
 				       
				       
        if (event_thread == NULL) {
                return AGENT_ERR_INIT;
        } 

        return AGENT_ERR_NOERROR;
}





