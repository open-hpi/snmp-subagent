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
 *       : mib2c.array-user.conf,v 5.15.2.1 2003/02/27 05:59:41 rstory Exp $
 *
 * $Id$
 *
 */
#include <net-snmp/net-snmp-config.h>
#include <net-snmp/net-snmp-includes.h>
#include <net-snmp/agent/net-snmp-agent-includes.h>
#include <net-snmp/library/snmp_assert.h>

#include <SaHpi.h>

#include <hpiSubagent.h>
#include <saHpiTable.h>
#include <saHpiRdrTable.h>

#include <saHpiSensorTable.h>
#include <saHpiCtrlTable.h>
#include <saHpiInventoryTable.h>
#include <saHpiWatchdogTable.h>

extern int send_traps;


static netsnmp_handler_registration *my_handler = NULL;
static netsnmp_table_array_callbacks cb;

static oid saHpiRdrTable_oid[] = { saHpiRdrTable_TABLE_OID };
static size_t saHpiRdrTable_oid_len = OID_LENGTH (saHpiRdrTable_oid);
static oid saHpiRdrCount_oid[] =
  { hpiResources_OID, SCALAR_COLUMN_SAHPIRDRCOUNT, 0 };



static oid saHpiResourceDataRecordNotification_oid[] =
  { hpiNotifications_OID, 7, 0 };

#define RDR_NOTIF_COUNT 5
#define RDR_NOTIF_RDRRECORDID 0
#define RDR_NOTIF_TYPE 1
#define RDR_NOTIF_ENTITY_PATH 2
#define RDR_NOTIF_RDR 3
#define RDR_NOTIF_RDR_RTP 4

static trap_vars saHpiResourceDataRecordNotification[] = {
  {COLUMN_SAHPIRDRRECORDID, ASN_UNSIGNED, NULL, 0},
  {COLUMN_SAHPIRDRTYPE, ASN_INTEGER, NULL, 0},
  {COLUMN_SAHPIRDRENTITYPATH, ASN_OCTET_STR, NULL, 0},
  {COLUMN_SAHPIRDR, ASN_OBJECT_ID, NULL, 0},
  {COLUMN_SAHPIRDRRTP, ASN_OBJECT_ID, NULL, 0}
};


static u_long rdr_count = 0;

static unsigned int rdr_mutex = AGENT_FALSE;

static int
saHpiRdrTable_modify_context (SaHpiRptEntryT * rpt_entry,
			      SaHpiRdrT * entry,
			      saHpiRdrTable_context * ctx,
			      oid * rdr_oid, size_t oid_len,
			      oid * child_oid, size_t child_oid_len,
			      unsigned long child_id,
			      trap_vars ** var,
			      size_t * var_len, oid ** var_oid);


/*
 * Populates up to FIVE different rows:
 *  - RDR
 *  - Sensor
 *  - Inventory
 *  - Control
 *  - Watchdog
 */
