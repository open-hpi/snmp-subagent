/*      -*- linux-c -*-
*
*(C) Copyright IBM Corp. 2005
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  This
* file and program are licensed under a BSD style license.  See
* the Copying file included with the OpenHPI distribution for
* full licensing terms.
*
* Authors:
*     W. david Ashley <dashley@us.ibm.com>    
*     Daniel de Araujo <ddearauj@us.ibm.com>
*     David Judkovics <djudkovi@us.ibm.com>
*/

/* Instructions for use: 
 * 1. Compile: gcc [-o pick_a_name] simulator.c
 * 2. Copy the executable to the directory 
 *    where you are running the subagent. 
 *    Likely: <path>/openhpi-subagent/src
 * 3. Login as root.
 * 4. To execute: ./exectuable -n handler_name,
 *    where the handler_name is the name you 
 *    specified in openhpi.conf.
 */

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/types.h>
#include </usr/local/include/openhpi/SaHpi.h>
#include </usr/local/include/openhpi/sim_injector_ext.h>
#include "simulator.h"


/* Global variables */ 
char topmenu[]="\nPlease select an event type to inject:\n\n"
               "1. RESOURCE \n"
	       "2. DOMAIN \n"
	       "3. SENSOR \n"
	       "4. SENSOR_ENABLE_CHANGE \n"
	       "5. HOTSWAP \n"
	       "6. WATCHDOG \n"
	       "7. SOFTWARE \n"
	       "8. Oem \n"
	       "9. USER \n\n"
	       "...or select q to quit:  ";

char resource_menu[]="\nPlease select a Resource Event Type:\n\n"
               "1. RESOURCE_FAILURE \n"
	       "2. RESOURCE_RESTORED \n"
	       "3. RESOURCE_ADDED \n"
	       "...or select q to quit:  ";

char domain_menu[]="\nPlease select a Resource Event Type:\n\n"
               "1. DOMAIN_REF_ADDED \n"
	       "2. DOMAIN_REF_REMOVED \n"
	       "...or select q to quit:  ";

static int get_user_input (char *input, char *menu) 
{
 	int retcode = -1;
	char val = 0;
	char val1 = 0;
	
	char s[10];
	
	do {     

		printf("%s", menu);
	
		fgets(s, sizeof(s), stdin);
		
                if ( ( (atoi(&s[0]) < 1) || (atoi(&s[0]) > 9)) &&
		       s[0] != 'q') {		       
			
			printf("\n**** Please select a specified value ****\n");
		}
		else {
			if (s[0] == 'q') {
				retcode = 0;
			}
			else {
				*input = atoi(&s[0]);
				retcode = 1;
			}
		}
					
		
	}while(retcode == -1);	
	
	return retcode;
}


/**
 * 
 * SIM_MSG_RESOURCE_ADD_EVENT
 * 
 * @param plugin_name
 * 
 * @return 
 */
