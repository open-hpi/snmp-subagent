/*
 * Note: this file originally auto-generated by mib2c using
 *       : mib2c.array-user.conf,v 5.18.2.2 2004/02/09 18:21:47 rstory Exp $
 *
 * $Id$
 *
 *
 * For help understanding NET-SNMP in general, please check the 
 *     documentation and FAQ at:
 *
 *     http://www.net-snmp.org/
 *
 *
 * For help understanding this code, the agent and how it processes
 *     requests, please check the following references.
 *
 *     http://www.net-snmp.org/tutorial-5/
 *
 *
 * You can also join the #net-snmp channel on irc.freenode.net
 *     and ask for help there.
 *
 *
 * And if all else fails, send a detailed message to the developers
 *     describing the problem you are having to:
 *
 *    net-snmp-coders@lists.sourceforge.net
 *
 *
 * Yes, there is lots of code here that you might not use. But it is much
 * easier to remove code than to add it!
 */
#include <net-snmp/net-snmp-config.h>
#include <net-snmp/net-snmp-includes.h>
#include <net-snmp/agent/net-snmp-agent-includes.h>

#include <net-snmp/library/snmp_assert.h>

#include <SaHpi.h>
#include "saHpiInventoryTable.h"
#include <hpiSubagent.h>
#include <hpiCheckIndice.h>
#include <saHpiResourceTable.h>
#include <saHpiAreaTable.h>
#include <session_info.h>
#include <oh_utils.h>

static     netsnmp_handler_registration *my_handler = NULL;
static     netsnmp_table_array_callbacks cb;

oid saHpiInventoryTable_oid[] = { saHpiInventoryTable_TABLE_OID };
size_t saHpiInventoryTable_oid_len = OID_LENGTH(saHpiInventoryTable_oid);

/************************************************************/
/************************************************************/
/************************************************************/
/************************************************************/

/*************************************************************
 * oid and fucntion declarations scalars
 */
static u_long inventory_entry_count = 0;
static oid saHpiInventoryEntryCount_oid[] = { 1,3,6,1,4,1,18568,2,1,1,4,8,1 };
int handle_saHpiInventoryEntryCount(netsnmp_mib_handler *handler,
                                    netsnmp_handler_registration *reginfo,
                                    netsnmp_agent_request_info   *reqinfo,
                                    netsnmp_request_info         *requests);
int initialize_table_saHpiInventoryEntryCount(void);

/**
 * 
 * @sessionid:
 * @rdr_entry:
 * @rpt_entry:
 * @full_oid:
 * @full_oid_len:
 * @child_oid:
 * @child_oid_len:
 * 
 * @return 
 */