int
populate_rdr (SaHpiRptEntryT * rpt_entry,
	      oid * rpt_oid, size_t rpt_oid_len,
	      oid * resource_oid, size_t resource_oid_len)
{

  SaErrorT err;
  SaHpiEntryIdT current_rdr;
  SaHpiEntryIdT next_rdr;
  SaHpiRdrT rdr_entry;
  SaHpiSessionIdT session_id;

  long backup_count = rdr_count;
  int rc = AGENT_ERR_NOERROR;

  oid rdr_oid[RDR_INDEX_NR];
  oid full_oid[MAX_OID_LEN];
  oid child_oid[MAX_OID_LEN];
  size_t child_oid_len;
  oid column[2];
  int column_len = 2;
  int full_oid_len;

  unsigned long child_id = 0xDEADBEEF;

  netsnmp_index rdr_index;
  saHpiRdrTable_context *rdr_context;



  oid *trap_oid;
  trap_vars *trap = NULL;
  size_t trap_len;
  netsnmp_variable_list *trap_var;

  DEBUGMSGTL ((AGENT, "\n\t--- populate_rdr: Entry.\n"));
  if ((rc = getSaHpiSession (&session_id)) == AGENT_ERR_NOERROR)
    {
      rdr_index.len = RDR_INDEX_NR;
      next_rdr = SAHPI_FIRST_ENTRY;
      do
	{
	  current_rdr = next_rdr;
	  memset (&rdr_entry, 0x00, sizeof (SaHpiRdrT));

	  err = saHpiRdrGet (session_id, rpt_entry->ResourceId,
			     current_rdr, &next_rdr, &rdr_entry);

	  if (err == SA_OK)
	    {
	      rdr_mutex = AGENT_TRUE;
	      // Look at the MIB to find out what the indexs are
	      rdr_oid[0] = rpt_entry->DomainId;
	      rdr_oid[1] = rpt_entry->ResourceId;
	      rdr_oid[2] = rdr_entry.RecordId;
	      rdr_oid[3] = rdr_entry.RdrType;

	      rdr_index.oids = (oid *) & rdr_oid;

	      if (backup_count == 0)
		{
		  rdr_context = saHpiRdrTable_create_row (&rdr_index);
		}
	      else
		{
		  // See if it exists.
		  rdr_context = NULL;
		  rdr_context = CONTAINER_FIND (cb.container, &rdr_index);
		  // If we don't find it - we create it.

		  if (!rdr_context)
		    {
		      // New entry. Add it
		      rdr_context = saHpiRdrTable_create_row (&rdr_index);
		    }

		  if (!rdr_context)
		    {
		      snmp_log (LOG_ERR, "Not enough memory for a RDR row!");
		      return AGENT_ERR_INTERNAL_ERROR;
		    }
		}

	      column[0] = 1;
	      column[1] = COLUMN_SAHPIRDRRESOURCEID;
	      build_full_oid (saHpiRdrTable_oid, saHpiRdrTable_oid_len,
			      column, column_len,
			      &rdr_index,
			      full_oid, MAX_OID_LEN, &full_oid_len);

	      child_oid_len = 0;
	      if (rdr_entry.RdrType == SAHPI_SENSOR_RDR)
		{
		  child_id = rdr_entry.RdrTypeUnion.SensorRec.Num;
		  rc = populate_sensor (&rdr_entry.RdrTypeUnion.SensorRec,
					rpt_entry,
					full_oid, full_oid_len,
					child_oid, &child_oid_len);
		  DEBUGMSGTL ((AGENT,
			       "Called populate_sensor(); ID: %d; rc: %d\n",
			       child_id, rc));
		}
	      if (rdr_entry.RdrType == SAHPI_CTRL_RDR)
		{
		  child_id = rdr_entry.RdrTypeUnion.CtrlRec.Num;
		  rc = populate_control (&rdr_entry.RdrTypeUnion.CtrlRec,
					 rpt_entry,
					 full_oid, full_oid_len,
					 child_oid, &child_oid_len);
		  DEBUGMSGTL ((AGENT,
			       "Called populate_control(); ID: %d; rc: %d\n",
			       child_id, rc));
		}
	      if (rdr_entry.RdrType == SAHPI_INVENTORY_RDR)
		{
		  child_id = rdr_entry.RdrTypeUnion.InventoryRec.EirId;
		  rc =
		    populate_inventory (&rdr_entry.RdrTypeUnion.InventoryRec,
					rpt_entry, full_oid, full_oid_len,
					child_oid, &child_oid_len);
		  DEBUGMSGTL ((AGENT,
			       "Called populate_inventory(); ID: %d; rc: %d\n",
			       child_id, rc));
		}
	      if (rdr_entry.RdrType == SAHPI_WATCHDOG_RDR)
		{
		  child_id = rdr_entry.RdrTypeUnion.WatchdogRec.WatchdogNum;
		  rc = populate_watchdog (&rdr_entry.RdrTypeUnion.WatchdogRec,
					  rpt_entry,
					  full_oid, full_oid_len,
					  child_oid, &child_oid_len);
		  DEBUGMSGTL ((AGENT,
			       "Called populate_watchdog(); ID: %d; rc: %d\n",
			       child_id, rc));
		}


	      // Mark this ctx as touched, or cleaned
	      rdr_context->dirty_bit = AGENT_FALSE;
	      // By this stage, rdr_context surely has something in it.
	      // '*_modify_context' does a checksum check to see if 
	      // the record needs to be altered, and if so populates with
	      // information from RDR and the OIDs passed.


	      if (saHpiRdrTable_modify_context (rpt_entry,
						&rdr_entry,
						rdr_context,
						rpt_oid,
						rpt_oid_len,
						child_oid,
						child_oid_len,
						child_id,
						&trap, &trap_len, &trap_oid)
		  == AGENT_NEW_ENTRY)
		{

		  CONTAINER_INSERT (cb.container, rdr_context);
		  rdr_count = CONTAINER_SIZE (cb.container);

		  if (send_traps == AGENT_TRUE)
		    {
		      if (trap != NULL)
			{
			  trap_var = build_notification (&rdr_index,
							 trap, trap_len,
							 saHpiResourceDataRecordNotification_oid,
							 OID_LENGTH
							 (saHpiResourceDataRecordNotification_oid),
							 saHpiRdrTable_oid,
							 saHpiRdrTable_oid_len,
							 rpt_entry->DomainId,
							 rpt_oid, rpt_oid_len,
							 rpt_entry->
							 ResourceId,
							 resource_oid,
							 resource_oid_len);
			  if (trap_var != NULL)
			    {
			      // Add some more (entryCount, and entryUpdate)
			      snmp_varlist_add_variable (&trap_var,
							 saHpiRdrCount_oid,
							 OID_LENGTH
							 (saHpiRdrCount_oid),
							 ASN_COUNTER,
							 (u_char *) &
							 rdr_count,
							 sizeof (rdr_count));

			      DEBUGMSGTL ((AGENT,
					   "Sending RDR TRAP/EVENT\n"));
			      send_v2trap (trap_var);
			      snmp_free_varbind (trap_var);
			    }
			  else
			    {

			      snmp_log (LOG_WARNING,
					"Could not build a RDR TRAP/EVENT message.\n");

			      rc = AGENT_ERR_BUILD_TRAP;
			    }

			}
		    }


		}
	    }
	  else
	    {			// Bail out.
	      // This can happend if there are no entries for that Resource
	      rc = AGENT_ERR_OPERATION;
	      break;
	    }
	}
      while (next_rdr != SAHPI_LAST_ENTRY);


    }

  DEBUGMSGTL ((AGENT, "\n\t--- populate_rdr. Exit\n"));
  return rc;
}