static int inject_resource_add_event(char *plugin_name) {
    key_t ipckey;
    int msgqueid;
    SIM_MSG_QUEUE_BUF buf;
    size_t n = 0;  // size of the data in the msg buf
    char *txtptr = buf.mtext;
    int rc;

    /* get the  queue */
    ipckey = ftok(".", SIM_MSG_QUEUE_KEY);
    msgqueid = msgget(ipckey, 0660);
    if (msgqueid == -1) {
        return -1;
    }

    /* fill out the message */
    buf.mtype = SIM_MSG_RESOURCE_ADD_EVENT;
    *txtptr = '\0';
    sprintf(txtptr, "%s=%s", SIM_MSG_HANDLER_NAME, plugin_name);
    n += strlen(txtptr) + 1;
    txtptr = buf.mtext + n;
    if (n > SIM_MSG_QUEUE_BUFSIZE) {
        return -1;
    }
    sprintf(txtptr, "%s=%s", SIM_MSG_RPT_ENTITYPATH,
            "{BIOS,1}");
    n += strlen(txtptr) + 1;
    txtptr = buf.mtext + n;
    if (n > SIM_MSG_QUEUE_BUFSIZE) {
        return -1;
    }
    sprintf(txtptr, "%s=%d", SIM_MSG_RPT_CAPABILITIES, SAHPI_CAPABILITY_RESOURCE);
    n += strlen(txtptr) + 1;
    txtptr = buf.mtext + n;
    if (n > SIM_MSG_QUEUE_BUFSIZE) {
        return -1;
    }
    sprintf(txtptr, "%s=%d", SIM_MSG_RPT_HSCAPABILITIES, 0);
    n += strlen(txtptr) + 1;
    txtptr = buf.mtext + n;
    if (n > SIM_MSG_QUEUE_BUFSIZE) {
        return -1;
    }
    sprintf(txtptr, "%s=%d", SIM_MSG_EVENT_SEVERITY, SAHPI_OK);
    n += strlen(txtptr) + 1;
    txtptr = buf.mtext + n;
    if (n > SIM_MSG_QUEUE_BUFSIZE) {
        return -1;
    }
    sprintf(txtptr, "%s=%d", SIM_MSG_RPT_FAILED, SAHPI_FALSE);
    n += strlen(txtptr) + 1;
    txtptr = buf.mtext + n;
    if (n > SIM_MSG_QUEUE_BUFSIZE) {
        return -1;
    }
    sprintf(txtptr, "%s=%s", SIM_MSG_RPT_COMMENT, "BIOS comment");
    n += strlen(txtptr) + 1;
    txtptr = buf.mtext + n;
    if (n > SIM_MSG_QUEUE_BUFSIZE) {
        return -1;
    }
    *txtptr = '\0'; // terminate buf with a zero-length string
    n++;

    /* send the msg */
    rc = msgsnd(msgqueid, &buf, n, 0);
    if (rc) {
        return -1;
    }

    return 0;
 }


/**
 * 
 * SAHPI_RESE_RESOURCE_RESTORED
 * 
 * @param handler_name
 * @param selection
 * 
 * @return 
 */
static int inject_resource_event (char *handler_name, char selection)
{
    key_t ipckey;
    int msgqueid;
    SIM_MSG_QUEUE_BUF buf;
    size_t n = 0;  // size of the data in the msg buf
    char *txtptr = buf.mtext;
    int rc;

    printf("\nSTATUS: Injecting a resource event...\n");

    /* get the  queue */
    ipckey = ftok(".", SIM_MSG_QUEUE_KEY);
    msgqueid = msgget(ipckey, 0660);
    if (msgqueid == -1) {
        return -1;
    }

    /* fill out the message */
    buf.mtype = SIM_MSG_RESOURCE_EVENT;
    *txtptr = '\0';
    sprintf(txtptr, "%s=%s", SIM_MSG_HANDLER_NAME, handler_name);
    n += strlen(txtptr) + 1;
    txtptr = buf.mtext + n;
    if (n > SIM_MSG_QUEUE_BUFSIZE) {
        return -1;
    }
    sprintf(txtptr, "%s=%d", SIM_MSG_RESOURCE_ID, 1);
    n += strlen(txtptr) + 1;
    txtptr = buf.mtext + n;
    if (n > SIM_MSG_QUEUE_BUFSIZE) {
        return -1;
    }
    sprintf(txtptr, "%s=%d", SIM_MSG_EVENT_SEVERITY, SAHPI_MINOR);
    n += strlen(txtptr) + 1;
    txtptr = buf.mtext + n;
    if (n > SIM_MSG_QUEUE_BUFSIZE) {
        return -1;
    }


    /* build the correct Resource EVENT Type */
    switch (selection) {
    case RESOURCE_FAILURE:
            sprintf(txtptr, "%s=%d", SIM_MSG_RESOURCE_EVENT_TYPE, SAHPI_RESE_RESOURCE_FAILURE);
            break;
    case RESOURCE_RESTORED:
            sprintf(txtptr, "%s=%d", SIM_MSG_RESOURCE_EVENT_TYPE, SAHPI_RESE_RESOURCE_RESTORED);
            break;
    default:
            break;
    }
    n += strlen(txtptr) + 1;
    txtptr = buf.mtext + n;
    if (n > SIM_MSG_QUEUE_BUFSIZE) {
        return -1;
    }
    *txtptr = '\0'; // terminate buf with a zero-length string
    n++;

    /* send the msg */
    rc = msgsnd(msgqueid, &buf, n, 0);
    if (rc) {
        return -1;
    }

    return 0;

}

