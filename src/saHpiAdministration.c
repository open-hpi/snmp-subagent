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
 */

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <net-snmp/net-snmp-config.h>
#include <net-snmp/net-snmp-includes.h>
#include <net-snmp/agent/net-snmp-agent-includes.h>

#include <SaHpi.h>
#include <hpiSubagent.h>
#include <saHpiAdministration.h>
#include <saHpiDomainInfoTable.h>
#include <saHpiDomainAlarmTable.h>
#include <saHpiDomainReferenceTable.h>

#include <saHpiResourceTable.h>
#include <saHpiRdrTable.h>

#include <saHpiAnnunciatorTable.h>

#include <saHpiInventoryTable.h>
#include <saHpiFieldTable.h>
#include <saHpiAreaTable.h>

#include <saHpiSensorTable.h>
#include <saHpiCurrentSensorStateTable.h>
#include <saHpiSensorReadingMaxTable.h>
#include <saHpiSensorReadingMinTable.h>
#include <saHpiSensorReadingNominalTable.h>
#include <saHpiSensorReadingNormalMaxTable.h>
#include <saHpiSensorReadingNormalMinTable.h>
#include <saHpiSensorThdLowCriticalTable.h>
#include <saHpiSensorThdLowMajorTable.h>
#include <saHpiSensorThdLowMinorTable.h>
#include <saHpiSensorThdNegHysteresisTable.h>
#include <saHpiSensorThdPosHysteresisTable.h>
#include <saHpiSensorThdUpCriticalTable.h>
#include <saHpiSensorThdUpMajorTable.h>
#include <saHpiSensorThdUpMinorTable.h>

#include <saHpiCtrlAnalogTable.h>
#include <saHpiCtrlDigitalTable.h>
#include <saHpiCtrlDiscreteTable.h>
#include <saHpiCtrlOemTable.h>
#include <saHpiCtrlStreamTable.h>
#include <saHpiCtrlTextTable.h>

#include <saHpiWatchdogTable.h>
#include <saHpiHotSwapTable.h>
#include <saHpiAutoInsertTimeoutTable.h>

#include <saHpiEventTable.h>
#include <saHpiResourceEventTable.h>
#include <saHpiDomainEventTable.h>
#include <saHpiSensorEventTable.h>
#include <saHpiSensorEnableChangeEventTable.h>
#include <saHpiHotSwapEventTable.h>
#include <saHpiWatchdogEventTable.h>
#include <saHpiSoftwareEventTable.h>
#include <saHpiOEMEventTable.h>
#include <saHpiUserEventTable.h>
#include <saHpiAnnouncementTable.h>

#include <saHpiEventLogInfoTable.h>
#include <saHpiEventLogTable.h>
#include <saHpiResourceEventLogTable.h>
#include <saHpiDomainEventLogTable.h>
#include <saHpiSensorEventLogTable.h>
#include <saHpiSensorEnableChangeEventLogTable.h>
#include <saHpiHotSwapEventLogTable.h>
#include <saHpiWatchdogEventLogTable.h>
#include <saHpiSoftwareEventLogTable.h>
#include <saHpiOEMEventLogTable.h>
#include <saHpiUserEventLogTable.h>
#include <saHpiAnnouncementEventLogTable.h>
#include <session_info.h>
#include <hpiEventThread.h>
#include <alarm.h>

/*************************************************************
 * oid and function declarations scalars
 */
 
static int administration_discover = MIB_FALSE;
static oid saHpiDiscover_oid[] = { 1,3,6,1,4,1,18568,2,1,1,1,4 };

int handle_saHpiDiscover(netsnmp_mib_handler *handler,
                                        netsnmp_handler_registration *reginfo,
                                        netsnmp_agent_request_info   *reqinfo,
                                        netsnmp_request_info         *requests);
					
int initialize_scalar_saHpiDiscover(void);

void init_saHpiSoftwareEventLogTable(void);


/**
 * 
 * @handler:
 * @reginfo:
 * @reqinfo:
 * @requests:
 * 
 * @return:
 */  