/*
 * Purges    up to FIVE different rows:
 *  - RDR
 *  - Sensor
 *  - Inventory
 *  - Control
 *  - Watchdog
 */

unsigned long
purge_rdr ()
{

  SaHpiDomainIdT domain_id;
  SaHpiResourceIdT resource_id;
  SaHpiEntryIdT num;
  SaHpiRdrTypeT type;
  unsigned long child_id;
  saHpiRdrTable_context *rdr_context;
  //   SaHpiSessionIdT session_id;
  //   SaHpiRdrT rdr_entry;

  int rc;
  unsigned long count = 0;
  unsigned int deleted;


  // Q: How do we determine which RDR entry is "fresh" ?
  // A: 'dirty-bit' is set (in populate_rdr) 
  //    to AGENT_FALSE for existing ('cleaned') records. The
  //    dirty ones are removed.

  DEBUGMSGTL ((AGENT, "purge_rdr. Entry.\n"));

  if (rdr_mutex == AGENT_TRUE)
    {
      rdr_context = CONTAINER_FIRST (cb.container);
      while (rdr_context != NULL)
	{
	  deleted = AGENT_FALSE;
	  DEBUGMSGTL ((AGENT, "Found %d.%d.%d (%X) (child: %d) purge: %s\n",
		       rdr_context->domain_id,
		       rdr_context->saHpiResourceID,
		       rdr_context->saHpiRdrRecordId,
		       rdr_context->saHpiRdrRecordId,
		       rdr_context->saHpiRdrId,
		       (rdr_context->dirty_bit ==
			AGENT_TRUE) ? "Yes" : "No"));

	  if (rdr_context != NULL)
	    {
	      if (rdr_context->dirty_bit == AGENT_FALSE)
		{
		  // Its ok.
		  rdr_context->dirty_bit = AGENT_TRUE;
		}
	      else
		{
		  // Dirty bit hasn't been cleaned. Purge the record.
		  // Copy the values (we are going to remove 'rdr_context' and
		  // we need the domain_id, resource_id, etc values for 
		  // deleteing sub-RDR records.
		  domain_id = rdr_context->domain_id;
		  resource_id = rdr_context->saHpiResourceID;
		  num = rdr_context->saHpiRdrRecordId;
		  type = rdr_context->saHpiRdrType;
		  child_id = rdr_context->saHpiRdrId;
		  // We are getting the next item here b/c effectivly  the rpt_context
		  // will be set to NULL in the 'delete_rpt_row' 
		  rdr_context = CONTAINER_NEXT (cb.container, rdr_context);
		  deleted = AGENT_TRUE;
		  count++;
		  // Delete the RDR row
		  rc = delete_rdr_row (domain_id, resource_id, num, type);
		  if (rc != AGENT_ERR_NOERROR)
		    DEBUGMSGTL ((AGENT,
				 "delete_rdr_row failed. Return code: %d.\n",
				 rc));
		  // Delete the other sub-type. Keep in mind that this will delete
		  // _only_ the specific subtypes. Therfore other records
		  // with the same resource_id, domain_id, and num can still
		  // exist.
		  switch (type)
		    {
		    case SAHPI_NO_RECORD:
		      break;
		    case SAHPI_CTRL_RDR:
		      rc = delete_ctrl_row (domain_id, resource_id, child_id);
		      break;
		    case SAHPI_SENSOR_RDR:
		      rc =
			delete_sensor_row (domain_id, resource_id, child_id);
		      break;
		    case SAHPI_INVENTORY_RDR:
		      rc =
			delete_inventory_rows (domain_id, resource_id,
					       child_id);
		      break;
		    case SAHPI_WATCHDOG_RDR:
		      rc =
			delete_watchdog_row (domain_id, resource_id,
					     child_id);
		      break;
		    default:
		      break;
		    }
		  if (rc != AGENT_ERR_NOERROR)
		    DEBUGMSGTL ((AGENT,
				 "Couldn't delete sub-RDR entry (rc: %d)\n",
				 rc));

		}
	    }
	  // Only get the next item if no deletion has happend.
	  if (deleted == AGENT_FALSE)
	    rdr_context = CONTAINER_NEXT (cb.container, rdr_context);
	}
      rdr_mutex = AGENT_FALSE;
    }
  DEBUGMSGTL ((AGENT, "purge_rdr: Exit (delete: %d).\n", count));
  return count;
}