static int inject_domain_event (char *handler_name, char selection)
{
    key_t ipckey;
    int msgqueid;
    SIM_MSG_QUEUE_BUF buf;
    size_t n = 0;  // size of the data in the msg buf
    char *txtptr = buf.mtext;
    int rc;

    printf("\nSTATUS: Injecting a domain event...\n");

    /* get the  queue */
    ipckey = ftok(".", SIM_MSG_QUEUE_KEY);
    msgqueid = msgget(ipckey, 0660);
    if (msgqueid == -1) {
        return -1;
    }

    /* fill out the message */
    buf.mtype = SIM_MSG_DOMAIN_EVENT;
    *txtptr = '\0';
    sprintf(txtptr, "%s=%s", SIM_MSG_HANDLER_NAME, handler_name);
    n += strlen(txtptr) + 1;
    txtptr = buf.mtext + n;
    if (n > SIM_MSG_QUEUE_BUFSIZE) {
        return -1;
    }
    sprintf(txtptr, "%s=%d", SIM_MSG_EVENT_SEVERITY, SAHPI_MINOR);
    n += strlen(txtptr) + 1;
    txtptr = buf.mtext + n;
    if (n > SIM_MSG_QUEUE_BUFSIZE) {
        return -1;
    }

        /* build the correct Resource EVENT Type */
    switch (selection) {
    case DOMAIN_REF_ADDED:
            sprintf(txtptr, "%s=%d", SIM_MSG_DOMAIN_TYPE, SAHPI_DOMAIN_REF_ADDED);
            break;
    case DOMAIN_REF_REMOVED:
            sprintf(txtptr, "%s=%d", SIM_MSG_DOMAIN_TYPE, SAHPI_DOMAIN_REF_REMOVED);
            break;
    default:
            break;
    }

    n += strlen(txtptr) + 1;
    txtptr = buf.mtext + n;
    if (n > SIM_MSG_QUEUE_BUFSIZE) {
        return -1;
    }
    sprintf(txtptr, "%s=%d", SIM_MSG_DOMAIN_ID, 2);
    n += strlen(txtptr) + 1;
    txtptr = buf.mtext + n;
    if (n > SIM_MSG_QUEUE_BUFSIZE) {
        return -1;
    }
    *txtptr = '\0'; // terminate buf with a zero-length string
    n++;

    /* send the msg */
    rc = msgsnd(msgqueid, &buf, n, 0);
    if (rc) {
        return -1;
    }

    return 0;

}
	       