SaErrorT populate_inventory (SaHpiSessionIdT sessionid, 
                             SaHpiRdrT *rdr_entry,
                             SaHpiRptEntryT *rpt_entry,
                             oid *full_oid, size_t full_oid_len,
                             oid *child_oid, size_t *child_oid_len)
{

        DEBUGMSGTL ((AGENT, "populate_inventory, called\n"));

        SaErrorT rv = SA_OK;    
        SaHpiIdrInfoT idr_info;

        oid inventory_oid[INVENTORY_INDEX_NR];
        netsnmp_index inventory_index;
        saHpiInventoryTable_context *inventory_context;

        oid column[2];
        int column_len = 2;

        DEBUGMSGTL ((AGENT, "SAHPI_INVENTORY_RDR populate_inventory() called\n"));

        /* check for NULL pointers */
        if (!rdr_entry) {
                DEBUGMSGTL ((AGENT, 
                             "ERROR: populate_inventory() passed NULL rdr_entry pointer\n"));
                return AGENT_ERR_INTERNAL_ERROR;
        }
        if (!rpt_entry) {
                DEBUGMSGTL ((AGENT, 
                             "ERROR: populate_inventory() passed NULL rdr_entry pointer\n"));
                return AGENT_ERR_INTERNAL_ERROR;
        }

        /* BUILD oid for new row */
        /* assign the number of indices */
        inventory_index.len = INVENTORY_INDEX_NR;
        /** Index saHpiDomainId is external */
        inventory_oid[0] = get_domain_id(sessionid);
        /** Index saHpiResourceId is external */
        inventory_oid[1] = rpt_entry->ResourceId;
        /** Index saHpiResourceIsHistorical is external */
        inventory_oid[2] = MIB_FALSE;
        /** Index saHpiSensorNum */
        inventory_oid[3] = rdr_entry->RdrTypeUnion.InventoryRec.IdrId;
        /* assign the indices to the index */
        inventory_index.oids = (oid *) & inventory_oid;

        /* create full oid on This row for parent RowPointer */
        column[0] = 1;
        column[1] = COLUMN_SAHPIINVENTORYPERSISTENT;
        memset(child_oid, 0, sizeof(child_oid_len));
        build_full_oid(saHpiInventoryTable_oid, saHpiInventoryTable_oid_len,
                       column, column_len,
                       &inventory_index,
                       child_oid, MAX_OID_LEN, child_oid_len);

        /* See if Row exists. */
        inventory_context = NULL;
        inventory_context = CONTAINER_FIND(cb.container, &inventory_index);

        if (!inventory_context) {
                // New entry. Add it
                inventory_context = 
                saHpiInventoryTable_create_row(&inventory_index);
        }
        if (!inventory_context) {
                snmp_log (LOG_ERR, "Not enough memory for a Annunciator row!");
                return AGENT_ERR_INTERNAL_ERROR;
        }

        /** SaHpiInstrumentId = ASN_UNSIGNED */
        inventory_context->saHpiInventoryId = 
                rdr_entry->RdrTypeUnion.InventoryRec.IdrId;

        /** TruthValue = ASN_INTEGER */
        inventory_context->saHpiInventoryPersistent =
                (rdr_entry->RdrTypeUnion.InventoryRec.Persistent == SAHPI_TRUE)
                ? MIB_TRUE : MIB_FALSE;

        /** UNSIGNED32 = ASN_UNSIGNED */
        inventory_context->saHpiInventoryOEM =
                rdr_entry->RdrTypeUnion.InventoryRec.Oem;

        /*******************/
        /* Idr Info Record */
        /*******************/
        rv = saHpiIdrInfoGet(sessionid, rpt_entry->ResourceId, 
                             rdr_entry->RdrTypeUnion.InventoryRec.IdrId,
                             &idr_info);
        if (rv != SA_OK) {
                DEBUGMSGTL ((AGENT, 
                             "ERROR: populate_inventory() saHpiIdrInfoGet() ERRORED out\n"));
                saHpiInventoryTable_delete_row( inventory_context );
                return AGENT_ERR_INTERNAL_ERROR;
        } 

        /** UNSIGNED32 = ASN_UNSIGNED */
        inventory_context->saHpiInventoryUpdateCount =
                idr_info.UpdateCount;

        /** TruthValue = ASN_INTEGER */
        inventory_context->saHpiInventoryIsReadOnly = 
                (idr_info.ReadOnly == SAHPI_TRUE) ? MIB_TRUE : MIB_FALSE;

        /** UNSIGNED32 = ASN_UNSIGNED */
        inventory_context->saHpiInventoryNumAreas =
                idr_info.NumAreas;   

        /** RowPointer = ASN_OBJECT_ID */
        memset(inventory_context->saHpiInventoryRDR,
               0, sizeof(inventory_context->saHpiInventoryRDR));
        inventory_context->saHpiInventoryRDR_len =
                full_oid_len * sizeof(oid);
        memcpy(inventory_context->saHpiInventoryRDR, 
               full_oid, 
               inventory_context->saHpiInventoryRDR_len);

        /********************************************/
        /* populate all Areas contained by this Idr */
        /********************************************/
        rv = populate_area (sessionid, rdr_entry, rpt_entry);

        CONTAINER_INSERT (cb.container, inventory_context);

        inventory_entry_count = CONTAINER_SIZE (cb.container);

        return rv;
} 

/**
 * 
 * @handler:
 * @reginfo:
 * @reqinfo:
 * @requests:
 * 
 * @return:
 */
int handle_saHpiInventoryEntryCount(netsnmp_mib_handler *handler,
                                    netsnmp_handler_registration *reginfo,
                                    netsnmp_agent_request_info   *reqinfo,
                                    netsnmp_request_info         *requests)
{
    /* We are never called for a GETNEXT if it's registered as a
       "instance", as it's "magically" handled for us.  */

    /* a instance handler also only hands us one request at a time, so
       we don't need to loop over a list of requests; we'll only get one. */

    inventory_entry_count = CONTAINER_SIZE (cb.container);
    
    switch(reqinfo->mode) {

        case MODE_GET:
            snmp_set_var_typed_value(requests->requestvb, ASN_COUNTER,
                                     (u_char *) &inventory_entry_count,
                                     sizeof(inventory_entry_count));
            break;


        default:
            /* we should never get here, so this is a really bad error */
            return SNMP_ERR_GENERR;
    }

    return SNMP_ERR_NOERROR;
}

/**
 * 
 * @return: 
 */
int initialize_table_saHpiInventoryEntryCount(void)
{

        DEBUGMSGTL ((AGENT, "initialize_table_saHpiInventoryEntryCount, called\n"));

        netsnmp_register_scalar(
                netsnmp_create_handler_registration(
                        "saHpiInventoryEntryCount", 
                        handle_saHpiInventoryEntryCount,
                        saHpiInventoryEntryCount_oid, 
                        OID_LENGTH(saHpiInventoryEntryCount_oid),
                        HANDLER_CAN_RONLY ));
        return SNMP_ERR_NOERROR;
}

/************************************************************/
/************************************************************/
/************************************************************/
/************************************************************/

/************************************************************
 * keep binary tree to find context by name
 */
static int saHpiInventoryTable_cmp( const void *lhs, const void *rhs );

/************************************************************
 * compare two context pointers here. Return -1 if lhs < rhs,
 * 0 if lhs == rhs, and 1 if lhs > rhs.
 */
