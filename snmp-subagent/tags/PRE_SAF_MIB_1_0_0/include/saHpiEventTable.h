/*
 * Note: this file originally auto-generated by mib2c using
 *        : mib2c.array-user.conf,v 5.18.2.2 2004/02/09 18:21:47 rstory Exp $
 *
 * $Id$
 *
 * Yes, there is lots of code here that you might not use. But it is much
 * easier to remove code than to add it!
 */
#ifndef SAHPIEVENTTABLE_H
#define SAHPIEVENTTABLE_H

#ifdef __cplusplus
extern "C" {
#endif

    
#include <net-snmp/net-snmp-config.h>
#include <net-snmp/library/container.h>
#include <net-snmp/agent/table_array.h>

#include <SaHpi.h>

        /** Index saHpiEventRowPointer is internal */

typedef struct saHpiEventTable_context_s {
    netsnmp_index index; /** THIS MUST BE FIRST!!! */

    /*************************************************************
     * You can store data internally in this structure.
     *
     * TODO: You will probably have to fix a few types here...
     */
        /** RowPointer = ASN_OBJECT_ID */
            oid saHpiEventRowPointer[MAX_OID_LEN];
            long saHpiEventRowPointer_len;

        /** SaHpiSeverity = ASN_INTEGER */
            long saHpiEventSeverity;

        /** SaHpiTime = ASN_COUNTER64 */
    /** TODO: Is this type correct? */
            long saHpiEventSaHpiTime;

        /** INTEGER = ASN_INTEGER */
            long saHpiEventType;


    /*
     * OR
     *
     * Keep a pointer to your data
     */
    void * data;

    /*
     *add anything else you want here
     */

} saHpiEventTable_context;

/*************************************************************
 * function declarations
 */
void init_saHpiEventTable(void);
void initialize_table_saHpiEventTable(void);
const saHpiEventTable_context * saHpiEventTable_get_by_idx(netsnmp_index *);
const saHpiEventTable_context * saHpiEventTable_get_by_idx_rs(netsnmp_index *,
                                        int row_status);
int saHpiEventTable_get_value(netsnmp_request_info *, netsnmp_index *, netsnmp_table_request_info *);

/*************************************************************
 * function declarations
 */
SaErrorT populate_saHpiEventTable(SaHpiSessionIdT sessionid);
SaErrorT async_event_add(SaHpiSessionIdT sessionid, SaHpiEventT *event, 
                         SaHpiRdrT *rdr, SaHpiRptEntryT *rpt_entry);

/*************************************************************
 * oid declarations
 */
extern oid saHpiEventTable_oid[];
extern size_t saHpiEventTable_oid_len;

#define saHpiEventTable_TABLE_OID 1,3,6,1,4,1,18568,2,1,1,3,1,3
    
/*************************************************************
 * column number definitions for table saHpiEventTable
 */
//#define EVENT_INDEX_NR 1
#define COLUMN_SAHPIEVENTROWPOINTER 1
#define COLUMN_SAHPIEVENTSEVERITY 2
#define COLUMN_SAHPIEVENTSAHPITIME 3
#define COLUMN_SAHPIEVENTTYPE 4
#define saHpiEventTable_COL_MIN 2
#define saHpiEventTable_COL_MAX 4

int saHpiEventTable_extract_index( saHpiEventTable_context * ctx, netsnmp_index * hdr );

void saHpiEventTable_set_reserve1( netsnmp_request_group * );
void saHpiEventTable_set_reserve2( netsnmp_request_group * );
void saHpiEventTable_set_action( netsnmp_request_group * );
void saHpiEventTable_set_commit( netsnmp_request_group * );
void saHpiEventTable_set_free( netsnmp_request_group * );
void saHpiEventTable_set_undo( netsnmp_request_group * );

saHpiEventTable_context * saHpiEventTable_duplicate_row( saHpiEventTable_context* );
netsnmp_index * saHpiEventTable_delete_row( saHpiEventTable_context* );

int saHpiEventTable_can_activate(saHpiEventTable_context *undo_ctx,
                      saHpiEventTable_context *row_ctx,
                      netsnmp_request_group * rg);
int saHpiEventTable_can_deactivate(saHpiEventTable_context *undo_ctx,
                        saHpiEventTable_context *row_ctx,
                        netsnmp_request_group * rg);
int saHpiEventTable_can_delete(saHpiEventTable_context *undo_ctx,
                    saHpiEventTable_context *row_ctx,
                    netsnmp_request_group * rg);
    
    
saHpiEventTable_context * saHpiEventTable_create_row( netsnmp_index* );

saHpiEventTable_context * saHpiEventTable_get( const char *name, int len );

#ifdef __cplusplus
};
#endif

#endif /** SAHPIEVENTTABLE_H */