int
delete_rdr_row (SaHpiDomainIdT domain_id,
		SaHpiResourceIdT resource_id,
		SaHpiEntryIdT num, SaHpiRdrTypeT type)
{

  saHpiRdrTable_context *ctx;
  oid rdr_oid[RDR_INDEX_NR];
  netsnmp_index rdr_index;
  int rc = AGENT_ERR_NOT_FOUND;

  DEBUGMSGTL ((AGENT, "delete_rdr_row (%d, %d, %d, %d). Entry.\n",
	       domain_id, resource_id, num, type));
// Look at the MIB to find out what the indexs are
  rdr_oid[0] = domain_id;
  rdr_oid[1] = resource_id;
  rdr_oid[2] = num;
  rdr_oid[3] = type;

  // Possible more indexs?
  rdr_index.oids = (oid *) & rdr_oid;
  rdr_index.len = RDR_INDEX_NR;

  ctx = CONTAINER_FIND (cb.container, &rdr_index);

  if (ctx)
    {

      CONTAINER_REMOVE (cb.container, ctx);
      saHpiRdrTable_delete_row (ctx);
      rdr_count = CONTAINER_SIZE (cb.container);
      rc = AGENT_ERR_NOERROR;
    }
  DEBUGMSGTL ((AGENT, "delete_rdr_row. Exit (rc: %d)\n", rc));
  return rc;
}

