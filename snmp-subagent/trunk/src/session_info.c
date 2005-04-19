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

#include <session_info.h>

static SaHpiSessionIdT stored_session_id;
static SaHpiDomainIdT stored_domain_id;

void store_session_info(SaHpiSessionIdT session_id, SaHpiDomainIdT domain_id)
{
	stored_session_id = session_id;
	stored_domain_id = domain_id;
}

SaHpiSessionIdT get_session_id(SaHpiDomainIdT domain_id)
{
	return( stored_session_id );
}

SaHpiDomainIdT get_domain_id(SaHpiSessionIdT session_id)
{
	return( stored_domain_id );
}



/*
 *  int build_full_oid()
 */
int build_full_oid (oid * prefix, size_t prefix_len,
		    oid * column, size_t column_len,
		    netsnmp_index * index,
		    oid * out_oid, size_t in_len, size_t * out_len)
{

  int register len = prefix_len + column_len;
  int register i = 0;

  DEBUGMSGTL((AGENT, "build_full_oid() called\n"));

  if (index)
    len += index->len;

  if (len > in_len)
    return AGENT_ERR_MEMORY_FAULT;

  *out_len = len;
  memcpy (out_oid, prefix, prefix_len * sizeof (oid));

  for (; i < column_len; i++)
    {
      out_oid[prefix_len + i] = column[i];
    }
  len = prefix_len + i;

  if (index)
    {
      for (i = 0; i < index->len; i++)
        {
          out_oid[len + i] = index->oids[i];
        }
    }
  return AGENT_ERR_NOERROR;
}









