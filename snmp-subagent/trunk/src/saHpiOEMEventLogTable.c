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

#include "saHpiOEMEventLogTable.h"

static     netsnmp_handler_registration *my_handler = NULL;
static     netsnmp_table_array_callbacks cb;

oid saHpiOEMEventLogTable_oid[] = { saHpiOEMEventLogTable_TABLE_OID };
size_t saHpiOEMEventLogTable_oid_len = OID_LENGTH(saHpiOEMEventLogTable_oid);


/************************************************************/
/************************************************************/
/************************************************************/
/************************************************************/

/*************************************************************
 * oid and fucntion declarations scalars
 */
static u_long oem_event_log_entry_count_total = 0;
static u_long oem_event_log_entry_count = 0;

static oid saHpiOEMEventLogEntryCountTotal_oid[] = { 1,3,6,1,4,1,18568,2,1,1,3,2,24 };
static oid saHpiOEMEventLogEntryCount_oid[] = { 1,3,6,1,4,1,18568,2,1,1,3,2,25 };

int handle_saHpiOEMEventLogEntryCountTotal(netsnmp_mib_handler *handler,
                                        netsnmp_handler_registration *reginfo,
                                        netsnmp_agent_request_info   *reqinfo,
                                        netsnmp_request_info         *requests);
			  
int handle_saHpiOEMEventLogEntryCount(netsnmp_mib_handler *handler,
                                        netsnmp_handler_registration *reginfo,
                                        netsnmp_agent_request_info   *reqinfo,
                                        netsnmp_request_info         *requests);
					