int
saHpiRdrTable_modify_context (SaHpiRptEntryT * rpt_entry,
			      SaHpiRdrT * entry,
			      saHpiRdrTable_context * ctx,
			      oid * rpt_oid,
			      size_t rpt_oid_len,
			      oid * child_oid,
			      size_t child_oid_len,
			      unsigned long child_id,
			      trap_vars ** var,
			      size_t * var_len, oid ** var_trap_oid)
{


  long hash;
  int len;

  // Make sure they are valid.
  if (entry && ctx)
    {

      // We are subtracting SaHpiTextBufferT b/c the underlaying HPI
      // library is not zeroing out the memory for not used entries -
      // thus garbage in SaHpiTextBufferT exist,
      hash = calculate_hash_value (entry, sizeof (SaHpiRdrTypeT) +
				   sizeof (SaHpiEntityPathT) +
				   sizeof (SaHpiEntryIdT));

      DEBUGMSGTL ((AGENT, " Hash value: %d, in ctx: %d\n", hash, ctx->hash));

      if (ctx->hash != 0)
	{
	  // Only do the check if the hash value is something else than zero.
	  // 'zero' value is only for newly created records, and in some
	  // rare instances when the hash has rolled to zero - in which
	  // case we will just consider the worst-case scenario and update
	  // the record and not trust the hash value.
	  if (hash == ctx->hash)
	    {
	      // The same data. No need to change.
	      return AGENT_ENTRY_EXIST;
	    }
	}

      if (hash == 0)
	hash = -1;

      ctx->hash = hash;
      ctx->domain_id = rpt_entry->DomainId;
      //ctx->child_id = child_id;
      ctx->saHpiResourceID = rpt_entry->ResourceId;
      ctx->saHpiRdrRecordId = entry->RecordId;
      ctx->saHpiRdrType = entry->RdrType;

      len = entitypath2string (&entry->Entity,
			       ctx->saHpiRdrEntityPath, SNMP_MAX_MSG_SIZE);

      // Try to get it from rpt_entry.

      if (len == 0)
	{
	  len = entitypath2string (&rpt_entry->ResourceEntity,
				   ctx->saHpiRdrEntityPath,
				   SNMP_MAX_MSG_SIZE);
	}

      if (len < 0)
	{
	  // Bummer, EntityPath too long to fit in the SNMP_MAX_MSG_SIZE.
	  len = 0;
	}
      DEBUGMSGTL ((AGENT, "EntityPath: %s\n", ctx->saHpiRdrEntityPath));
      ctx->saHpiRdrEntityPath_len = len;

      ctx->saHpiRdr_len = child_oid_len * sizeof (oid);
      memcpy (ctx->saHpiRdr, child_oid, ctx->saHpiRdr_len);


      ctx->saHpiRdrRTP_len = rpt_oid_len * sizeof (oid);
      memcpy (ctx->saHpiRdrRTP, rpt_oid, ctx->saHpiRdrRTP_len);

      ctx->saHpiRdrId = child_id;

      // Fix the trap messages
      saHpiResourceDataRecordNotification[RDR_NOTIF_RDRRECORDID].value =
	(u_char *) & ctx->saHpiRdrRecordId;
      saHpiResourceDataRecordNotification[RDR_NOTIF_RDRRECORDID].value_len =
	sizeof (ctx->saHpiRdrRecordId);

      saHpiResourceDataRecordNotification[RDR_NOTIF_TYPE].value =
	(u_char *) & ctx->saHpiRdrType;
      saHpiResourceDataRecordNotification[RDR_NOTIF_TYPE].value_len =
	sizeof (ctx->saHpiRdrType);


      saHpiResourceDataRecordNotification[RDR_NOTIF_ENTITY_PATH].value =
	(u_char *) & ctx->saHpiRdrEntityPath;
      saHpiResourceDataRecordNotification[RDR_NOTIF_ENTITY_PATH].value_len =
	ctx->saHpiRdrEntityPath_len;

      saHpiResourceDataRecordNotification[RDR_NOTIF_RDR].value =
	(u_char *) & ctx->saHpiRdr;
      saHpiResourceDataRecordNotification[RDR_NOTIF_RDR].value_len =
	ctx->saHpiRdr_len;

      saHpiResourceDataRecordNotification[RDR_NOTIF_RDR_RTP].value =
	(u_char *) & ctx->saHpiRdrRTP;
      saHpiResourceDataRecordNotification[RDR_NOTIF_RDR_RTP].value_len =
	ctx->saHpiRdrRTP_len;

      // Point *var to this trap_vars. 
      *var = (trap_vars *) & saHpiResourceDataRecordNotification;
      *var_len = RDR_NOTIF_COUNT;
      *var_trap_oid = (oid *) & saHpiResourceDataRecordNotification_oid;



      return AGENT_NEW_ENTRY;
    }

  return AGENT_ERR_NULL_DATA;
}


/*
 * the *_extract_index routine
 */
