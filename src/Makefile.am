MAINTAINERCLEANFILES = Makefile.in

INCLUDES 		= -I$(top_srcdir)/include 

AM_CFLAGS 		= @CFLAGS@

bin_PROGRAMS		= hpiSubagent

SUBDIRS                 = t

hpiSubagent_SOURCES	= saHpiSensorReadingCurrentTable.c \
	saHpiSensorReadingMaxTable.c saHpiSensorReadingMinTable.c \
	saHpiSensorReadingNormalMaxTable.c saHpiSensorReadingNormalMinTable.c \
	saHpiSensorReadingNominalTable.c \
	saHpiHotSwapTable.c \
	saHpiEventTable.c saHpiSystemEventLogTable.c saHpiWatchdogTable.c \
	saHpiSensorTable.c saHpiInventoryTable.c saHpiCtrlTable.c hpiSubagent.c \
	saHpiTable.c saHpiRdrTable.c alarm.c

hpiSubagent_LDADD	= 
hpiSubagent_LDFLAGS	= 

clean:
	rm -f *~
	rm -f *.o

