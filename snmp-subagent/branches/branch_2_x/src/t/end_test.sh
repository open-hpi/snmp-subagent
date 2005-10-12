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

stop
