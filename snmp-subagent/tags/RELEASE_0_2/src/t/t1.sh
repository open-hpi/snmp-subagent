#!/bin/sh
# (C) Copyright IBM Corp. 2003
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  This
# file and program are licensed under a BSD style license.  See
# the Copying file included with the OpenHPI distribution for
# full licensing terms.
#
# Authors:
#   Konrad Rzeszutek <konradr@us.ibm.com>
#
# $Id$ 
#

. ./shared.env

if [ "$1" == "-v" ]; then
	VERBOSE=yes
fi

start

dbg "Remove the Event tables"
q $SNMPSET -v2c -c $SNMPD_COMMUNITY $SNMPD_SOCKET HPI-MIB::saHpiEventDelete.0.1.0 = 6  > $0.tmp
q $SNMPWALK -v2c -c $SNMPD_COMMUNITY $SNMPD_SOCKET events >> $0.tmp
cat $0.tmp | grep -v $SNMPSET | grep -v $SNMPWALK > $0.result
rm $0.tmp

check
stop
