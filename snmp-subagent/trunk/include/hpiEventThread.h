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


#ifndef _HPI_EVENT_THREAD_
#define _HPI_EVENT_THREAD_

int start_event_thread(SaHpiSessionIdT *sessionid);
gboolean get_run_threaded(void);
void set_run_threaded(gboolean val);

extern GMutex *thread_mutex;

#endif /*_HPI_EVENT_THREAD_*/