static int inject_sensor_event(char *plugin_name) {
    key_t ipckey;
    int msgqueid;
    SIM_MSG_QUEUE_BUF buf;
    size_t n = 0;  // size of the data in the msg buf
    char *txtptr = buf.mtext;
    int rc;

    printf("\nSTATUS: Injecting a sensor event...\n");


    /* get the  queue */
    ipckey = ftok(".", SIM_MSG_QUEUE_KEY);
    msgqueid = msgget(ipckey, 0660);
    if (msgqueid == -1) {
        return -1;
    }

    /* fill out the message */
    buf.mtype = SIM_MSG_SENSOR_EVENT;
    *txtptr = '\0';
    sprintf(txtptr, "%s=%s", SIM_MSG_HANDLER_NAME, plugin_name);
    n += strlen(txtptr) + 1;
    txtptr = buf.mtext + n;
    if (n > SIM_MSG_QUEUE_BUFSIZE) {
        return -1;
    }
    sprintf(txtptr, "%s=%d", SIM_MSG_RESOURCE_ID, 1);
    n += strlen(txtptr) + 1;
    txtptr = buf.mtext + n;
    if (n > SIM_MSG_QUEUE_BUFSIZE) {
        return -1;
    }
    sprintf(txtptr, "%s=%d", SIM_MSG_EVENT_SEVERITY, SAHPI_CRITICAL);
    n += strlen(txtptr) + 1;
    txtptr = buf.mtext + n;
    if (n > SIM_MSG_QUEUE_BUFSIZE) {
        return -1;
    }
    sprintf(txtptr, "%s=%d", SIM_MSG_SENSOR_NUM, 1);
    n += strlen(txtptr) + 1;
    txtptr = buf.mtext + n;
    if (n > SIM_MSG_QUEUE_BUFSIZE) {
        return -1;
    }
    sprintf(txtptr, "%s=%d", SIM_MSG_SENSOR_TYPE, SAHPI_TEMPERATURE);
    n += strlen(txtptr) + 1;
    txtptr = buf.mtext + n;
    if (n > SIM_MSG_QUEUE_BUFSIZE) {
        return -1;
    }
    sprintf(txtptr, "%s=%d", SIM_MSG_SENSOR_EVENT_CATEGORY, SAHPI_EC_THRESHOLD);
    n += strlen(txtptr) + 1;
    txtptr = buf.mtext + n;
    if (n > SIM_MSG_QUEUE_BUFSIZE) {
        return -1;
    }
    sprintf(txtptr, "%s=%d", SIM_MSG_SENSOR_ASSERTION, SAHPI_TRUE);
    n += strlen(txtptr) + 1;
    txtptr = buf.mtext + n;
    if (n > SIM_MSG_QUEUE_BUFSIZE) {
        return -1;
    }
    sprintf(txtptr, "%s=%d", SIM_MSG_SENSOR_EVENT_STATE, SAHPI_ES_UPPER_CRIT);
    n += strlen(txtptr) + 1;
    txtptr = buf.mtext + n;
    if (n > SIM_MSG_QUEUE_BUFSIZE) {
        return -1;
    }
    sprintf(txtptr, "%s=%d", SIM_MSG_SENSOR_OPTIONAL_DATA, SAHPI_SOD_TRIGGER_READING);
    n += strlen(txtptr) + 1;
    txtptr = buf.mtext + n;
    if (n > SIM_MSG_QUEUE_BUFSIZE) {
        return -1;
    }
    sprintf(txtptr, "%s=%d", SIM_MSG_SENSOR_TRIGGER_READING_SUPPORTED, SAHPI_TRUE);
    n += strlen(txtptr) + 1;
    txtptr = buf.mtext + n;
    if (n > SIM_MSG_QUEUE_BUFSIZE) {
        return -1;
    }
    sprintf(txtptr, "%s=%d", SIM_MSG_SENSOR_TRIGGER_READING_TYPE, SAHPI_SENSOR_READING_TYPE_INT64);
    n += strlen(txtptr) + 1;
    txtptr = buf.mtext + n;
    if (n > SIM_MSG_QUEUE_BUFSIZE) {
        return -1;
    }
    sprintf(txtptr, "%s=%d", SIM_MSG_SENSOR_TRIGGER_READING, 21);
    n += strlen(txtptr) + 1;
    txtptr = buf.mtext + n;
    if (n > SIM_MSG_QUEUE_BUFSIZE) {
        return -1;
    }
    *txtptr = '\0'; // terminate buf with a zero-length string
    n++;

    /* send the msg */
    rc = msgsnd(msgqueid, &buf, n, 0);
    if (rc) {
        return -1;
    }

    return 0;
 }



