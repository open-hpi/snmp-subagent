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
*     Daniel de Araujo <ddearauj@us.ibm.com>
*/

typedef enum {
	RESOURCE = 1,
	DOMAIN,
	SENSOR,
	SENSOR_ENABLE_CHANGE,
	HOTSWAP, 
	WATCHDOG,
	SOFTWARE,
	OEM,
	USER
}event_types;

typedef enum {
        RESOURCE_FAILURE = 1,
        RESOURCE_RESTORED,
        RESOURCE_ADDED
}resource_event_types;




/* function prototypes */
static void print_usage(void);

static int get_user_input (char *input, char *menu);

static int inject_resource_event (char *handler_name, char selection);
static int inject_domain_event   (char *handler_name, char selection);
static int inject_sensor_event   (char *handler_name);
static int inject_sensorec_event (char *handler_name);
static int inject_hotswap_event  (char *handler_name);
static int inject_watchdog_event (char *handler_name);
static int inject_software_event (char *handler_name);
static int inject_oem_event      (char *handler_name);
static int inject_user_event     (char *handler_name);
