MAINTAINERCLEANFILES = Makefile.in

INCLUDES 		= -I$(top_srcdir)/include 

AM_CFLAGS 		= @CFLAGS@

bin_PROGRAMS		= hpiSubagent

SUBDIRS                 = 

hpiSubagent_SOURCES	= hpiB.c \
			saHpiAnnouncementTable.c \
			saHpiAnnunciatorTable.c \
			saHpiAreaTable.c \
			saHpiAutoTimeOutTable.c \
			saHpiCtrlTable.c \
			saHpiDomainAlarmTable.c \
			saHpiDomainEventTable.c \
			saHpiDomainInfoTable.c \
			saHpiDomainReferenceTable.c \
			saHpiEventLogInfoTable.c \
			saHpiEventLogTable.c \
			saHpiEventTable.c \
			saHpiFieldTable.c \
			saHpiHotSwapEventTable.c \
			saHpiHotSwapTable.c \
			saHpiInventoryTable.c \
			saHpiOEMEventTable.c \
			saHpiRdrTable.c \
			saHpiResourceEventTable.c \
			saHpiResourceTable.c \
			saHpiSensorEnableChangeEventTable.c \
			saHpiSensorEventTable.c \
			saHpiSensorReadingCurrentTable.c \
			saHpiSensorReadingMaxTable.c \
			saHpiSensorReadingMinTable.c \
			saHpiSensorReadingNominalTable.c \
			saHpiSensorReadingNormalMaxTable.c \
			saHpiSensorReadingNormalMinTable.c \
			saHpiSensorTable.c \
			saHpiSensorThdLowCriticalTable.c \
			saHpiSensorThdLowMajorTable.c \
			saHpiSensorThdLowMinorTable.c \
			saHpiSensorThdNegHysteresisTable.c \
			saHpiSensorThdPosHysteresisTable.c \
			saHpiSensorThdUpCriticalTable.c \
			saHpiSensorThdUpMajorTable.c \
			saHpiSensorThdUpMinorTable.c \
			saHpiSoftwareEventTable.c \
			saHpiUserEventTable.c \
			saHpiWatchdogEventTable.c \
			saHpiWatchdogTable.c

hpiSubagent_LDADD	= 
hpiSubagent_LDFLAGS	= 

clean:
	rm -f *~
	rm -f *.o

