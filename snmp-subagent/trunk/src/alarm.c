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

  DEBUGMSGTL((AGENT,"--- do_alarm: Exit\n"));
}
