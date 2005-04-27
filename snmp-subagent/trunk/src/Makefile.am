MAINTAINERCLEANFILES = Makefile.in

INCLUDES 		= -I$(top_srcdir)/include
 
AM_CFLAGS 		= @CFLAGS@

bin_PROGRAMS		= hpiSubagent

SUBDIRS                 = 


hpiSubagent_SOURCES	= alarm.c \
			hpiB0101.c \
			hpiCheckIndice.c \
			session_info.c \
			hpiSubagent.c \
			saHpiDomainInfoTable.c \
			saHpiDomainReferenceTable.c \
			saHpiResourceTable.c \
			saHpiRdrTable.c \
			saHpiCtrlDigitalTable.c \
			saHpiCtrlDiscreteTable.c \
			saHpiCtrlAnalogTable.c 


hpiSubagent_LDADD	= 
hpiSubagent_LDFLAGS	= 

clean:
	rm -f *~
	rm -f *.o
	
#			uid_utils.c \
#			saHpiAnnouncementEventLogTable.c \
#			saHpiAnnouncementTable.c \
#			saHpiAnnunciatorTable.c \
#			saHpiAutoInsertTimeoutTable.c \
#			saHpiCtrlOemTable.c \
#			saHpiCtrlStreamTable.c \
#			saHpiCtrlTextTable.c \
#			saHpiCurrentSensorStateTable.c \
#			saHpiDomainAlarmTable.c \
#			saHpiDomainEventLogTable.c \
#			saHpiDomainEventTable.c \
#			saHpiEventLogInfoTable.c \
#			saHpiEventLogTable.c \
#			saHpiEventTable.c \
#			saHpiFieldTable.c \
#			saHpiHotSwapEventLogTable.c \
#			saHpiHotSwapEventTable.c \
#			saHpiHotSwapTable.c \
#			saHpiInventoryTable.c \
#			saHpiOEMEventLogTable.c \
#			saHpiOEMEventTable.c \
#			saHpiResourceEventLogTable.c \
#			saHpiResourceEventTable.c \
#			saHpiSensorEnableChangeEventLogTable.c \
#			saHpiSensorEnableChangeEventTable.c \
#			saHpiSensorEventLogTable.c \
#			saHpiSensorEventTable.c \
#			saHpiSensorReadingMaxTable.c \
#			saHpiSensorReadingMinTable.c \
#			saHpiSensorReadingNominalTable.c \
#			saHpiSensorReadingNormalMaxTable.c \
#			saHpiSensorReadingNormalMinTable.c \
#			saHpiSensorTable.c \
#			saHpiSensorThdLowCriticalTable.c \
#			saHpiSensorThdLowMajorTable.c \
#			saHpiSensorThdLowMinorTable.c \
#			saHpiSensorThdNegHysteresisTable.c \
#			saHpiSensorThdPosHysteresisTable.c \
#			saHpiSensorThdUpCriticalTable.c \
#			saHpiSensorThdUpMajorTable.c \
#			saHpiSensorThdUpMinorTable.c \
#			saHpiSoftwareEventLogTable.c \
#			saHpiSoftwareEventTable.c \
#			saHpiUserEventLogTable.c \
#			saHpiUserEventTable.c \
#			saHpiWatchdogEventLogTable.c \
#			saHpiWatchdogEventTable.c \
#			saHpiWatchdogTable.c 
