#
# (C) Copyright IBM Corp. 2005
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  This
# file and program are licensed under a BSD style license.  See
# the Copying file included with the OpenHPI distribution for
# full licensing terms.
#
# Authors:
#   David Judkovics  <djudkovi@us.ibm.com>
#   Daniel de Araujo <ddearauj@us.ibm.com>
#

MAINTAINERCLEANFILES = Makefile.in

INCLUDES 		= -I$(top_srcdir)/include
 
AM_CFLAGS 		= @CFLAGS@

bin_PROGRAMS		= hpiSubagent

SUBDIRS                 = 

hpiSubagent_SOURCES	= alarm.c \
			hpiCheckIndice.c \
			session_info.c \
			hash_utils.c \
			hpiSubagent.c \
			hpiAnnunciatorMapping.c \
			hpiDomainAlarmMapping.c \
			hpiEventThread.c \
			saHpiDomainInfoTable.c \
			saHpiDomainReferenceTable.c \
			saHpiResourceTable.c \
			saHpiRdrTable.c \
			saHpiCtrlDigitalTable.c \
			saHpiCtrlDiscreteTable.c \
			saHpiCtrlAnalogTable.c \
			saHpiCtrlStreamTable.c \
			saHpiCtrlTextTable.c \
			saHpiCtrlOemTable.c \
			saHpiSensorTable.c \
			saHpiCurrentSensorStateTable.c \
			saHpiSensorReadingMaxTable.c \
			saHpiSensorReadingMinTable.c \
			saHpiSensorReadingNominalTable.c \
			saHpiSensorReadingNormalMaxTable.c \
			saHpiSensorReadingNormalMinTable.c \
			saHpiSensorThdLowCriticalTable.c \
			saHpiSensorThdLowMajorTable.c \
			saHpiSensorThdLowMinorTable.c \
			saHpiSensorThdUpCriticalTable.c \
			saHpiSensorThdUpMajorTable.c \
			saHpiSensorThdUpMinorTable.c \
			saHpiSensorThdPosHysteresisTable.c \
			saHpiSensorThdNegHysteresisTable.c \
			saHpiInventoryTable.c \
			saHpiWatchdogTable.c \
			saHpiAnnunciatorTable.c \
			saHpiAreaTable.c \
			saHpiFieldTable.c \
			saHpiEventTable.c \
			saHpiResourceEventTable.c \
			saHpiDomainEventTable.c \
			saHpiEventLogInfoTable.c \
			saHpiEventLogTable.c \
			saHpiResourceEventLogTable.c \
			saHpiSensorEventLogTable.c \
			saHpiSensorEventTable.c \
			saHpiOemEventTable.c \
			saHpiHotSwapEventTable.c \
			saHpiWatchdogEventTable.c \
			saHpiSoftwareEventTable.c \
			saHpiSensorEnableChangeEventTable.c \
			saHpiUserEventTable.c \
                        saHpiHotSwapEventLogTable.c \
                        saHpiWatchdogEventLogTable.c \
			saHpiSoftwareEventLogTable.c \
			saHpiOemEventLogTable.c \
			saHpiUserEventLogTable.c \
			saHpiSensorEnableChangeEventLogTable.c \
			saHpiDomainEventLogTable.c \
			saHpiHotSwapTable.c \
			saHpiDomainAlarmTable.c \
			saHpiAutoInsertTimeoutTable.c \
			saHpiAnnouncementTable.c \
			saHpiAnnouncementEventLogTable.c \
			saHpiAdministration.c \
			hpiLock.c								
				


clean:
	rm -f *~
	rm -f *.o
	
#			hpiB0101.c \
#			uid_utils.c