static int
saHpiInventoryTable_cmp( const void *lhs, const void *rhs )
{
    saHpiInventoryTable_context *context_l =
        (saHpiInventoryTable_context *)lhs;
    saHpiInventoryTable_context *context_r =
        (saHpiInventoryTable_context *)rhs;

    /*
     * check primary key, then secondary. Add your own code if
     * there are more than 2 indexes
     */
	DEBUGMSGTL ((AGENT, "saHpiAnnunciatorTable_cmp, called\n"));

	/* check for NULL pointers */
	if (lhs == NULL || rhs == NULL ) {
		DEBUGMSGTL((AGENT,"saHpiAnnunciatorTable_cmp() NULL pointer ERROR\n" ));
		return 0;
	}
	/* CHECK FIRST INDEX,  saHpiDomainId */
	if ( context_l->index.oids[0] < context_r->index.oids[0])
		return -1;

	if ( context_l->index.oids[0] > context_r->index.oids[0])
		return 1;

	if ( context_l->index.oids[0] == context_r->index.oids[0]) {
		/* If saHpiDomainId index is equal sort by second index */
		/* CHECK SECOND INDEX,  saHpiResourceEntryId */
		if ( context_l->index.oids[1] < context_r->index.oids[1])
			return -1;

		if ( context_l->index.oids[1] > context_r->index.oids[1])
			return 1;

		if ( context_l->index.oids[1] == context_r->index.oids[1]) {
			/* If saHpiResourceEntryId index is equal sort by third index */
			/* CHECK THIRD INDEX,  saHpiResourceIsHistorical */
			if ( context_l->index.oids[2] < context_r->index.oids[2])
				return -1;

			if ( context_l->index.oids[2] > context_r->index.oids[2])
				return 1;

			if ( context_l->index.oids[2] == context_r->index.oids[2]) {
				/* If saHpiResourceIsHistorical index is equal sort by forth index */
				/* CHECK FORTH INDEX,  saHpiInventoryId */
				if ( context_l->index.oids[3] < context_r->index.oids[3])
					return -1;

				if ( context_l->index.oids[3] > context_r->index.oids[3])
					return 1;

				if ( context_l->index.oids[3] == context_r->index.oids[3])
					return 0;
			}
		}
	}

	return 0;
}

/************************************************************
 * Initializes the saHpiInventoryTable module
 */
void
init_saHpiInventoryTable(void)
{
	DEBUGMSGTL ((AGENT, "init_saHpiInventoryTable, called\n"));

    initialize_table_saHpiInventoryTable();

    initialize_table_saHpiInventoryEntryCount();

    /*
     * TODO: perform any startup stuff here, such as
     * populating the table with initial data.
     *
     * saHpiInventoryTable_context * new_row = create_row(index);
     * CONTAINER_INSERT(cb.container,new_row);
     */
}

/************************************************************
 * the *_row_copy routine
 */
static int saHpiInventoryTable_row_copy(saHpiInventoryTable_context * dst,
                         saHpiInventoryTable_context * src)
{
	DEBUGMSGTL ((AGENT, "saHpiInventoryTable_row_copy, called\n"));

    if(!dst||!src)
        return 1;
        
    /*
     * copy index, if provided
     */
    if(dst->index.oids)
        free(dst->index.oids);
    if(snmp_clone_mem( (void*)&dst->index.oids, src->index.oids,
                           src->index.len * sizeof(oid) )) {
        dst->index.oids = NULL;
        return 1;
    }
    dst->index.len = src->index.len;
    

    /*
     * copy components into the context structure
     */
    /** TODO: add code for external index(s)! */
    dst->saHpiInventoryId = src->saHpiInventoryId;

    dst->saHpiInventoryPersistent = src->saHpiInventoryPersistent;

    dst->saHpiInventoryOEM = src->saHpiInventoryOEM;

    dst->saHpiInventoryUpdateCount = src->saHpiInventoryUpdateCount;

    dst->saHpiInventoryIsReadOnly = src->saHpiInventoryIsReadOnly;

    dst->saHpiInventoryNumAreas = src->saHpiInventoryNumAreas;

    memcpy( src->saHpiInventoryRDR, dst->saHpiInventoryRDR, src->saHpiInventoryRDR_len );
    dst->saHpiInventoryRDR_len = src->saHpiInventoryRDR_len;

    return 0;
}

/**
 * the *_extract_index routine
 *
 * This routine is called when a set request is received for an index
 * that was not found in the table container. Here, we parse the oid
 * in the the individual index components and copy those indexes to the
 * context. Then we make sure the indexes for the new row are valid.
 */