static int inject_sensorec_event (char * handler_name)
{
    key_t ipckey;
    int msgqueid;
    SIM_MSG_QUEUE_BUF buf;
    size_t n = 0;  // size of the data in the msg buf
    char *txtptr = buf.mtext;
    int rc;

    printf("\nSTATUS: Injecting a sensor enable change event...\n");

    /* get the  queue */
    ipckey = ftok(".", SIM_MSG_QUEUE_KEY);
    msgqueid = msgget(ipckey, 0660);
    if (msgqueid == -1) {
        return -1;
    }

    /* fill out the message */
    buf.mtype = SIM_MSG_SENSOR_ENABLE_CHANGE_EVENT;
    *txtptr = '\0';
    sprintf(txtptr, "%s=%s", SIM_MSG_HANDLER_NAME, handler_name);
    n += strlen(txtptr) + 1;
    txtptr = buf.mtext + n;
    if (n > SIM_MSG_QUEUE_BUFSIZE) {
        return -1;
    }
    sprintf(txtptr, "%s=%d", SIM_MSG_RESOURCE_ID, 1);
    n += strlen(txtptr) + 1;
    txtptr = buf.mtext + n;
    if (n > SIM_MSG_QUEUE_BUFSIZE) {
        return -1;
    }
    sprintf(txtptr, "%s=%d", SIM_MSG_EVENT_SEVERITY, SAHPI_MINOR);
    n += strlen(txtptr) + 1;
    txtptr = buf.mtext + n;
    if (n > SIM_MSG_QUEUE_BUFSIZE) {
        return -1;
    }
    sprintf(txtptr, "%s=%d", SIM_MSG_SENSOR_NUM, 1);
    n += strlen(txtptr) + 1;
    txtptr = buf.mtext + n;
    if (n > SIM_MSG_QUEUE_BUFSIZE) {
        return -1;
    }
    sprintf(txtptr, "%s=%d", SIM_MSG_SENSOR_TYPE, SAHPI_TEMPERATURE);
    n += strlen(txtptr) + 1;
    txtptr = buf.mtext + n;
    if (n > SIM_MSG_QUEUE_BUFSIZE) {
        return -1;
    }
    sprintf(txtptr, "%s=%d", SIM_MSG_SENSOR_EVENT_CATEGORY, SAHPI_EC_THRESHOLD);
    n += strlen(txtptr) + 1;
    txtptr = buf.mtext + n;
    if (n > SIM_MSG_QUEUE_BUFSIZE) {
        return -1;
    }
    sprintf(txtptr, "%s=%d", SIM_MSG_SENSOR_ENABLE, SAHPI_TRUE);
    n += strlen(txtptr) + 1;
    txtptr = buf.mtext + n;
    if (n > SIM_MSG_QUEUE_BUFSIZE) {
        return -1;
    }
    sprintf(txtptr, "%s=%d", SIM_MSG_SENSOR_EVENT_ENABLE, SAHPI_TRUE);
    n += strlen(txtptr) + 1;
    txtptr = buf.mtext + n;
    if (n > SIM_MSG_QUEUE_BUFSIZE) {
        return -1;
    }
    sprintf(txtptr, "%s=%d", SIM_MSG_SENSOR_ASSERT_EVENT_MASK, SAHPI_ES_OK);
    n += strlen(txtptr) + 1;
    txtptr = buf.mtext + n;
    if (n > SIM_MSG_QUEUE_BUFSIZE) {
        return -1;
    }
    sprintf(txtptr, "%s=%d", SIM_MSG_SENSOR_DEASSERT_EVENT_MASK, SAHPI_ES_OK);
    n += strlen(txtptr) + 1;
    txtptr = buf.mtext + n;
    if (n > SIM_MSG_QUEUE_BUFSIZE) {
        return -1;
    }
    sprintf(txtptr, "%s=%d", SIM_MSG_SENSOR_OPTIONAL_DATA, SAHPI_SOD_CURRENT_STATE);
    n += strlen(txtptr) + 1;
    txtptr = buf.mtext + n;
    if (n > SIM_MSG_QUEUE_BUFSIZE) {
        return -1;
    }
    sprintf(txtptr, "%s=%d", SIM_MSG_SENSOR_CURRENT_STATE, SAHPI_ES_OK);
    n += strlen(txtptr) + 1;
    txtptr = buf.mtext + n;
    if (n > SIM_MSG_QUEUE_BUFSIZE) {
        return -1;
    }
    *txtptr = '\0'; // terminate buf with a zero-length string
    n++;

    /* send the msg */
    rc = msgsnd(msgqueid, &buf, n, 0);
    if (rc) {
        return -1;
    }

    return 0;

}
static int inject_hotswap_event (char * handler_name)
{
    key_t ipckey;
    int msgqueid;
    SIM_MSG_QUEUE_BUF buf;
    size_t n = 0;  // size of the data in the msg buf
    char *txtptr = buf.mtext;
    int rc;

    printf("\nSTATUS: Injecting a hotswap event...\n");

    /* get the  queue */
    ipckey = ftok(".", SIM_MSG_QUEUE_KEY);
    msgqueid = msgget(ipckey, 0660);
    if (msgqueid == -1) {
        return -1;
    }

    /* fill out the message */
    buf.mtype = SIM_MSG_HOT_SWAP_EVENT;
    *txtptr = '\0';
    sprintf(txtptr, "%s=%s", SIM_MSG_HANDLER_NAME, handler_name);
    n += strlen(txtptr) + 1;
    txtptr = buf.mtext + n;
    if (n > SIM_MSG_QUEUE_BUFSIZE) {
        return -1;
    }
    sprintf(txtptr, "%s=%d", SIM_MSG_RESOURCE_ID, 4);
    n += strlen(txtptr) + 1;
    txtptr = buf.mtext + n;
    if (n > SIM_MSG_QUEUE_BUFSIZE) {
        return -1;
    }
    sprintf(txtptr, "%s=%d", SIM_MSG_EVENT_SEVERITY, SAHPI_MINOR);
    n += strlen(txtptr) + 1;
    txtptr = buf.mtext + n;
    if (n > SIM_MSG_QUEUE_BUFSIZE) {
        return -1;
    }
    sprintf(txtptr, "%s=%d", SIM_MSG_HS_STATE, SAHPI_HS_STATE_ACTIVE);
    n += strlen(txtptr) + 1;
    txtptr = buf.mtext + n;
    if (n > SIM_MSG_QUEUE_BUFSIZE) {
        return -1;
    }
    sprintf(txtptr, "%s=%d", SIM_MSG_HS_PREVIOUS_STATE, SAHPI_HS_STATE_INACTIVE);
    n += strlen(txtptr) + 1;
    txtptr = buf.mtext + n;
    if (n > SIM_MSG_QUEUE_BUFSIZE) {
        return -1;
    }
    *txtptr = '\0'; // terminate buf with a zero-length string
    n++;

    /* send the msg */
    rc = msgsnd(msgqueid, &buf, n, 0);
    if (rc) {
        return -1;
    }

    return 0;

}
static int inject_watchdog_event (char *handler_name) 
{
    key_t ipckey;
    int msgqueid;
    SIM_MSG_QUEUE_BUF buf;
    size_t n = 0;  // size of the data in the msg buf
    char *txtptr = buf.mtext;
    int rc;

    printf("\nSTATUS: Injecting a watchdog event...\n");

    /* get the  queue */
    ipckey = ftok(".", SIM_MSG_QUEUE_KEY);
    msgqueid = msgget(ipckey, 0660);
    if (msgqueid == -1) {
        return -1;
    }

    /* fill out the message */
    buf.mtype = SIM_MSG_WATCHDOG_EVENT;
    *txtptr = '\0';
    sprintf(txtptr, "%s=%s", SIM_MSG_HANDLER_NAME, handler_name);
    n += strlen(txtptr) + 1;
    txtptr = buf.mtext + n;
    if (n > SIM_MSG_QUEUE_BUFSIZE) {
        return -1;
    }
    sprintf(txtptr, "%s=%d", SIM_MSG_RESOURCE_ID, 1);
    n += strlen(txtptr) + 1;
    txtptr = buf.mtext + n;
    if (n > SIM_MSG_QUEUE_BUFSIZE) {
        return -1;
    }
    sprintf(txtptr, "%s=%d", SIM_MSG_EVENT_SEVERITY, SAHPI_MINOR);
    n += strlen(txtptr) + 1;
    txtptr = buf.mtext + n;
    if (n > SIM_MSG_QUEUE_BUFSIZE) {
        return -1;
    }
    sprintf(txtptr, "%s=%d", SIM_MSG_WATCHDOG_NUM, 1);
    n += strlen(txtptr) + 1;
    txtptr = buf.mtext + n;
    if (n > SIM_MSG_QUEUE_BUFSIZE) {
        return -1;
    }
    sprintf(txtptr, "%s=%d", SIM_MSG_WATCHDOG_ACTION, SAHPI_WA_RESET);
    n += strlen(txtptr) + 1;
    txtptr = buf.mtext + n;
    if (n > SIM_MSG_QUEUE_BUFSIZE) {
        return -1;
    }
    sprintf(txtptr, "%s=%d", SIM_MSG_WATCHDOG_PRETIMER_ACTION, SAHPI_WPI_NONE);
    n += strlen(txtptr) + 1;
    txtptr = buf.mtext + n;
    if (n > SIM_MSG_QUEUE_BUFSIZE) {
        return -1;
    }
    sprintf(txtptr, "%s=%d", SIM_MSG_WATCHDOG_TIMER_USE, SAHPI_WTU_NONE);
    n += strlen(txtptr) + 1;
    txtptr = buf.mtext + n;
    if (n > SIM_MSG_QUEUE_BUFSIZE) {
        return -1;
    }
    *txtptr = '\0'; // terminate buf with a zero-length string
    n++;

    /* send the msg */
    rc = msgsnd(msgqueid, &buf, n, 0);
    if (rc) {
        return -1;
    }

    return 0;
}


