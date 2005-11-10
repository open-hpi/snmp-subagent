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
 *   Daniel de Araujo <ddearauj@us.ibm.com>
 *
 *
 */

#ifndef HPILOCK_H
#define HPILOCK_H

#ifdef __cplusplus
extern "C" {
#endif
 
#include <stdlib.h>
#include <glib.h>

typedef struct {
      GStaticRecMutex thread_mutex;
      int lockcount;
}hpi_lock_type;

extern hpi_lock_type hpi_lock_data;      

void subagent_lock( hpi_lock_type * hpi_lock_data);
void subagent_unlock( hpi_lock_type * hpi_lock_data);

#endif
