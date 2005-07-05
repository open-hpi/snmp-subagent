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
			saHpiAnnunciatorTable.c \
			saHpiInventoryTable.c \
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
			saHpiOEMEventTable.c \
			saHpiHotSwapEventTable.c \
			saHpiWatchdogEventTable.c \
			saHpiSoftwareEventTable.c \
			saHpiSensorEnableChangeEventTable.c \
			saHpiUserEventTable.c \
                        saHpiHotSwapEventLogTable.c \
                        saHpiWatchdogEventLogTable.c \
			saHpiSoftwareEventLogTable.c \
			saHpiOEMEventLogTable.c \
			saHpiUserEventLogTable.c \
			saHpiSensorEnableChangeEventLogTable.c

					
			
			


hpiSubagent_LDADD	= 
hpiSubagent_LDFLAGS	= 

clean:
	rm -f *~
	rm -f *.o
	
#			hpiB0101.c \
#			uid_utils.c \
#			saHpiAnnouncementEventLogTable.c \
#			saHpiAnnouncementTable.c \
#			saHpiAutoInsertTimeoutTable.c \
#			saHpiDomainAlarmTable.c \
#			saHpiDomainEventLogTable.c \
#			saHpiHotSwapTable.c
