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
 * Yes, there is lots of code here that you might not use. But it is much
 * easier to remove code than to add it!
 */
#ifndef SAHPIRDRTABLE_H
#define SAHPIRDRTABLE_H

#ifdef __cplusplus
extern          "C" {
#endif


#include <net-snmp/net-snmp-config.h>
#include <net-snmp/library/container.h>
#include <net-snmp/agent/table_array.h>
#include <SaHpi.h>
#include <hpiSubagent.h>


  
  typedef struct saHpiRdrTable_context_s {
      netsnmp_index   index;

       /** UNSIGNED32 = ASN_UNSIGNED */
        unsigned long   saHpiResourceID;

        /** UNSIGNED32 = ASN_UNSIGNED */
        unsigned long   saHpiRdrRecordId;

        /** INTEGER = ASN_INTEGER */
        long            saHpiRdrType;

        /** OCTETSTR = ASN_OCTET_STR */
        unsigned char   saHpiRdrEntityPath[SNMP_MAX_MSG_SIZE];
        long            saHpiRdrEntityPath_len;

        /** RowPointer = ASN_OBJECT_ID */
        oid             saHpiRdr[MAX_OID_LEN];
        long            saHpiRdr_len;

        /** COUNTER = ASN_COUNTER */
        unsigned long   saHpiRdrId;

        /** RowPointer = ASN_OBJECT_ID */
          oid             saHpiRdrRTP[MAX_OID_LEN];
        long            saHpiRdrRTP_len;


      long hash;

    } saHpiRdrTable_context;

/*************************************************************
 * function declarations
 */

    void            initialize_table_saHpiRdrTable(void);

    int             saHpiRdrTable_get_value(netsnmp_request_info *,
                                            netsnmp_index *,
                                            netsnmp_table_request_info *);

 int populate_rdr(SaHpiRptEntryT *rpt_entry,
		  SaHpiResourceIdT resource_id,
		  oid *rpt_oid, size_t rpt_oid_len);

int
delete_rdr_row(SaHpiDomainIdT domain_id,
	       SaHpiResourceIdT resource_id,
	       SaHpiEntryIdT num,
	       SaHpiRdrTypeT type);
int  
saHpiRdrTable_modify_context(SaHpiResourceIdT resource_id,
			     SaHpiRdrT *entry, 
			     saHpiRdrTable_context *ctx,
			     oid* rdr_oid, size_t oid_len,
			     oid* child_oid, size_t child_oid_len,
			     unsigned long child_id);


  int send_saHpiRdrTable_notification(saHpiRdrTable_context *ctx);

void
make_SaHpiRdrTable_trap_msg(netsnmp_variable_list *list, 
	      netsnmp_index *index,
	      int column, 
	      u_char type,
	      const u_char *value, 
	      const size_t value_len);


/*************************************************************
 * oid declarations
 */
    extern oid      saHpiRdrTable_oid[];
    extern size_t   saHpiRdrTable_oid_len;
//1,3,6,1,3,90,3,2
#define saHpiRdrTable_TABLE_OID hpiResources_OID,2


/*************************************************************
 * column number definitions for table saHpiRdrTable
 */
#define COLUMN_SAHPIRDRRESOURCEID 1
#define COLUMN_SAHPIRDRRECORDID 1
#define COLUMN_SAHPIRDRTYPE 2
#define COLUMN_SAHPIRDRENTITYPATH 3
#define COLUMN_SAHPIRDR 4
#define COLUMN_SAHPIRDRID 5
#define COLUMN_SAHPIRDRRTP 6
#define saHpiRdrTable_COL_MIN 1
#define saHpiRdrTable_COL_MAX 6

#define SCALAR_COLUMN_SAHPIRDRCOUNT 1


    int             saHpiRdrTable_extract_index(saHpiRdrTable_context *
                                                ctx, netsnmp_index * hdr);


    saHpiRdrTable_context *saHpiRdrTable_create_row(netsnmp_index *);



#ifdef __cplusplus
};
#endif

#endif /** SAHPIRDRTABLE_H */