int
saHpiInventoryTable_extract_index( saHpiInventoryTable_context * ctx, netsnmp_index * hdr )
{
    /*
     * temporary local storage for extracting oid index
     *
     * extract index uses varbinds (netsnmp_variable_list) to parse
     * the index OID into the individual components for each index part.
     */
    /** TODO: add storage for external index(s)! */
    netsnmp_variable_list var_saHpiDomainId;
    netsnmp_variable_list var_saHpiResourceId;
    netsnmp_variable_list var_saHpiResourceIsHistorical;
    netsnmp_variable_list var_saHpiInventoryId;
    int err;

    DEBUGMSGTL ((AGENT, "saHpiInventoryTable_extract_index, called\n"));

    /*
     * copy index, if provided
     */
    if(hdr) {
        netsnmp_assert(ctx->index.oids == NULL);
        if(snmp_clone_mem( (void*)&ctx->index.oids, hdr->oids,
                           hdr->len * sizeof(oid) )) {
            return -1;
        }
        ctx->index.len = hdr->len;
    }

    /*
     * initialize variable that will hold each component of the index.
     * If there are multiple indexes for the table, the variable_lists
     * need to be linked together, in order.
     */
       /** TODO: add code for external index(s)! */
       memset( &var_saHpiDomainId, 0x00, sizeof(var_saHpiDomainId) );
       var_saHpiDomainId.type = ASN_UNSIGNED; /* type hint for parse_oid_indexes */
       /** TODO: link this index to the next, or NULL for the last one */
       var_saHpiDomainId.next_variable = &var_saHpiResourceId;

       memset( &var_saHpiResourceId, 0x00, sizeof(var_saHpiResourceId) );
       var_saHpiResourceId.type = ASN_UNSIGNED; /* type hint for parse_oid_indexes */
       /** TODO: link this index to the next, or NULL for the last one */
       var_saHpiResourceId.next_variable = &var_saHpiResourceIsHistorical;

       memset( &var_saHpiResourceIsHistorical, 0x00, sizeof(var_saHpiResourceIsHistorical) );
       var_saHpiResourceIsHistorical.type = ASN_INTEGER; /* type hint for parse_oid_indexes */
       /** TODO: link this index to the next, or NULL for the last one */
       var_saHpiResourceIsHistorical.next_variable = &var_saHpiInventoryId;

       memset( &var_saHpiInventoryId, 0x00, sizeof(var_saHpiInventoryId) );
       var_saHpiInventoryId.type = ASN_UNSIGNED; /* type hint for parse_oid_indexes */
       /** TODO: link this index to the next, or NULL for the last one */
       var_saHpiInventoryId.next_variable = NULL;


    /*
     * parse the oid into the individual index components
     */
    err = parse_oid_indexes( hdr->oids, hdr->len, &var_saHpiDomainId );
    if (err == SNMP_ERR_NOERROR) {
       /*
        * copy index components into the context structure
        */
              /** skipping external index saHpiDomainId */
   
              /** skipping external index saHpiResourceId */
   
              /** skipping external index saHpiResourceIsHistorical */
   
                ctx->saHpiInventoryId = *var_saHpiInventoryId.val.integer;
   
   
                err = saHpiDomainId_check_index(
                                               *var_saHpiDomainId.val.integer);
                err = saHpiResourceEntryId_check_index(
                                                      *var_saHpiResourceId.val.integer);  
                err = saHpiResourceIsHistorical_check_index(
                                                           *var_saHpiResourceIsHistorical.val.integer);
                err = saHpiInventoryId_check_index(
                                                     *var_saHpiInventoryId.val.integer);
    }

    /*
     * parsing may have allocated memory. free it.
     */
    snmp_reset_var_buffers( &var_saHpiDomainId );

    return err;
}

/************************************************************
 * the *_can_activate routine is called
 * when a row is changed to determine if all the values
 * set are consistent with the row's rules for a row status
 * of ACTIVE.
 *
 * return 1 if the row could be ACTIVE
 * return 0 if the row is not ready for the ACTIVE state
 */
int saHpiInventoryTable_can_activate(saHpiInventoryTable_context *undo_ctx,
                      saHpiInventoryTable_context *row_ctx,
                      netsnmp_request_group * rg)
{
        DEBUGMSGTL ((AGENT, "saHpiInventoryTable_can_activate, called\n"));
    /*
     * TODO: check for activation requirements here
     */


    /*
     * be optimistic.
     */
    return 1;
}

/************************************************************
 * the *_can_deactivate routine is called when a row that is
 * currently ACTIVE is set to a state other than ACTIVE. If
 * there are conditions in which a row should not be allowed
 * to transition out of the ACTIVE state (such as the row being
 * referred to by another row or table), check for them here.
 *
 * return 1 if the row can be set to a non-ACTIVE state
 * return 0 if the row must remain in the ACTIVE state
 */
int saHpiInventoryTable_can_deactivate(saHpiInventoryTable_context *undo_ctx,
                        saHpiInventoryTable_context *row_ctx,
                        netsnmp_request_group * rg)
{
        DEBUGMSGTL ((AGENT, "saHpiInventoryTable_can_deactivate, called\n"));
    /*
     * TODO: check for deactivation requirements here
     */
    return 1;
}

/************************************************************
 * the *_can_delete routine is called to determine if a row
 * can be deleted.
 *
 * return 1 if the row can be deleted
 * return 0 if the row cannot be deleted
 */
