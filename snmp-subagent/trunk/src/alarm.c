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

#include <saHpiTable.h>
#include <saHpiRdrTable.h>
#include <saHpiEventTable.h>

extern int alarm_interval;

int
init_alarm() {

  int rc = AGENT_ERR_NOERROR;
  DEBUGMSGTL((AGENT,"--- init_alarm: Entry\n"));
  DEBUGMSGTL((AGENT,"Alarm every: %d\n", alarm_interval));
  if (snmp_alarm_register(alarm_interval,
			  SA_REPEAT,
			  do_alarm,
			  NULL) == 0) {
    rc = AGENT_ERR_MEMORY_FAULT;
  }

  DEBUGMSGTL((AGENT,"--- init_alarm: Exit\n"));
  return rc;
}

void
do_alarm(unsigned int clientreg, void *clientarg) {

  int rc;
  DEBUGMSGTL((AGENT,"--- do_alarm: Entry\n"));

  rc = populate_rpt();
  DEBUGMSGTL((AGENT,"Call to populate_rpt() returns: %d.\n", rc));

  rc = purge_rpt();
  DEBUGMSGTL((AGENT,"Call to purge_rpt(). Purged: %d RPT records.\n", rc));

  // No need to call 'populate_rdr()' b/c populate_rpt() already did
  // that per each RPT. - The populate_rdr() marked used rows and
  // purge_rdr() will just erase non-marked rows.
  rc = purge_rdr();
  DEBUGMSGTL((AGENT,"Call to purge_rdr(). Purged: %d RDR records.\n", rc));

  // Get new events, if there are any.
  rc = populate_event();
  DEBUGMSGTL((AGENT,"Call to populate_event() returns: %d.\n", rc));

  //rc = purge_event();
  DEBUGMSGTL((AGENT,"Call to purge_event(). Purged: %d EVENT rows.\n", rc));

  DEBUGMSGTL((AGENT,"--- do_alarm: Exit\n"));
}
