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
 * Note: this file originally auto-generated by mib2c using
 *        : mib2c.array-user.conf,v 5.15.2.1 2003/02/27 05:59:41 rstory Exp $
 *
 * $Id$
 *
 */
#ifndef SAHPISYSTEMEVENTLOGTABLE_H
#define SAHPISYSTEMEVENTLOGTABLE_H

#ifdef __cplusplus
extern          "C" {
#endif


#include <net-snmp/net-snmp-config.h>
#include <net-snmp/library/container.h>
#include <net-snmp/agent/table_array.h>
#include <SaHpi.h>
#include <hpiSubagent.h>
        /** Index saHpiDomainID is external */
        /** Index saHpiResourceID is external */
        /** Index saHpiSystemEventLogEntryId is internal */

    typedef struct saHpiSystemEventLogTable_context_s {
        netsnmp_index   index;
      
        /** UNSIGNED32 = ASN_UNSIGNED */
        unsigned long   saHpiSystemEventLogEntryId;

        /** TimeStamp = ASN_TIMETICKS */
        unsigned long   saHpiSystemEventLogTimestamp;

        /** RowPointer = ASN_OBJECT_ID */
        oid             saHpiSystemEventLogged[MAX_OID_LEN];
        long            saHpiSystemEventLogged_len;

      /** RowStatus = ASN_INTEGER */
        long            saHpiSystemEventClearEventTable;

        /** INTEGER = ASN_INTEGER */
      // long            saHpiSystemEventLogState;


      long hash;
      unsigned long resource_id;
      unsigned long domain_id;

    } saHpiSystemEventLogTable_context;

/*************************************************************
 * function declarations
 */
   
    void            initialize_table_saHpiSystemEventLogTable(void);
    const saHpiSystemEventLogTable_context
        *saHpiSystemEventLogTable_get_by_idx(netsnmp_index *);
    const saHpiSystemEventLogTable_context
        *saHpiSystemEventLogTable_get_by_idx_rs(netsnmp_index *,
                                                int row_status);
    int             saHpiSystemEventLogTable_get_value(netsnmp_request_info
                                                       *, netsnmp_index *,
                                                       netsnmp_table_request_info
                                                       *);
  int populate_sel(SaHpiRptEntryT *rpt_entry);

int
saHpiSystemEventLogTable_modify_context(SaHpiSelEntryT *sel,
					//SaHpiBoolT *state,
					SaHpiRptEntryT *rpt,
					oid *event_entry, 
					size_t event_entry_oid_len,
					saHpiSystemEventLogTable_context *ctx);
		   
  int send_saHpiSystemEventLogTable_notification(saHpiSystemEventLogTable_context);

  //int
  //set_logstate(saHpiSystemEventLogTable_context *ctx);

int 
set_clear_event_table(saHpiSystemEventLogTable_context *ctx);

  //int
  //set_timestamp();

int
delete_SEL_row(SaHpiDomainIdT domain_id,
	      SaHpiResourceIdT resource_id,
	      SaHpiSelEntryIdT num);

int
event_log_update_timestamp_handler(netsnmp_mib_handler *handler,
			 netsnmp_handler_registration *reginfo,
			 netsnmp_agent_request_info *reqinfo,
			 netsnmp_request_info *requests);
int
event_log_current_timestamp_handler(netsnmp_mib_handler *handler,
			 netsnmp_handler_registration *reginfo,
			 netsnmp_agent_request_info *reqinfo,
			 netsnmp_request_info *requests);


int
event_log_entries_handler(netsnmp_mib_handler *handler,
			 netsnmp_handler_registration *reginfo,
			 netsnmp_agent_request_info *reqinfo,
			 netsnmp_request_info *requests);
/*************************************************************
 * oid declarations
 */
    extern oid      saHpiSystemEventLogTable_oid[];
    extern size_t   saHpiSystemEventLogTable_oid_len;
//1,3,6,1,3,90,2,2,9
#define saHpiSystemEventLogTable_TABLE_OID systemEvents_OID, 9


/*************************************************************
 * column number definitions for table saHpiSystemEventLogTable
 */
#define COLUMN_SAHPISYSTEMEVENTLOGENTRYID 1
#define COLUMN_SAHPISYSTEMEVENTLOGTIMESTAMP 2
#define COLUMN_SAHPISYSTEMEVENTLOGGED 3
#define COLUMN_SAHPISYSTEMEVENTCLEAREVENTTABLE 4
  //#define COLUMN_SAHPISYSTEMEVENTLOGSTATE 5
#define saHpiSystemEventLogTable_COL_MIN 1
#define saHpiSystemEventLogTable_COL_MAX 4

   
   

    int            
        saHpiSystemEventLogTable_extract_index
        (saHpiSystemEventLogTable_context * ctx, netsnmp_index * hdr);
  
    void           
        saHpiSystemEventLogTable_set_reserve1(netsnmp_request_group *);
    void           
        saHpiSystemEventLogTable_set_reserve2(netsnmp_request_group *);
    void           
        saHpiSystemEventLogTable_set_action(netsnmp_request_group *);
    void           
        saHpiSystemEventLogTable_set_commit(netsnmp_request_group *);
    void            saHpiSystemEventLogTable_set_free(netsnmp_request_group
                                                      *);
    void            saHpiSystemEventLogTable_set_undo(netsnmp_request_group
                                                      *);

    saHpiSystemEventLogTable_context
        *saHpiSystemEventLogTable_duplicate_row
        (saHpiSystemEventLogTable_context *);
  
    netsnmp_index 
        *saHpiSystemEventLogTable_delete_row
        (saHpiSystemEventLogTable_context *);

    int            
        saHpiSystemEventLogTable_can_delete
        (saHpiSystemEventLogTable_context * undo_ctx,
         saHpiSystemEventLogTable_context * row_ctx,
         netsnmp_request_group * rg);



    saHpiSystemEventLogTable_context
        *saHpiSystemEventLogTable_create_row(netsnmp_index *);

  /*
    saHpiSystemEventLogTable_context *saHpiSystemEventLogTable_get(const
                                                                   char
                                                                   *name,
                                                                   int
                                                                   len);

  */
#ifdef __cplusplus
};
#endif

#endif /** SAHPISYSTEMEVENTLOGTABLE_H */