static int inject_software_event (char *handler_name)
{
    key_t ipckey;
    int msgqueid;
    SIM_MSG_QUEUE_BUF buf;
    size_t n = 0;  // size of the data in the msg buf
    char *txtptr = buf.mtext;
    int rc;

    printf("\nSTATUS: Injecting a software event...\n");

    /* get the  queue */
    ipckey = ftok(".", SIM_MSG_QUEUE_KEY);
    msgqueid = msgget(ipckey, 0660);
    if (msgqueid == -1) {
        return -1;
    }

    /* fill out the message */
    buf.mtype = SIM_MSG_SW_EVENT;
    *txtptr = '\0';
    sprintf(txtptr, "%s=%s", SIM_MSG_HANDLER_NAME, handler_name);
    n += strlen(txtptr) + 1;
    txtptr = buf.mtext + n;
    if (n > SIM_MSG_QUEUE_BUFSIZE) {
        return -1;
    }
    sprintf(txtptr, "%s=%d", SIM_MSG_RESOURCE_ID, 1);
    n += strlen(txtptr) + 1;
    txtptr = buf.mtext + n;
    if (n > SIM_MSG_QUEUE_BUFSIZE) {
        return -1;
    }
    sprintf(txtptr, "%s=%d", SIM_MSG_EVENT_SEVERITY, SAHPI_MINOR);
    n += strlen(txtptr) + 1;
    txtptr = buf.mtext + n;
    if (n > SIM_MSG_QUEUE_BUFSIZE) {
        return -1;
    }
    sprintf(txtptr, "%s=%d", SIM_MSG_SW_MID, 123);
    n += strlen(txtptr) + 1;
    txtptr = buf.mtext + n;
    if (n > SIM_MSG_QUEUE_BUFSIZE) {
        return -1;
    }
    sprintf(txtptr, "%s=%d", SIM_MSG_SW_EVENT_TYPE, SAHPI_HPIE_OTHER);
    n += strlen(txtptr) + 1;
    txtptr = buf.mtext + n;
    if (n > SIM_MSG_QUEUE_BUFSIZE) {
        return -1;
    }
    sprintf(txtptr, "%s=%s", SIM_MSG_SW_EVENT_DATA, "This is a test");
    n += strlen(txtptr) + 1;
    txtptr = buf.mtext + n;
    if (n > SIM_MSG_QUEUE_BUFSIZE) {
        return -1;
    }
    *txtptr = '\0'; // terminate buf with a zero-length string
    n++;

    /* send the msg */
    rc = msgsnd(msgqueid, &buf, n, 0);
    if (rc) {
        return -1;
    }

    return 0;


}



