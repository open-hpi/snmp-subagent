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
 *   David Judkovics <djudkovi@us.ibm.com>
 *
 *
 */

/**************************/
/* saHpiAnnouncementTable */    
/**************************/
int saHpiDomainId_check_index(long val) 
{
	return 0;
}

int saHpiResourceId_check_index(long val)
{
	return 0;
}

int saHpiDomainAlarmId_check_index(long val)
{
	return 0;
}

int saHpiEntryId_check_index(long val)
{
	return 0;
}
  
/**************************/
/* saHpiAnnunciatorTable */    
/**************************/
int saHpiAnnunciatorNum_check_index(long val)
{
	return 0;
}

/**************************/
/* saHpiAreaTable */    
/**************************/
int saHpiAreaId_check_index(long val)
{
	return 0;
}

/**************************/
/* saHpiCtrlTable	 */    
/**************************/
int saHpiCtrlNum_check_index(long val)
{
	return 0;
}

/**************************/
/* saHpiDomainAlarmTable  */    
/**************************/
int saHpiDomainAlarmId_check_index(long val)
{
	return 0;
}

/**************************/
/* saHpiEventDomainTable  */    
/**************************/
int saHpiEventSeverity_check_index(long val)
{
	return 0;
}

int saHpiEventHistorical_check_index(long val)
{
	return 0;
}

int saHpiDomainEventTimestamp_check_index(netsnmp_variable_list var_saHpiDomainEventTimestamp)
{
	snmp_log(LOG_ERR, "saHpiDomainEventTimestamp_check_index NEED TO INVESTIGATE A BETTER CHECK!\n" );

	return 0;
}



 