int
saHpiRdrTable_extract_index (saHpiRdrTable_context * ctx, netsnmp_index * hdr)
{
  /*
   * temporary local storage for extracting oid index
   */
  netsnmp_variable_list var_saHpiDomainID;
  netsnmp_variable_list var_saHpiResourceID;
  netsnmp_variable_list var_saHpiRdrRecordId;
  netsnmp_variable_list var_saHpiRdrType;
  int err;

  /*
   * copy index, if provided
   */
  if (hdr)
    {
      netsnmp_assert (ctx->index.oids == NULL);
      if (snmp_clone_mem ((void *) &ctx->index.oids, hdr->oids,
			  hdr->len * sizeof (oid)))
	{
	  return -1;
	}
      ctx->index.len = hdr->len;
    }

    /**
     * Create variable to hold each component of the index
     */

  memset (&var_saHpiDomainID, 0x00, sizeof (var_saHpiDomainID));
  var_saHpiDomainID.type = ASN_UNSIGNED;
  var_saHpiDomainID.next_variable = &var_saHpiResourceID;

  memset (&var_saHpiResourceID, 0x00, sizeof (var_saHpiResourceID));
  var_saHpiResourceID.type = ASN_UNSIGNED;
  var_saHpiResourceID.next_variable = &var_saHpiRdrRecordId;

  memset (&var_saHpiRdrRecordId, 0x00, sizeof (var_saHpiRdrRecordId));
  var_saHpiRdrRecordId.type = ASN_UNSIGNED;
  var_saHpiRdrRecordId.next_variable = &var_saHpiRdrType;

  memset (&var_saHpiRdrType, 0x00, sizeof (var_saHpiRdrType));
  var_saHpiRdrType.type = ASN_INTEGER;
  var_saHpiRdrType.next_variable = NULL;

  /*
   * parse the oid into the individual components
   */
  err = parse_oid_indexes (hdr->oids, hdr->len, &var_saHpiDomainID);
  if (err == SNMP_ERR_NOERROR)
    {
      /*
       * copy components into the context structure
       */
      ctx->saHpiResourceID = *var_saHpiResourceID.val.integer;
      ctx->saHpiRdrRecordId = *var_saHpiRdrRecordId.val.integer;
      ctx->saHpiRdrType = *var_saHpiRdrType.val.integer;

    }

  /*
   * parsing may have allocated memory. free it.
   */
  snmp_reset_var_buffers (&var_saHpiDomainID);

  return err;
}


/************************************************************
 * the *_delete_row method is called to delete a row.
 */