int intialize_table_saHpiOEMEventLogEntryCountTotal(void);
int intialize_table_saHpiOEMEventLogEntryCount(void);

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
handle_saHpiOEMEventLogEntryCountTotal(netsnmp_mib_handler *handler,
                                        netsnmp_handler_registration *reginfo,
                                        netsnmp_agent_request_info   *reqinfo,
                                        netsnmp_request_info         *requests)
{
        /* We are never called for a GETNEXT if it's registered as a
           "instance", as it's "magically" handled for us.  */
        /* a instance handler also only hands us one request at a time, so
           we don't need to loop over a list of requests; we'll only get one. */
        
        DEBUGMSGTL ((AGENT, "handle_saHpiOEMEventLogEntryCountTotal, called\n"));

        switch(reqinfo->mode) {

        case MODE_GET:
                snmp_set_var_typed_value(requests->requestvb, ASN_COUNTER,
        			        (u_char *) &oem_event_log_entry_count_total,
        			        sizeof(oem_event_log_entry_count_total));
            break;


        default:
                /* we should never get here, so this is a really bad error */
                return SNMP_ERR_GENERR;
        }

        return SNMP_ERR_NOERROR;
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
int
handle_saHpiOEMEventLogEntryCount(netsnmp_mib_handler *handler,
                                netsnmp_handler_registration *reginfo,
                                netsnmp_agent_request_info   *reqinfo,
                                netsnmp_request_info         *requests)
{
        /* We are never called for a GETNEXT if it's registered as a
           "instance", as it's "magically" handled for us.  */
        /* a instance handler also only hands us one request at a time, so
           we don't need to loop over a list of requests; we'll only get one. */


        DEBUGMSGTL ((AGENT, "handle_saHpiOEMEventLogEntryCount, called\n"));

        oem_event_log_entry_count = CONTAINER_SIZE (cb.container);
        
        switch(reqinfo->mode) {

        case MODE_GET:
                snmp_set_var_typed_value(requests->requestvb, ASN_COUNTER,
        			        (u_char *)&oem_event_log_entry_count,
        			        sizeof(oem_event_log_entry_count));
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
int intialize_table_saHpiOEMEventLogEntryCountTotal(void)
{

        DEBUGMSGTL ((AGENT, "intialize_table_saHpiOEMEventLogEntryCountTotal, called\n"));

        netsnmp_register_scalar(
                                netsnmp_create_handler_registration(
				        "saHpiOEMEventLogEntryCountTotal", 
				         handle_saHpiOEMEventLogEntryCountTotal,
                                         saHpiOEMEventLogEntryCountTotal_oid, 
					 OID_LENGTH(saHpiOEMEventLogEntryCountTotal_oid),
                                         HANDLER_CAN_RONLY ));

        return SNMP_ERR_NOERROR;
} 

/**
 * 
 * @return: 
 */ 
int intialize_table_saHpiOEMEventLogEntryCount(void)
{

        DEBUGMSGTL ((AGENT, "intialize_table_saHpiOEMEventLogEntryCount, called\n"));

        netsnmp_register_scalar(
                                netsnmp_create_handler_registration(
                                        "saHpiOEMEventLogEntryCount", 
                                         handle_saHpiOEMEventLogEntryCount,
                                         saHpiOEMEventLogEntryCount_oid, 
					 OID_LENGTH(saHpiOEMEventLogEntryCount_oid),
                                         HANDLER_CAN_RONLY ));

        return SNMP_ERR_NOERROR;
}
 
/************************************************************/
/************************************************************/
/************************************************************/
/************************************************************/


#ifdef saHpiOEMEventLogTable_IDX2
/************************************************************
 * keep binary tree to find context by name
 */
static int saHpiOEMEventLogTable_cmp( const void *lhs, const void *rhs );

/************************************************************
 * compare two context pointers here. Return -1 if lhs < rhs,
 * 0 if lhs == rhs, and 1 if lhs > rhs.
 */
static int
saHpiOEMEventLogTable_cmp( const void *lhs, const void *rhs )
{
        saHpiOEMEventLogTable_context *context_l =
        (saHpiOEMEventLogTable_context *)lhs;
        saHpiOEMEventLogTable_context *context_r =
        (saHpiOEMEventLogTable_context *)rhs;

        /*
         * check primary key, then secondary. Add your own code if
         * there are more than 2 indexes
         */
        DEBUGMSGTL ((AGENT, "saHpiOEMEventLogTable_cmp, called\n"));

        /* check for NULL pointers */
        if (lhs == NULL || rhs == NULL ) {
                DEBUGMSGTL((AGENT,"saHpiOEMEventLogTable_cmp() NULL pointer ERROR\n" ));
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
                        /* CHECK THIRD INDEX,  saHpiEventSeverity */
                        if ( context_l->index.oids[2] < context_r->index.oids[2])
                                return -1;

                        if ( context_l->index.oids[2] > context_r->index.oids[2])
                                return 1;

                        if ( context_l->index.oids[2] == context_r->index.oids[2]) {
                                /* If saHpiEventSeverity index is equal sort by forth index */
                                /* CHECK FORTH INDEX,  saHpiOEMEventEntryId */
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
 * search tree
 */
/** TODO: set additional indexes as parameters */
saHpiOEMEventLogTable_context *
saHpiOEMEventLogTable_get( const char *name, int len )
{
    saHpiOEMEventLogTable_context tmp;

    /** we should have a secondary index */
    netsnmp_assert(cb.container->next != NULL);
    
    /*
     * TODO: implement compare. Remove this ifdef code and
     * add your own code here.
     */
#ifdef TABLE_CONTAINER_TODO
    snmp_log(LOG_ERR, "saHpiOEMEventLogTable_get not implemented!\n" );
    return NULL;
#endif

    /*
     * EXAMPLE:
     *
     * if(len > sizeof(tmp.xxName))
     *   return NULL;
     *
     * strncpy( tmp.xxName, name, sizeof(tmp.xxName) );
     * tmp.xxName_len = len;
     *
     * return CONTAINER_FIND(cb.container->next, &tmp);
     */
}
#endif


/************************************************************
 * Initializes the saHpiOEMEventLogTable module
 */
void
init_saHpiOEMEventLogTable(void)
{
        DEBUGMSGTL ((AGENT, "init_saHpiOEMEventLogTable, called\n"));
	
	initialize_table_saHpiOEMEventLogTable();

        intialize_table_saHpiOEMEventLogEntryCountTotal();
        intialize_table_saHpiOEMEventLogEntryCount();        
}

/************************************************************
 * the *_row_copy routine
 */
static int saHpiOEMEventLogTable_row_copy(saHpiOEMEventLogTable_context * dst,
                         saHpiOEMEventLogTable_context * src)
{
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
    dst->saHpiOEMEventLogTimestamp = src->saHpiOEMEventLogTimestamp;

    dst->saHpiOEMEventLogManufacturerIdT = src->saHpiOEMEventLogManufacturerIdT;

    dst->saHpiOEMEventLogTextType = src->saHpiOEMEventLogTextType;

    dst->saHpiOEMEventLogTextLanguage = src->saHpiOEMEventLogTextLanguage;

    memcpy( dst->saHpiOEMEventLogText, src->saHpiOEMEventLogText, src->saHpiOEMEventLogText_len );
    dst->saHpiOEMEventLogText_len = src->saHpiOEMEventLogText_len;

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
saHpiOEMEventLogTable_extract_index( saHpiOEMEventLogTable_context * ctx, netsnmp_index * hdr )
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
        netsnmp_variable_list var_saHpiEventSeverity;
        netsnmp_variable_list var_saHpiOEMEventEntryId;
        int err;

        DEBUGMSGTL ((AGENT, "saHpiOEMEventLogTable_extract_index, called\n"));

        /*
         * copy index, if provided
         */
        if (hdr) {
                netsnmp_assert(ctx->index.oids == NULL);
                if (snmp_clone_mem( (void*)&ctx->index.oids, hdr->oids,
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
       var_saHpiResourceId.next_variable = &var_saHpiEventSeverity;

       memset( &var_saHpiEventSeverity, 0x00, sizeof(var_saHpiEventSeverity) );
       var_saHpiEventSeverity.type = ASN_INTEGER; /* type hint for parse_oid_indexes */
       /** TODO: link this index to the next, or NULL for the last one */
       var_saHpiEventSeverity.next_variable = &var_saHpiOEMEventEntryId;

       memset( &var_saHpiOEMEventEntryId, 0x00, sizeof(var_saHpiOEMEventEntryId) );
       var_saHpiOEMEventEntryId.type = ASN_UNSIGNED; /* type hint for parse_oid_indexes */
       /** TODO: link this index to the next, or NULL for the last one */
       var_saHpiOEMEventEntryId.next_variable = NULL;


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
   
                /** skipping external index saHpiEventSeverity */
   
                /** skipping external index saHpiOEMEventEntryId */
   
                err = saHpiDomainId_check_index(*var_saHpiDomainId.val.integer);
                err = saHpiResourceEntryId_check_index(*var_saHpiResourceId.val.integer);
		err = saHpiEventSeverity_check_index(*var_saHpiEventSeverity.val.integer);
		err = saHpiOEMEventEntryId_check_index(*var_saHpiOEMEventEntryId.val.integer);  
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
int saHpiOEMEventLogTable_can_activate(saHpiOEMEventLogTable_context *undo_ctx,
                      saHpiOEMEventLogTable_context *row_ctx,
                      netsnmp_request_group * rg)
{
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
int saHpiOEMEventLogTable_can_deactivate(saHpiOEMEventLogTable_context *undo_ctx,
                        saHpiOEMEventLogTable_context *row_ctx,
                        netsnmp_request_group * rg)
{
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
int saHpiOEMEventLogTable_can_delete(saHpiOEMEventLogTable_context *undo_ctx,
                    saHpiOEMEventLogTable_context *row_ctx,
                    netsnmp_request_group * rg)
{
    /*
     * probably shouldn't delete a row that we can't
     * deactivate.
     */
    if(saHpiOEMEventLogTable_can_deactivate(undo_ctx,row_ctx,rg) != 1)
        return 0;
    
    /*
     * TODO: check for other deletion requirements here
     */
    return 1;
}

#ifdef saHpiOEMEventLogTable_ROW_CREATION
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
 * returns a newly allocated saHpiOEMEventLogTable_context
 *   structure if the specified indexes are not illegal
 * returns NULL for errors or illegal index values.
 */
saHpiOEMEventLogTable_context *
saHpiOEMEventLogTable_create_row( netsnmp_index* hdr)
{
    saHpiOEMEventLogTable_context * ctx =
        SNMP_MALLOC_TYPEDEF(saHpiOEMEventLogTable_context);
    if(!ctx)
        return NULL;

    oem_event_log_entry_count_total++;
        
    /*
     * TODO: check indexes, if necessary.
     */
    if(saHpiOEMEventLogTable_extract_index( ctx, hdr )) {
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
#endif

/************************************************************
 * the *_duplicate row routine
 */
saHpiOEMEventLogTable_context *
saHpiOEMEventLogTable_duplicate_row( saHpiOEMEventLogTable_context * row_ctx)
{
    saHpiOEMEventLogTable_context * dup;

    if(!row_ctx)
        return NULL;

    dup = SNMP_MALLOC_TYPEDEF(saHpiOEMEventLogTable_context);
    if(!dup)
        return NULL;
        
    if(saHpiOEMEventLogTable_row_copy(dup,row_ctx)) {
        free(dup);
        dup = NULL;
    }

    return dup;
}

/************************************************************
 * the *_delete_row method is called to delete a row.
 */
netsnmp_index * saHpiOEMEventLogTable_delete_row( saHpiOEMEventLogTable_context * ctx )
{
  /* netsnmp_mutex_destroy(ctx->lock); */

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
void saHpiOEMEventLogTable_set_reserve1( netsnmp_request_group *rg )
{
    saHpiOEMEventLogTable_context *row_ctx =
            (saHpiOEMEventLogTable_context *)rg->existing_row;
    saHpiOEMEventLogTable_context *undo_ctx =
            (saHpiOEMEventLogTable_context *)rg->undo_info;
    netsnmp_variable_list *var;
    netsnmp_request_group_item *current;
    int rc;


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
                     "saHpiOEMEventLogTable_set_reserve1\n");
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

void saHpiOEMEventLogTable_set_reserve2( netsnmp_request_group *rg )
{
    saHpiOEMEventLogTable_context *row_ctx = (saHpiOEMEventLogTable_context *)rg->existing_row;
    saHpiOEMEventLogTable_context *undo_ctx = (saHpiOEMEventLogTable_context *)rg->undo_info;
    netsnmp_request_group_item *current;
    netsnmp_variable_list *var;
    int rc;

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
void saHpiOEMEventLogTable_set_action( netsnmp_request_group *rg )
{
    netsnmp_variable_list *var;
    saHpiOEMEventLogTable_context *row_ctx = (saHpiOEMEventLogTable_context *)rg->existing_row;
    saHpiOEMEventLogTable_context *undo_ctx = (saHpiOEMEventLogTable_context *)rg->undo_info;
    netsnmp_request_group_item *current;

    int            row_err = 0;

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

    /*
     * done with all the columns. Could check row related
     * requirements here.
     */
#ifndef saHpiOEMEventLogTable_CAN_MODIFY_ACTIVE_ROW
    if( undo_ctx && RS_IS_ACTIVE(undo_ctx->saHpiDomainAlarmRowStatus) &&
        row_ctx && RS_IS_ACTIVE(row_ctx->saHpiDomainAlarmRowStatus) ) {
            row_err = 1;
    }
#endif

    /*
     * check activation/deactivation
     */
    row_err = netsnmp_table_array_check_row_status(&cb, rg,
                                  row_ctx ? &row_ctx->saHpiDomainAlarmRowStatus : NULL,
                                  undo_ctx ? &undo_ctx->saHpiDomainAlarmRowStatus : NULL);
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
void saHpiOEMEventLogTable_set_commit( netsnmp_request_group *rg )
{
    netsnmp_variable_list *var;
    saHpiOEMEventLogTable_context *row_ctx = (saHpiOEMEventLogTable_context *)rg->existing_row;
    saHpiOEMEventLogTable_context *undo_ctx = (saHpiOEMEventLogTable_context *)rg->undo_info;
    netsnmp_request_group_item *current;

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
void saHpiOEMEventLogTable_set_free( netsnmp_request_group *rg )
{
    netsnmp_variable_list *var;
    saHpiOEMEventLogTable_context *row_ctx = (saHpiOEMEventLogTable_context *)rg->existing_row;
    saHpiOEMEventLogTable_context *undo_ctx = (saHpiOEMEventLogTable_context *)rg->undo_info;
    netsnmp_request_group_item *current;

    /*
     * loop through columns
     */
    for( current = rg->list; current; current = current->next ) {

        var = current->ri->requestvb;

        switch(current->tri->colnum) {

        default: /** We shouldn't get here */
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
void saHpiOEMEventLogTable_set_undo( netsnmp_request_group *rg )
{
    netsnmp_variable_list *var;
    saHpiOEMEventLogTable_context *row_ctx = (saHpiOEMEventLogTable_context *)rg->existing_row;
    saHpiOEMEventLogTable_context *undo_ctx = (saHpiOEMEventLogTable_context *)rg->undo_info;
    netsnmp_request_group_item *current;

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

#endif /** saHpiOEMEventLogTable_SET_HANDLING */


/************************************************************
 *
 * Initialize the saHpiOEMEventLogTable table by defining its contents and how it's structured
 */
void
initialize_table_saHpiOEMEventLogTable(void)
{
    netsnmp_table_registration_info *table_info;

    if(my_handler) {
        snmp_log(LOG_ERR, "initialize_table_saHpiOEMEventLogTable_handler called again\n");
        return;
    }

    memset(&cb, 0x00, sizeof(cb));

    /** create the table structure itself */
    table_info = SNMP_MALLOC_TYPEDEF(netsnmp_table_registration_info);

    /* if your table is read only, it's easiest to change the
       HANDLER_CAN_RWRITE definition below to HANDLER_CAN_RONLY */
    my_handler = netsnmp_create_handler_registration("saHpiOEMEventLogTable",
                                             netsnmp_table_array_helper_handler,
                                             saHpiOEMEventLogTable_oid,
                                             saHpiOEMEventLogTable_oid_len,
                                             HANDLER_CAN_RWRITE);
            
    if (!my_handler || !table_info) {
        snmp_log(LOG_ERR, "malloc failed in "
                 "initialize_table_saHpiOEMEventLogTable_handler\n");
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
        /** index: saHpiEventSeverity */
        netsnmp_table_helper_add_index(table_info, ASN_INTEGER);
        /** index: saHpiOEMEventEntryId */
        netsnmp_table_helper_add_index(table_info, ASN_UNSIGNED);

    table_info->min_column = saHpiOEMEventLogTable_COL_MIN;
    table_info->max_column = saHpiOEMEventLogTable_COL_MAX;

    /***************************************************
     * registering the table with the master agent
     */
    cb.get_value = saHpiOEMEventLogTable_get_value;
    cb.container = netsnmp_container_find("saHpiOEMEventLogTable_primary:"
                                          "saHpiOEMEventLogTable:"
                                          "table_container");
#ifdef saHpiOEMEventLogTable_IDX2
    netsnmp_container_add_index(cb.container,
                                netsnmp_container_find("saHpiOEMEventLogTable_secondary:"
                                                       "saHpiOEMEventLogTable:"
                                                       "table_container"));
    cb.container->next->compare = saHpiOEMEventLogTable_cmp;
#endif
#ifdef saHpiOEMEventLogTable_SET_HANDLING
    cb.can_set = 1;
#ifdef saHpiOEMEventLogTable_ROW_CREATION
    cb.create_row = (UserRowMethod*)saHpiOEMEventLogTable_create_row;
#endif
    cb.duplicate_row = (UserRowMethod*)saHpiOEMEventLogTable_duplicate_row;
    cb.delete_row = (UserRowMethod*)saHpiOEMEventLogTable_delete_row;
    cb.row_copy = (Netsnmp_User_Row_Operation *)saHpiOEMEventLogTable_row_copy;

    cb.can_activate = (Netsnmp_User_Row_Action *)saHpiOEMEventLogTable_can_activate;
    cb.can_deactivate = (Netsnmp_User_Row_Action *)saHpiOEMEventLogTable_can_deactivate;
    cb.can_delete = (Netsnmp_User_Row_Action *)saHpiOEMEventLogTable_can_delete;

    cb.set_reserve1 = saHpiOEMEventLogTable_set_reserve1;
    cb.set_reserve2 = saHpiOEMEventLogTable_set_reserve2;
    cb.set_action = saHpiOEMEventLogTable_set_action;
    cb.set_commit = saHpiOEMEventLogTable_set_commit;
    cb.set_free = saHpiOEMEventLogTable_set_free;
    cb.set_undo = saHpiOEMEventLogTable_set_undo;
#endif
    DEBUGMSGTL(("initialize_table_saHpiOEMEventLogTable",
                "Registering table saHpiOEMEventLogTable "
                "as a table array\n"));
    netsnmp_table_container_register(my_handler, table_info, &cb,
                                     cb.container, 1);
}

/************************************************************
 * saHpiOEMEventLogTable_get_value
 *
 * This routine is called for get requests to copy the data
 * from the context to the varbind for the request. If the
 * context has been properly maintained, you don't need to
 * change in code in this fuction.
 */
int saHpiOEMEventLogTable_get_value(
            netsnmp_request_info *request,
            netsnmp_index *item,
            netsnmp_table_request_info *table_info )
{
    netsnmp_variable_list *var = request->requestvb;
    saHpiOEMEventLogTable_context *context = (saHpiOEMEventLogTable_context *)item;

    switch(table_info->colnum) {

        case COLUMN_SAHPIOEMEVENTLOGTIMESTAMP:
            /** SaHpiTime = ASN_COUNTER64 */
            snmp_set_var_typed_value(var, ASN_COUNTER64,
                         (char*)&context->saHpiOEMEventLogTimestamp,
                         sizeof(context->saHpiOEMEventLogTimestamp) );
        break;
    
        case COLUMN_SAHPIOEMEVENTLOGMANUFACTURERIDT:
            /** SaHpiManufacturerId = ASN_UNSIGNED */
            snmp_set_var_typed_value(var, ASN_UNSIGNED,
                         (char*)&context->saHpiOEMEventLogManufacturerIdT,
                         sizeof(context->saHpiOEMEventLogManufacturerIdT) );
        break;
    
        case COLUMN_SAHPIOEMEVENTLOGTEXTTYPE:
            /** SaHpiTextType = ASN_INTEGER */
            snmp_set_var_typed_value(var, ASN_INTEGER,
                         (char*)&context->saHpiOEMEventLogTextType,
                         sizeof(context->saHpiOEMEventLogTextType) );
        break;
    
        case COLUMN_SAHPIOEMEVENTLOGTEXTLANGUAGE:
            /** SaHpiTextLanguage = ASN_INTEGER */
            snmp_set_var_typed_value(var, ASN_INTEGER,
                         (char*)&context->saHpiOEMEventLogTextLanguage,
                         sizeof(context->saHpiOEMEventLogTextLanguage) );
        break;
    
        case COLUMN_SAHPIOEMEVENTLOGTEXT:
            /** SaHpiText = ASN_OCTET_STR */
            snmp_set_var_typed_value(var, ASN_OCTET_STR,
                         (char*)&context->saHpiOEMEventLogText,
                         context->saHpiOEMEventLogText_len );
        break;
    
    default: /** We shouldn't get here */
        snmp_log(LOG_ERR, "unknown column in "
                 "saHpiOEMEventLogTable_get_value\n");
        return SNMP_ERR_GENERR;
    }
    return SNMP_ERR_NOERROR;
}

/************************************************************
 * saHpiOEMEventLogTable_get_by_idx
 */
const saHpiOEMEventLogTable_context *
saHpiOEMEventLogTable_get_by_idx(netsnmp_index * hdr)
{
    return (const saHpiOEMEventLogTable_context *)
        CONTAINER_FIND(cb.container, hdr );
}