static int inject_oem_event (char *handler_name)
{
    key_t ipckey;
    int msgqueid;
    SIM_MSG_QUEUE_BUF buf;
    size_t n = 0;  // size of the data in the msg buf
    char *txtptr = buf.mtext;
    int rc;

    printf("\nSTATUS: Injecting an oem event...\n");

    /* get the  queue */
    ipckey = ftok(".", SIM_MSG_QUEUE_KEY);
    msgqueid = msgget(ipckey, 0660);
    if (msgqueid == -1) {
        return -1;
    }

    /* fill out the message */
    buf.mtype = SIM_MSG_OEM_EVENT;
    *txtptr = '\0';
    sprintf(txtptr, "%s=%s", SIM_MSG_HANDLER_NAME, handler_name);
    n += strlen(txtptr) + 1;
    txtptr = buf.mtext + n;
    if (n > SIM_MSG_QUEUE_BUFSIZE) {
        return -1;
    }
    sprintf(txtptr, "%s=%d", SIM_MSG_RESOURCE_ID, 1);
    n += strlen(txtptr) + 1;
    txtptr = buf.mtext + n;
    if (n > SIM_MSG_QUEUE_BUFSIZE) {
        return -1;
    }
    sprintf(txtptr, "%s=%d", SIM_MSG_EVENT_SEVERITY, SAHPI_MINOR);
    n += strlen(txtptr) + 1;
    txtptr = buf.mtext + n;
    if (n > SIM_MSG_QUEUE_BUFSIZE) {
        return -1;
    }
    sprintf(txtptr, "%s=%d", SIM_MSG_OEM_MID, 123);
    n += strlen(txtptr) + 1;
    txtptr = buf.mtext + n;
    if (n > SIM_MSG_QUEUE_BUFSIZE) {
        return -1;
    }
    sprintf(txtptr, "%s=%s", SIM_MSG_OEM_EVENT_DATA, "This is a test");
    n += strlen(txtptr) + 1;
    txtptr = buf.mtext + n;
    if (n > SIM_MSG_QUEUE_BUFSIZE) {
        return -1;
    }
    *txtptr = '\0'; // terminate buf with a zero-length string
    n++;

    /* send the msg */
    rc = msgsnd(msgqueid, &buf, n, 0);
    if (rc) {
        return -1;
    }

    return 0;

}