int saHpiInventoryTable_can_delete(saHpiInventoryTable_context *undo_ctx,
                    saHpiInventoryTable_context *row_ctx,
                    netsnmp_request_group * rg)
{
        DEBUGMSGTL ((AGENT, "saHpiInventoryTable_can_delete, called\n"));
    /*
     * probably shouldn't delete a row that we can't
     * deactivate.
     */
    if(saHpiInventoryTable_can_deactivate(undo_ctx,row_ctx,rg) != 1)
        return 0;
    
    /*
     * TODO: check for other deletion requirements here
     */
    return 1;
}

/************************************************************
 * the *_create_row routine is called by the table handler
 * to create a new row for a given index. If you need more
 * information (such as column values) to make a decision
 * on creating rows, you must create an initial row here
 * (to hold the column values), and you can examine the
 * situation in more detail in the *_set_reserve1 or later
 * states of set processing. Simple check for a NULL undo_ctx
 * in those states and do detailed creation checking there.
 *
 * returns a newly allocated saHpiInventoryTable_context
 *   structure if the specified indexes are not illegal
 * returns NULL for errors or illegal index values.
 */
saHpiInventoryTable_context *
saHpiInventoryTable_create_row( netsnmp_index* hdr)
{
    saHpiInventoryTable_context * ctx =
        SNMP_MALLOC_TYPEDEF(saHpiInventoryTable_context);

    DEBUGMSGTL ((AGENT, "saHpiInventoryTable_create_row, called\n"));

    if(!ctx)
        return NULL;
        
    /*
     * TODO: check indexes, if necessary.
     */
    if(saHpiInventoryTable_extract_index( ctx, hdr )) {
        free(ctx->index.oids);
        free(ctx);
        return NULL;
    }

    /* netsnmp_mutex_init(ctx->lock);
       netsnmp_mutex_lock(ctx->lock); */

    /*
     * TODO: initialize any default values here. This is also
     * first place you really should allocate any memory for
     * yourself to use.  If you allocated memory earlier,
     * make sure you free it for earlier error cases!
     */
    /**
    */

    return ctx;
}

/************************************************************
 * the *_duplicate row routine
 */
saHpiInventoryTable_context *
saHpiInventoryTable_duplicate_row( saHpiInventoryTable_context * row_ctx)
{
    saHpiInventoryTable_context * dup;

    DEBUGMSGTL ((AGENT, "saHpiInventoryTable_duplicate_row, called\n"));

    if(!row_ctx)
        return NULL;

    dup = SNMP_MALLOC_TYPEDEF(saHpiInventoryTable_context);
    if(!dup)
        return NULL;
        
    if(saHpiInventoryTable_row_copy(dup,row_ctx)) {
        free(dup);
        dup = NULL;
    }

    return dup;
}

/************************************************************
 * the *_delete_row method is called to delete a row.
 */
netsnmp_index * saHpiInventoryTable_delete_row( saHpiInventoryTable_context * ctx )
{
  /* netsnmp_mutex_destroy(ctx->lock); */

        DEBUGMSGTL ((AGENT, "saHpiInventoryTable_delete_row, called\n"));

    if(ctx->index.oids)
        free(ctx->index.oids);

    /*
     * TODO: release any memory you allocated here...
     */

    /*
     * release header
     */
    free( ctx );

    return NULL;
}


/************************************************************
 * RESERVE is used to check the syntax of all the variables
 * provided, that the values being set are sensible and consistent,
 * and to allocate any resources required for performing the SET.
 * After this stage, the expectation is that the set ought to
 * succeed, though this is not guaranteed. (In fact, with the UCD
 * agent, this is done in two passes - RESERVE1, and
 * RESERVE2, to allow for dependancies between variables).
 *
 * BEFORE calling this routine, the agent will call duplicate_row
 * to create a copy of the row (unless this is a new row; i.e.
 * row_created == 1).
 *
 * next state -> SET_RESERVE2 || SET_FREE
 */
void saHpiInventoryTable_set_reserve1( netsnmp_request_group *rg )
{
//    saHpiInventoryTable_context *row_ctx =
//            (saHpiInventoryTable_context *)rg->existing_row;
//    saHpiInventoryTable_context *undo_ctx =
//            (saHpiInventoryTable_context *)rg->undo_info;
    netsnmp_variable_list *var;
    netsnmp_request_group_item *current;
    int rc;

    DEBUGMSGTL ((AGENT, "saHpiInventoryTable_set_reserve1, called\n"));

    /*
     * TODO: loop through columns, check syntax and lengths. For
     * columns which have no dependencies, you could also move
     * the value/range checking here to attempt to catch error
     * cases as early as possible.
     */
    for( current = rg->list; current; current = current->next ) {

        var = current->ri->requestvb;
        rc = SNMP_ERR_NOERROR;

        switch(current->tri->colnum) {

        default: /** We shouldn't get here */
            rc = SNMP_ERR_GENERR;
            snmp_log(LOG_ERR, "unknown column in "
                     "saHpiInventoryTable_set_reserve1\n");
        }

        if (rc)
           netsnmp_set_mode_request_error(MODE_SET_BEGIN, current->ri, rc );
        rg->status = SNMP_MAX( rg->status, current->ri->status );
    }

    /*
     * done with all the columns. Could check row related
     * requirements here.
     */
}