netsnmp_index *
saHpiRdrTable_delete_row (saHpiRdrTable_context * ctx)
{


  if (ctx->index.oids)
    free (ctx->index.oids);


  free (ctx);

  return NULL;
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
 * returns a newly allocated saHpiRdrTable_context
 *   structure if the specified indexes are not illegal
 * returns NULL for errors or illegal index values.
 */
saHpiRdrTable_context *
saHpiRdrTable_create_row (netsnmp_index * hdr)
{

  saHpiRdrTable_context *ctx = SNMP_MALLOC_TYPEDEF (saHpiRdrTable_context);
  if (!ctx)
    return NULL;

  if (saHpiRdrTable_extract_index (ctx, hdr))
    {
      free (ctx->index.oids);
      free (ctx);
      return NULL;
    }

  ctx->hash = 0;
  ctx->dirty_bit = AGENT_TRUE;
  return ctx;
}





/************************************************************
 *
 * Initialize the saHpiRdrTable table by defining its contents and how it's structured
 */
void
initialize_table_saHpiRdrTable (void)
{
  netsnmp_table_registration_info *table_info;

  if (my_handler)
    {
      snmp_log (LOG_ERR,
		"initialize_table_saHpiRdrTable_handler called again\n");
      return;
    }

  memset (&cb, 0x00, sizeof (cb));

    /** create the table structure itself */
  table_info = SNMP_MALLOC_TYPEDEF (netsnmp_table_registration_info);

  /*
   * if your table is read only, it's easiest to change the
   * HANDLER_CAN_RWRITE definition below to HANDLER_CAN_RONLY 
   */
  my_handler = netsnmp_create_handler_registration ("saHpiRdrTable",
						    netsnmp_table_array_helper_handler,
						    saHpiRdrTable_oid,
						    saHpiRdrTable_oid_len,
						    HANDLER_CAN_RONLY);

  if (!my_handler || !table_info)
    {
      snmp_log (LOG_ERR, "malloc failed in "
		"initialize_table_saHpiRdrTable_handler\n");
      return;	/** mallocs failed */
    }


	/** index: saHpiRdrRecordId */
  netsnmp_table_helper_add_index (table_info, ASN_UNSIGNED);
  netsnmp_table_helper_add_index (table_info, ASN_UNSIGNED);
  netsnmp_table_helper_add_index (table_info, ASN_UNSIGNED);
	/** index: saHpiRdrType */
  netsnmp_table_helper_add_index (table_info, ASN_INTEGER);

  table_info->min_column = saHpiRdrTable_COL_MIN;
  table_info->max_column = saHpiRdrTable_COL_MAX;

    /***************************************************
     * registering the table with the master agent
     */
  cb.get_value = saHpiRdrTable_get_value;
  cb.container = netsnmp_container_find ("saHpiRdrTable_primary:"
					 "saHpiRdrTable:" "table_container");

  cb.create_row = (UserRowMethod *) saHpiRdrTable_create_row;


  DEBUGMSGTL (("initialize_table_saHpiRdrTable",
	       "Registering table saHpiRdrTable " "as a table array\n"));

  netsnmp_table_container_register (my_handler, table_info, &cb,
				    cb.container, 1);

  netsnmp_register_read_only_counter32_instance ("rdr_count",
						 saHpiRdrCount_oid,
						 OID_LENGTH
						 (saHpiRdrCount_oid),
						 &rdr_count, NULL);

}

/************************************************************
 * saHpiRdrTable_get_value
 */
int
saHpiRdrTable_get_value (netsnmp_request_info * request,
			 netsnmp_index * item,
			 netsnmp_table_request_info * table_info)
{
  netsnmp_variable_list *var = request->requestvb;
  saHpiRdrTable_context *context = (saHpiRdrTable_context *) item;


  DEBUGMSGTL ((AGENT, "--- saHpiRdrTable_get_value: Entry\n"));
  switch (table_info->colnum)
    {




    case COLUMN_SAHPIRDRRECORDID:
	    /** UNSIGNED32 = ASN_UNSIGNED */
      snmp_set_var_typed_value (var, ASN_UNSIGNED,
				(char *) &context->saHpiRdrRecordId,
				sizeof (context->saHpiRdrRecordId));
      break;

    case COLUMN_SAHPIRDRTYPE:
	    /** INTEGER = ASN_INTEGER */
      snmp_set_var_typed_value (var, ASN_INTEGER,
				(char *) &context->saHpiRdrType,
				sizeof (context->saHpiRdrType));
      break;

    case COLUMN_SAHPIRDRENTITYPATH:
	    /** OCTETSTR = ASN_OCTET_STR */
      snmp_set_var_typed_value (var, ASN_OCTET_STR,
				(char *) &context->saHpiRdrEntityPath,
				context->saHpiRdrEntityPath_len);
      break;

    case COLUMN_SAHPIRDR:
	    /** RowPointer = ASN_OBJECT_ID */
      snmp_set_var_typed_value (var, ASN_OBJECT_ID,
				(char *) &context->saHpiRdr,
				context->saHpiRdr_len);
      break;

    case COLUMN_SAHPIRDRID:
	    /** COUNTER = ASN_COUNTER */
      snmp_set_var_typed_value (var, ASN_COUNTER,
				(char *) &context->saHpiRdrId,
				sizeof (context->saHpiRdrId));
      break;

    case COLUMN_SAHPIRDRRTP:
	    /** RowPointer = ASN_OBJECT_ID */
      snmp_set_var_typed_value (var, ASN_OBJECT_ID,
				(char *) &context->saHpiRdrRTP,
				context->saHpiRdrRTP_len);
      break;

    default:
	    /** We shouldn't get here */
      snmp_log (LOG_ERR, "unknown column in " "saHpiRdrTable_get_value\n");
      return SNMP_ERR_GENERR;
    }

  DEBUGMSGTL ((AGENT, "--- saHpiRdrTable_get_value: Exit\n"));
  return SNMP_ERR_NOERROR;
}
