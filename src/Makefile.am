MAINTAINERCLEANFILES = Makefile.in

INCLUDES 		= -I$(top_srcdir)/include 

#COMMONLIBS		= $(top_builddir)/src/libopenhpi.la

AM_CFLAGS 		= @CFLAGS@

bin_PROGRAMS		= hpiSubagent

hpiSubagent_SOURCES	= saHpiHotSwapTable.c saHpiEventTable.c saHpiSystemEventLogTable.c saHpiWatchdogTable.c saHpiSensorTable.c saHpiInventoryTable.c saHpiCtrlTable.c hpiSubagent.c saHpiTable.c saHpiRdrTable.c  alarm.c
hpiSubagent_LDADD	= $(COMMONLIBS)
hpiSubagent_LDFLAGS	= 

clean:
	rm -f *~

