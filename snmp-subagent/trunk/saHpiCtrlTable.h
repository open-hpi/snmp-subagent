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
#ifndef SAHPICTRLTABLE_H
#define SAHPICTRLTABLE_H

#ifdef __cplusplus
extern          "C" {
#endif


#include <net-snmp/net-snmp-config.h>
#include <net-snmp/library/container.h>
#include <net-snmp/agent/table_array.h>
#include <SaHpi.h>

#define TEXT_MAX 255
#define SAHPI_CTRL_STATE_MAX 7//sizeof(SaHpiCtrlStateStreamT)
#define SAHPI_CTRL_ATTR_MAX 269
#define POS_DIGITAL 0
#define POS_DISCRETE 1


    typedef struct saHpiCtrlTable_context_s {
        netsnmp_index   index;

        /** UNSIGNED32 = ASN_UNSIGNED */
        unsigned long   saHpiCtrlNum;

        /** INTEGER = ASN_INTEGER */
        long            saHpiCtrlOutputType;

        /** TruthValue = ASN_INTEGER */
        long            saHpiCtrlBool;

        /** INTEGER = ASN_INTEGER */
        long            saHpiCtrlType;

        /** OCTETSTR = ASN_OCTET_STR */
        unsigned char   saHpiCtrlState[SAHPI_CTRL_STATE_MAX];
        long            saHpiCtrlState_len;

      /** OCTETSTR = ASN_OCTET_STR */
        unsigned char   saHpiCtrlAttributes[SAHPI_CTRL_ATTR_MAX];
        long            saHpiCtrlAttributes_len;

        /** UNSIGNED32 = ASN_UNSIGNED */
        unsigned long   saHpiCtrlOem;

        /** INTEGER = ASN_INTEGER */
        long            saHpiCtrlTextType;

        /** INTEGER = ASN_INTEGER */
        long            saHpiCtrlTextLanguage;

        /** OCTETSTR = ASN_OCTET_STR */
        unsigned char   saHpiCtrlText[TEXT_MAX];
        long            saHpiCtrlText_len;

        /** RowPointer = ASN_OBJECT_ID */
        oid             saHpiCtrlRDR[MAX_OID_LEN];
        long            saHpiCtrlRDR_len;

      
      long resource_id;
      long hash;
      long domain_id;
    } saHpiCtrlTable_context;

/*************************************************************
 * function declarations
 */

    void            initialize_table_saHpiCtrlTable(void);

    int             saHpiCtrlTable_get_value(netsnmp_request_info *,
                                             netsnmp_index *,
                                             netsnmp_table_request_info *);

int  populate_control(SaHpiCtrlRecT *ctrl, 
		      SaHpiRptEntryT *rpt_entry,
		      oid *rdr_entry_oid, size_t rdr_entry_oid_len,
		      oid *ctrl_oid, size_t *ctrl_oid_len);
  


int 
set_ctrl_state(saHpiCtrlTable_context *ctx);


/*************************************************************
 * oid declarations
 */
    extern oid      saHpiCtrlTable_oid[];
    extern size_t   saHpiCtrlTable_oid_len;
//1,3,6,1,3,90,3,4
#define saHpiCtrlTable_TABLE_OID hpiResources_OID,4


/*************************************************************
 * column number definitions for table saHpiCtrlTable
 */

#define COLUMN_SAHPICTRLNUM 1
#define COLUMN_SAHPICTRLOUTPUTTYPE 2
#define COLUMN_SAHPICTRLBOOL 3
#define COLUMN_SAHPICTRLTYPE 4
#define COLUMN_SAHPICTRLSTATE 5
#define COLUMN_SAHPICTRLATTRIBUTES 6
#define COLUMN_SAHPICTRLOEM 7
#define COLUMN_SAHPICTRLTEXTTYPE 8
#define COLUMN_SAHPICTRLTEXTLANGUAGE 9
#define COLUMN_SAHPICTRLTEXT 10
#define COLUMN_SAHPICTRLRDR 11
#define saHpiCtrlTable_COL_MIN 1
#define saHpiCtrlTable_COL_MAX 11

  

    int             saHpiCtrlTable_extract_index(saHpiCtrlTable_context *
                                                 ctx, netsnmp_index * hdr);
  
    void            saHpiCtrlTable_set_reserve1(netsnmp_request_group *);
    void            saHpiCtrlTable_set_reserve2(netsnmp_request_group *);
    void            saHpiCtrlTable_set_action(netsnmp_request_group *);
    void            saHpiCtrlTable_set_commit(netsnmp_request_group *);
    void            saHpiCtrlTable_set_free(netsnmp_request_group *);
    void            saHpiCtrlTable_set_undo(netsnmp_request_group *);

    saHpiCtrlTable_context
        *saHpiCtrlTable_duplicate_row(saHpiCtrlTable_context *);
    netsnmp_index  *saHpiCtrlTable_delete_row(saHpiCtrlTable_context *);

    int             saHpiCtrlTable_can_delete(saHpiCtrlTable_context *
                                              undo_ctx,
                                              saHpiCtrlTable_context *
                                              row_ctx,
                                              netsnmp_request_group * rg);

  

    saHpiCtrlTable_context *saHpiCtrlTable_create_row(netsnmp_index *);


#ifdef __cplusplus
};
#endif

#endif /** SAHPICTRLTABLE_H */