void saHpiInventoryTable_set_reserve2( netsnmp_request_group *rg )
{
//    saHpiInventoryTable_context *row_ctx = (saHpiInventoryTable_context *)rg->existing_row;
//    saHpiInventoryTable_context *undo_ctx = (saHpiInventoryTable_context *)rg->undo_info;
    netsnmp_request_group_item *current;
    netsnmp_variable_list *var;
    int rc;

    DEBUGMSGTL ((AGENT, "saHpiInventoryTable_set_reserve2, called\n"));

    rg->rg_void = rg->list->ri;

    /*
     * TODO: loop through columns, check for valid
     * values and any range constraints.
     */
    for( current = rg->list; current; current = current->next ) {

        var = current->ri->requestvb;
        rc = SNMP_ERR_NOERROR;

        switch(current->tri->colnum) {

        default: /** We shouldn't get here */
            netsnmp_assert(0); /** why wasn't this caught in reserve1? */
        }

        if (rc)
           netsnmp_set_mode_request_error(MODE_SET_BEGIN, current->ri, rc);
    }

    /*
     * done with all the columns. Could check row related
     * requirements here.
     */
}

/************************************************************
 * Assuming that the RESERVE phases were successful, the next
 * stage is indicated by the action value ACTION. This is used
 * to actually implement the set operation. However, this must
 * either be done into temporary (persistent) storage, or the
 * previous value stored similarly, in case any of the subsequent
 * ACTION calls fail.
 *
 * In your case, changes should be made to row_ctx. A copy of
 * the original row is in undo_ctx.
 */
void saHpiInventoryTable_set_action( netsnmp_request_group *rg )
{
    netsnmp_variable_list *var;
//    saHpiInventoryTable_context *row_ctx = (saHpiInventoryTable_context *)rg->existing_row;
//    saHpiInventoryTable_context *undo_ctx = (saHpiInventoryTable_context *)rg->undo_info;
    netsnmp_request_group_item *current;

    int            row_err = 0;

    DEBUGMSGTL ((AGENT, "saHpiInventoryTable_set_action, called\n"));

    /*
     * TODO: loop through columns, copy varbind values
     * to context structure for the row.
     */
    for( current = rg->list; current; current = current->next ) {

        var = current->ri->requestvb;

        switch(current->tri->colnum) {

        default: /** We shouldn't get here */
            netsnmp_assert(0); /** why wasn't this caught in reserve1? */
        }
    }

    if(row_err) {
        netsnmp_set_mode_request_error(MODE_SET_BEGIN,
                                       (netsnmp_request_info*)rg->rg_void,
                                       row_err);
        return;
    }

    /*
     * TODO: if you have dependencies on other tables, this would be
     * a good place to check those, too.
     */
}

/************************************************************
 * Only once the ACTION phase has completed successfully, can
 * the final COMMIT phase be run. This is used to complete any
 * writes that were done into temporary storage, and then release
 * any allocated resources. Note that all the code in this phase
 * should be "safe" code that cannot possibly fail (cue
 * hysterical laughter). The whole intent of the ACTION/COMMIT
 * division is that all of the fallible code should be done in
 * the ACTION phase, so that it can be backed out if necessary.
 *
 * BEFORE calling this routine, the agent will update the
 * container (inserting a row if row_created == 1, or removing
 * the row if row_deleted == 1).
 *
 * AFTER calling this routine, the agent will delete the
 * undo_info.
 */
void saHpiInventoryTable_set_commit( netsnmp_request_group *rg )
{
    netsnmp_variable_list *var;
//    saHpiInventoryTable_context *row_ctx = (saHpiInventoryTable_context *)rg->existing_row;
//    saHpiInventoryTable_context *undo_ctx = (saHpiInventoryTable_context *)rg->undo_info;
    netsnmp_request_group_item *current;

    DEBUGMSGTL ((AGENT, "saHpiInventoryTable_set_commit, called\n"));

    /*
     * loop through columns
     */
    for( current = rg->list; current; current = current->next ) {

        var = current->ri->requestvb;

        switch(current->tri->colnum) {

        default: /** We shouldn't get here */
            netsnmp_assert(0); /** why wasn't this caught in reserve1? */
        }
    }

    /*
     * done with all the columns. Could check row related
     * requirements here.
     */
}

/************************************************************
 * If either of the RESERVE calls fail, the write routines
 * are called again with the FREE action, to release any resources
 * that have been allocated. The agent will then return a failure
 * response to the requesting application.
 *
 * AFTER calling this routine, the agent will delete undo_info.
 */
