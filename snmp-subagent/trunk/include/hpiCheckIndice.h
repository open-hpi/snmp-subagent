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
int saHpiDomainId_check_index(long val);
int saHpiResourceId_check_index(long val);
int saHpiDomainAlarmId_check_index(long val);
int saHpiEntryId_check_index(long val);
  
/**************************/
/* saHpiAnnunciatorTable */    
/**************************/
int saHpiAnnunciatorNum_check_index(long val);

/**************************/
/* saHpiAreaTable */    
/**************************/
int saHpiAreaId_check_index(long val);

/**************************/
/* saHpiCtrlTable	 */    
/**************************/
int saHpiCtrlNum_check_index(long val);

/**************************/
/* saHpiDomainAlarmTable  */    
/**************************/
int saHpiDomainAlarmId_check_index(long val);

/**************************/
/* saHpiEventDomainTable  */    
/**************************/
int saHpiEventSeverity_check_index(long val);
int saHpiEventHistorical_check_index(long val);
int saHpiDomainEventTimestamp_check_index(netsnmp_variable_list var_saHpiDomainEventTimestamp);
 