int
handle_saHpiDiscover(netsnmp_mib_handler *handler,
                                        netsnmp_handler_registration *reginfo,
                                        netsnmp_agent_request_info   *reqinfo,
                                        netsnmp_request_info         *requests)
{
        /* We are never called for a GETNEXT if it's registered as a
           "instance", as it's "magically" handled for us.  */
        /* a instance handler also only hands us one request at a time, so
           we don't need to loop over a list of requests; we'll only get one. */

        
	int rc = SNMP_ERR_NOERROR;
	        
	DEBUGMSGTL ((AGENT, "handle_saHpiDiscover, called\n"));
        

        switch(reqinfo->mode) {

        case MODE_GET:
                snmp_set_var_typed_value(requests->requestvb, ASN_INTEGER,
        			        (u_char *) &administration_discover,
        			        sizeof(administration_discover));
                break;

        case MODE_SET_RESERVE1:
	        DEBUGMSGTL ((AGENT, "set_reserved 1, called\n"));
		rc = netsnmp_check_vb_type_and_size(requests->requestvb, ASN_INTEGER,
                      sizeof(administration_discover));
		      
		break;
	case MODE_SET_RESERVE2:

		break;
	case MODE_SET_ACTION:
		DEBUGMSGTL ((AGENT, "set_action, called\n"));
		if (*requests->requestvb->val.integer == MIB_TRUE) {
			administration_discover = MIB_FALSE;
			if (get_run_threaded() == TRUE) {
              		        rediscover = SAHPI_TRUE;
			}
			else {
			        do_rediscover = TRUE;
			}		
		}	
		break;
	case MODE_SET_COMMIT:
	      DEBUGMSGTL ((AGENT, "set_commit, called\n"));
	      break;
	
        default:
                /* we should never get here, so this is a really bad error */
                return SNMP_ERR_GENERR;
        }

        if (rc) {
           netsnmp_request_set_error( requests, rc );
	   return SNMP_ERR_GENERR;
	}   
	else {   return SNMP_ERR_NOERROR; }
}

/**
 * registers the scalar as a read-write scalar.
 * @return: 
 */
int initialize_scalar_saHpiDiscover(void)
{

        DEBUGMSGTL ((AGENT, "initialize_scalar_saHpiDiscover, called\n"));

        
	netsnmp_register_int_instance("saHpiDiscover",
                                  saHpiDiscover_oid,
                                  OID_LENGTH(saHpiDiscover_oid),
                                  &administration_discover, handle_saHpiDiscover);
	

        return SNMP_ERR_NOERROR;
 
} 

/************************************************************
 * Initializes the saHpiAdministration module
 */
void
init_saHpiAdministration(void)
{

        DEBUGMSGTL ((AGENT, "init_saHpiAdministration, called\n"));

        initialize_scalar_saHpiDiscover();
	
}


/**
 * Called to repopulate all the tables.
 * @return: 
 */
void repopulate_tables(SaHpiSessionIdT session_id)
{

	populate_saHpiDomainInfoTable(session_id);

	populate_saHpiDomainAlarmTable(session_id);

	poplulate_saHpiDomainReferenceTable(session_id);	

	populate_saHpiResourceTable(session_id);
	    /* populate_saHpiResourceTable() calls:
	     *     populate_saHpiRdrTable(); calls:
	     *         populate_saHpiCtrlDigitalTable();		
	     *	       populate_saHpiCtrlDiscreteTable();		
	     *	       populate_saHpiCtrlAnalogTable();		
	     *	       populate_saHpiCtrlStreamTable();		
	     *	       populate_saHpiCtrlTextTable();		
	     *	       populate_saHpiCtrlOemTable();		
	     *	       populate_saHpiSensorTable();		
	     *	           populate_saHpiSesnorReadingMaxTable();		
	     *	           populate_saHpiSesnorReadingMinTable();		
	     *	           populate_saHpiSesnorReadingNominalTable();		
	     *	           populate_saHpiSesnorReadingNormalMaxTable();		
	     *	           populate_saHpiSesnorReadingNormalMinTable();		
	     *	           populate_saHpiSensorThdLowCriticalTable();		
	     *	           populate_saHpiSensorThdLowMajorTable();		
	     *	           populate_saHpiSensorThdLowMinorTable();		
	     *	           populate_saHpiSensorThdUpCriticalTable();		
	     *	           populate_saHpiSensorThdUpMajorTable();		
	     *	           populate_saHpiSensorThdUpMinorTable();		
	     *	           populate_saHpiSensorThdPosHysteresisTable();		
	     *	           populate_saHpiSensorThdNegHysteresisTable();		
	     *	       populate_saHpiCurrentSensorStateTable();		
	     *	       populate_saHpiInventoryTable();	
	     *       	   populate_saHpiAreaTable();		
	     *	               populate_saHpiFieldTable(); 	     	
	     *	       populate_saHpiWatchdogTable();		
	     *	       populate_saHpiAnnunciatorTable();		
             *         populate_saHpiHotSwapTable();
             *             populate_saHpiAutoInsertTimeoutTable();             
  	     *         populate_saHpiAnnouncementTable();
	     */  
        
	event_log_info_update(session_id);
	    /* populate_saHpiEventLog (sessionid);
             * 	   populate_saHpiResourceEventLogTable();
             * 	   populate_saHpiSensorEventLogTable();
	     * 	   populate_saHpiHotSwapEventLogTable();
	     * 	   populate_saHpiWatchdogEventLogTable();
	     * 	   populate_saHpiSoftwareEventLogTable();
	     * 	   populate_saHpiOemEventLogTable();
	     * 	   populate_saHpiUserEventLogTable();
	     *     populate_saHpiSensorEnableChangeEventLogTable();
	     *     populate_saHpiDomainEventLogTable();	     
             */


}