static int inject_user_event (char *handler_name)
{
    key_t ipckey;
    int msgqueid;
    SIM_MSG_QUEUE_BUF buf;
    size_t n = 0;  // size of the data in the msg buf
    char *txtptr = buf.mtext;
    int rc;

    printf("\nSTATUS: Injecting a user event...\n");

    /* get the  queue */
    ipckey = ftok(".", SIM_MSG_QUEUE_KEY);
    msgqueid = msgget(ipckey, 0660);
    if (msgqueid == -1) {
        return -1;
    }

    /* fill out the message */
    buf.mtype = SIM_MSG_USER_EVENT;
    *txtptr = '\0';
    sprintf(txtptr, "%s=%s", SIM_MSG_HANDLER_NAME, handler_name);
    n += strlen(txtptr) + 1;
    txtptr = buf.mtext + n;
    if (n > SIM_MSG_QUEUE_BUFSIZE) {
        return -1;
    }
    sprintf(txtptr, "%s=%d", SIM_MSG_RESOURCE_ID, 1);
    n += strlen(txtptr) + 1;
    txtptr = buf.mtext + n;
    if (n > SIM_MSG_QUEUE_BUFSIZE) {
        return -1;
    }
    sprintf(txtptr, "%s=%d", SIM_MSG_EVENT_SEVERITY, SAHPI_MINOR);
    n += strlen(txtptr) + 1;
    txtptr = buf.mtext + n;
    if (n > SIM_MSG_QUEUE_BUFSIZE) {
        return -1;
    }
    sprintf(txtptr, "%s=%s", SIM_MSG_USER_EVENT_DATA, "This is a test");
    n += strlen(txtptr) + 1;
    txtptr = buf.mtext + n;
    if (n > SIM_MSG_QUEUE_BUFSIZE) {
        return -1;
    }
    *txtptr = '\0'; // terminate buf with a zero-length string
    n++;

    /* send the msg */
    rc = msgsnd(msgqueid, &buf, n, 0);
    if (rc) {
        return -1;
    }

    return 0;
}

static void print_usage (void)
{
   printf("Usage is ./executable -n your_handler_name*\n");
   printf("*Note: your_handler_name is the handler_name specified in openhpi.conf\n\n");
}   
   

int main(int argc, char **argv)
{
	int retc = 0, c;
	char *handler_name = NULL;
	char selection;
	
	if (argc < 3) { printf("Too few arguments\n"); print_usage(); exit(1); }
	else if (argc > 3) { printf("Too many arguments\n"); print_usage(); exit(1); }
	
	while ((c = getopt (argc, argv, "n:s")) != EOF) {
	    switch (c) {

	 	case 'n':
			handler_name = optarg;
			break;
			
	    default:
			exit(1);
	      	break;
	    }
	}	
	
	while (get_user_input(&selection, topmenu)) {

		switch (selection) {
		
                case RESOURCE:
                        get_user_input(&selection, resource_menu);

                        switch (selection) {
                        case 1:
                        case 2:
                                inject_resource_event(handler_name, selection);
                                break;
                        case 3:
                                inject_resource_add_event(handler_name);
                                break;
                        default:
                                break;

                        }

                        break;
				
		case DOMAIN:
                        get_user_input(&selection, domain_menu);
			inject_domain_event(handler_name, selection);
			break;
							
		case SENSOR:
			inject_sensor_event(handler_name);
			break;

		case SENSOR_ENABLE_CHANGE:
			inject_sensorec_event(handler_name);
			break;
			
		case HOTSWAP:
			inject_hotswap_event(handler_name);
			break;
			
		case WATCHDOG:
			inject_watchdog_event(handler_name);
			break;
			
		case SOFTWARE:
			inject_software_event(handler_name);
			break;
			
		case Oem:
			inject_oem_event(handler_name);
			break;
			
		case USER:
			inject_user_event(handler_name);
			break;
		
		default:
			printf("Unrecognized selection\n");
			break;
		}
		
	}
	 

	return 0;
}