void saHpiInventoryTable_set_free( netsnmp_request_group *rg )
{
    netsnmp_variable_list *var;
//    saHpiInventoryTable_context *row_ctx = (saHpiInventoryTable_context *)rg->existing_row;
//    saHpiInventoryTable_context *undo_ctx = (saHpiInventoryTable_context *)rg->undo_info;
    netsnmp_request_group_item *current;

    DEBUGMSGTL ((AGENT, "saHpiInventoryTable_set_free, called\n"));

    /*
     * loop through columns
     */
    for( current = rg->list; current; current = current->next ) {

        var = current->ri->requestvb;

        switch(current->tri->colnum) {

        default: 
                break;
                /** We shouldn't get here */
                /** should have been logged in reserve1 */
        }
    }

    /*
     * done with all the columns. Could check row related
     * requirements here.
     */
}

/************************************************************
 * If the ACTION phase does fail (for example due to an apparently
 * valid, but unacceptable value, or an unforeseen problem), then
 * the list of write routines are called again, with the UNDO
 * action. This requires the routine to reset the value that was
 * changed to its previous value (assuming it was actually changed),
 * and then to release any resources that had been allocated. As
 * with the FREE phase, the agent will then return an indication
 * of the error to the requesting application.
 *
 * BEFORE calling this routine, the agent will update the container
 * (remove any newly inserted row, re-insert any removed row).
 *
 * AFTER calling this routing, the agent will call row_copy
 * to restore the data in existing_row from the date in undo_info.
 * Then undo_info will be deleted (or existing row, if row_created
 * == 1).
 */
void saHpiInventoryTable_set_undo( netsnmp_request_group *rg )
{
    netsnmp_variable_list *var;
//    saHpiInventoryTable_context *row_ctx = (saHpiInventoryTable_context *)rg->existing_row;
//    saHpiInventoryTable_context *undo_ctx = (saHpiInventoryTable_context *)rg->undo_info;
    netsnmp_request_group_item *current;

    DEBUGMSGTL ((AGENT, "saHpiInventoryTable_set_undo, called\n"));

    /*
     * loop through columns
     */
    for( current = rg->list; current; current = current->next ) {

        var = current->ri->requestvb;

        switch(current->tri->colnum) {

        default: /** We shouldn't get here */
            netsnmp_assert(0); /** why wasn't this caught in reserve1? */
        }
    }

    /*
     * done with all the columns. Could check row related
     * requirements here.
     */
}

/************************************************************
 *
 * Initialize the saHpiInventoryTable table by defining its contents and how it's structured
 */
void
initialize_table_saHpiInventoryTable(void)
{
    netsnmp_table_registration_info *table_info;

    DEBUGMSGTL ((AGENT, "initialize_table_saHpiInventoryTable, called\n"));

    if(my_handler) {
        snmp_log(LOG_ERR, "initialize_table_saHpiInventoryTable_handler called again\n");
        return;
    }

    memset(&cb, 0x00, sizeof(cb));

    /** create the table structure itself */
    table_info = SNMP_MALLOC_TYPEDEF(netsnmp_table_registration_info);

    /* if your table is read only, it's easiest to change the
       HANDLER_CAN_RWRITE definition below to HANDLER_CAN_RONLY */
    my_handler = netsnmp_create_handler_registration("saHpiInventoryTable",
                                             netsnmp_table_array_helper_handler,
                                             saHpiInventoryTable_oid,
                                             saHpiInventoryTable_oid_len,
                                             HANDLER_CAN_RWRITE);
            
    if (!my_handler || !table_info) {
        snmp_log(LOG_ERR, "malloc failed in "
                 "initialize_table_saHpiInventoryTable_handler\n");
        return; /** mallocs failed */
    }

    /***************************************************
     * Setting up the table's definition
     */
    /*
     * TODO: add any external indexes here.
     */
        /** TODO: add code for external index(s)! */

    /*
     * internal indexes
     */
        /** index: saHpiDomainId */
        netsnmp_table_helper_add_index(table_info, ASN_UNSIGNED);
        /** index: saHpiResourceId */
        netsnmp_table_helper_add_index(table_info, ASN_UNSIGNED);
        /** index: saHpiResourceIsHistorical */
        netsnmp_table_helper_add_index(table_info, ASN_INTEGER);
        /** index: saHpiInventoryId */
        netsnmp_table_helper_add_index(table_info, ASN_UNSIGNED);

    table_info->min_column = saHpiInventoryTable_COL_MIN;
    table_info->max_column = saHpiInventoryTable_COL_MAX;

    /***************************************************
     * registering the table with the master agent
     */
    cb.get_value = saHpiInventoryTable_get_value;
    cb.container = netsnmp_container_find("saHpiInventoryTable_primary:"
                                          "saHpiInventoryTable:"
                                          "table_container");

    netsnmp_container_add_index(cb.container,
                                netsnmp_container_find("saHpiInventoryTable_secondary:"
                                                       "saHpiInventoryTable:"
                                                       "table_container"));
    cb.container->next->compare = saHpiInventoryTable_cmp;


    cb.can_set = 1;

    cb.create_row = (UserRowMethod*)saHpiInventoryTable_create_row;

    cb.duplicate_row = (UserRowMethod*)saHpiInventoryTable_duplicate_row;
    cb.delete_row = (UserRowMethod*)saHpiInventoryTable_delete_row;
    cb.row_copy = (Netsnmp_User_Row_Operation *)saHpiInventoryTable_row_copy;

    cb.can_activate = (Netsnmp_User_Row_Action *)saHpiInventoryTable_can_activate;
    cb.can_deactivate = (Netsnmp_User_Row_Action *)saHpiInventoryTable_can_deactivate;
    cb.can_delete = (Netsnmp_User_Row_Action *)saHpiInventoryTable_can_delete;

    cb.set_reserve1 = saHpiInventoryTable_set_reserve1;
    cb.set_reserve2 = saHpiInventoryTable_set_reserve2;
    cb.set_action = saHpiInventoryTable_set_action;
    cb.set_commit = saHpiInventoryTable_set_commit;
    cb.set_free = saHpiInventoryTable_set_free;
    cb.set_undo = saHpiInventoryTable_set_undo;

    DEBUGMSGTL(("initialize_table_saHpiInventoryTable",
                "Registering table saHpiInventoryTable "
                "as a table array\n"));
    netsnmp_table_container_register(my_handler, table_info, &cb,
                                     cb.container, 1);
}

