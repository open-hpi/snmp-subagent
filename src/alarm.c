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
#include <alarm.h>
#include <hpiSubagent.h>
#include <net-snmp/net-snmp-config.h>
#include <net-snmp/net-snmp-includes.h>
#include <net-snmp/agent/net-snmp-agent-includes.h>

extern int alarm_interval;



int
init_alarm ()
{

  int rc = AGENT_ERR_NOERROR;
  DEBUGMSGTL ((AGENT, "--- init_alarm: Entry\n"));
  DEBUGMSGTL ((AGENT, "Alarm every: %d\n", alarm_interval));
  if (snmp_alarm_register (alarm_interval, SA_REPEAT, do_alarm, NULL) == 0)
    {
      rc = AGENT_ERR_MEMORY_FAULT;
    }

  DEBUGMSGTL ((AGENT, "--- init_alarm: Exit\n"));
  return rc;
}

void
do_alarm (unsigned int clientreg, void *clientarg)
{

  int rc;
  DEBUGMSGTL ((AGENT, "--- do_alarm: Entry\n"));

  DEBUGMSGTL ((AGENT, "TODO: Call to populate_rpt() returns: %d.\n", rc));
//  rc = populate_rpt ();
  DEBUGMSGTL ((AGENT, "Call to populate_rpt() returns: %d.\n", rc));

  DEBUGMSGTL ((AGENT, "TODO: Call to purge_rpt(). Purged: %d RPT records.\n", rc));  
//  rc = purge_rpt ();
  DEBUGMSGTL ((AGENT, "Call to purge_rpt(). Purged: %d RPT records.\n", rc));
  if (rc > 0) 
  	snmp_log (LOG_INFO,"Removed %d RPT record%s.\n", rc,
		(rc > 1) ? "s" : "" );
  // No need to call 'populate_rdr()' b/c populate_rpt() already did
  // that per each RPT. - The populate_rdr() marked used rows and
  // purge_rdr() will just erase non-marked rows.
  DEBUGMSGTL ((AGENT, "TODO: Call to purge_rdr(). Purged: %d RDR records.\n", rc));  
//  rc = purge_rdr ();
  DEBUGMSGTL ((AGENT, "Call to purge_rdr(). Purged: %d RDR records.\n", rc));
  if (rc > 0) 
  	snmp_log (LOG_INFO,"Removed %d RDR record%s.\n", rc,
		(rc > 1) ? "s" : "" );

  // Get new events, if there are any.
  DEBUGMSGTL ((AGENT, "TODO: Call to populate_event() returns: %d.\n", rc));  
//  rc = populate_event ();
  DEBUGMSGTL ((AGENT, "Call to populate_event() returns: %d.\n", rc));

  DEBUGMSGTL ((AGENT, "TODO: Call to purge_event(). Purged: %d EVENT rows.\n", rc));  
//  rc = purge_event ();
  DEBUGMSGTL ((AGENT, "Call to purge_event(). Purged: %d EVENT rows.\n", rc));
  if (rc > 0)
  	snmp_log (LOG_INFO,"Removed %d Event record%s.\n", rc,
		(rc > 1) ? "s" : "" );

  DEBUGMSGTL ((AGENT, "--- do_alarm: Exit\n"));
}
