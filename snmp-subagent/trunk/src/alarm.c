/*
* (C) Copyright IBM Corp. 2003
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  This
 * file and program are licensed under a BSD style license.  See
 * the Copying file included with the OpenHPI distribution for
 * full licensing terms.
 *
 * Authors:
 *   Konrad Rzeszutek <konradr@us.ibm.com>
 *
 * $Id$
 *
 */
 
#include <net-snmp/net-snmp-config.h>
#include <net-snmp/net-snmp-includes.h>
#include <net-snmp/agent/net-snmp-agent-includes.h>

#include <SaHpi.h>
#include <alarm.h>
#include <hpiSubagent.h>
#include <oh_utils.h>
#include <saHpiEventTable.h>
#include <saHpiEventLogInfoTable.h>
#include <session_info.h>
#include <hpiEventThread.h>

extern int alarm_interval;

int
init_alarm ()
{

        int rc = AGENT_ERR_NOERROR;
        
	DEBUGMSGTL ((AGENT, "init_alarm: Entry\n"));
	DEBUGMSGTL ((AGENT, "Polling events every %d seconds\n", alarm_interval));

        if (snmp_alarm_register (alarm_interval, SA_REPEAT, do_alarm, NULL) == 0)
        {
                rc = AGENT_ERR_MEMORY_FAULT;
        }

        DEBUGMSGTL ((AGENT, "init_alarm: Exit\n"));
        return rc;
}

void
do_alarm (unsigned int clientreg, void *clientarg)
{
        SaErrorT rv;
        SaHpiEventT          event;
        SaHpiRdrT            rdr;
        SaHpiRptEntryT       rpt_entry;
        SaHpiEvtQueueStatusT event_queue_status;

        SaHpiSessionIdT sessionid = get_session_id(SAHPI_UNSPECIFIED_DOMAIN_ID);

        DEBUGMSGTL ((AGENT, "do_alarm: Entry\n"));

        
        rv = saHpiEventGet (sessionid,
                            SAHPI_TIMEOUT_IMMEDIATE,
                            &event,
                            &rdr,
                            &rpt_entry,
                            &event_queue_status);
	
	
	while(rv == SA_OK) {
                DEBUGMSGTL ((AGENT, "sessionid [%d]\n", get_session_id(SAHPI_UNSPECIFIED_DOMAIN_ID)));
                DEBUGMSGTL ((AGENT, "Polling mechanism found an event\n"));
                snmp_log (LOG_INFO, "Polling mechanism found an event \n");

                snmp_log (LOG_INFO, "rv [%s]\n", oh_lookup_error(rv));
                snmp_log (LOG_INFO, "returned from saHpiEventGet\n");
                DEBUGMSGTL ((AGENT, "Event Type [%s]\n", 
                             oh_lookup_eventtype(event.EventType)));
                oh_print_event(&event, 0);

                switch (event.EventType) {
                case SAHPI_ET_RESOURCE:
                        rv = async_event_add(sessionid, &event, &rdr, &rpt_entry);
                        break;
                case SAHPI_ET_DOMAIN:
                        rv = async_event_add(sessionid, &event, &rdr, &rpt_entry);
                        break;
                case SAHPI_ET_SENSOR:              
                        rv = async_event_add(sessionid, &event, &rdr, &rpt_entry);
                        break;
                case SAHPI_ET_SENSOR_ENABLE_CHANGE:
                        rv = async_event_add(sessionid, &event, &rdr, &rpt_entry);
                        break;
                case SAHPI_ET_HOTSWAP:
                        rv = async_event_add(sessionid, &event, &rdr, &rpt_entry);
                        break;
                case SAHPI_ET_WATCHDOG:            
                        rv = async_event_add(sessionid, &event, &rdr, &rpt_entry);
                        break;
                case SAHPI_ET_HPI_SW:            
                        rv = async_event_add(sessionid, &event, &rdr, &rpt_entry);
                        break;
                case SAHPI_ET_OEM:              
                        rv = async_event_add(sessionid, &event, &rdr, &rpt_entry);
                        break;
                case SAHPI_ET_USER: 
                        rv = async_event_add(sessionid, &event, &rdr, &rpt_entry);
                        break;
                default:
                        break;
                }


		// Now check for updates to the event logs
		rv = event_log_info_update(sessionid);
		
		rv = saHpiEventGet (sessionid,
                            SAHPI_TIMEOUT_IMMEDIATE,
                            &event,
                            &rdr,
                            &rpt_entry,
                            &event_queue_status);
        

        }
	
	

	

        DEBUGMSGTL ((AGENT, "do_alarm: Exit\n"));
}