/************************************************************
 * saHpiInventoryTable_get_value
 *
 * This routine is called for get requests to copy the data
 * from the context to the varbind for the request. If the
 * context has been properly maintained, you don't need to
 * change in code in this fuction.
 */
int saHpiInventoryTable_get_value(
            netsnmp_request_info *request,
            netsnmp_index *item,
            netsnmp_table_request_info *table_info )
{
    netsnmp_variable_list *var = request->requestvb;
    saHpiInventoryTable_context *context = (saHpiInventoryTable_context *)item;

    DEBUGMSGTL ((AGENT, "saHpiInventoryTable_get_value, called\n"));

    switch(table_info->colnum) {

        case COLUMN_SAHPIINVENTORYID:
            /** SaHpiInstrumentId = ASN_UNSIGNED */
            snmp_set_var_typed_value(var, ASN_UNSIGNED,
                         (char*)&context->saHpiInventoryId,
                         sizeof(context->saHpiInventoryId) );
        break;
    
        case COLUMN_SAHPIINVENTORYPERSISTENT:
            /** TruthValue = ASN_INTEGER */
            snmp_set_var_typed_value(var, ASN_INTEGER,
                         (char*)&context->saHpiInventoryPersistent,
                         sizeof(context->saHpiInventoryPersistent) );
        break;
    
        case COLUMN_SAHPIINVENTORYOEM:
            /** UNSIGNED32 = ASN_UNSIGNED */
            snmp_set_var_typed_value(var, ASN_UNSIGNED,
                         (char*)&context->saHpiInventoryOEM,
                         sizeof(context->saHpiInventoryOEM) );
        break;
    
        case COLUMN_SAHPIINVENTORYUPDATECOUNT:
            /** UNSIGNED32 = ASN_UNSIGNED */
            snmp_set_var_typed_value(var, ASN_UNSIGNED,
                         (char*)&context->saHpiInventoryUpdateCount,
                         sizeof(context->saHpiInventoryUpdateCount) );
        break;
    
        case COLUMN_SAHPIINVENTORYISREADONLY:
            /** TruthValue = ASN_INTEGER */
            snmp_set_var_typed_value(var, ASN_INTEGER,
                         (char*)&context->saHpiInventoryIsReadOnly,
                         sizeof(context->saHpiInventoryIsReadOnly) );
        break;
    
        case COLUMN_SAHPIINVENTORYNUMAREAS:
            /** UNSIGNED32 = ASN_UNSIGNED */
            snmp_set_var_typed_value(var, ASN_UNSIGNED,
                         (char*)&context->saHpiInventoryNumAreas,
                         sizeof(context->saHpiInventoryNumAreas) );
        break;
    
        case COLUMN_SAHPIINVENTORYRDR:
            /** RowPointer = ASN_OBJECT_ID */
            snmp_set_var_typed_value(var, ASN_OBJECT_ID,
                         (char*)&context->saHpiInventoryRDR,
                         context->saHpiInventoryRDR_len );
        break;
    
    default: /** We shouldn't get here */
        snmp_log(LOG_ERR, "unknown column in "
                 "saHpiInventoryTable_get_value\n");
        return SNMP_ERR_GENERR;
    }
    return SNMP_ERR_NOERROR;
}

/************************************************************
 * saHpiInventoryTable_get_by_idx
 */
const saHpiInventoryTable_context *
saHpiInventoryTable_get_by_idx(netsnmp_index * hdr)
{
        DEBUGMSGTL ((AGENT, "saHpiInventoryTable_get_by_idx, called\n"));

    return (const saHpiInventoryTable_context *)
        CONTAINER_FIND(cb.container, hdr );
}


