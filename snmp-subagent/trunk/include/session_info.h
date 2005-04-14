/*
 * (C) Copyright IBM Corp. 2005
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  This
 * file and program are licensed under a BSD style license.  See
 * the Copying file included with the OpenHPI distribution for
 * full licensing terms.
 *
 * Authors:
 *   David Judkovics  <djudkovi@us.ibm.com>
 *
 *					  
 */

#ifndef _SESSION_INFO_H_
#define _SESSION_INFO_H_

#include <net-snmp/net-snmp-config.h>
#include <net-snmp/net-snmp-includes.h>
#include <net-snmp/agent/net-snmp-agent-includes.h>
#include <signal.h>

#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>

#include <hpiSubagent.h>

#include <SaHpi.h> 
#include <oh_utils.h>

#include <alarm.h>
#include <oh_error.h>

void store_session_info(SaHpiSessionIdT session_id, SaHpiDomainIdT domain_id);

SaHpiSessionIdT get_session_id(SaHpiDomainIdT domain_id);

SaHpiDomainIdT get_domain_id(SaHpiSessionIdT session_id);

#endif //_SESSION_INFO_H_
