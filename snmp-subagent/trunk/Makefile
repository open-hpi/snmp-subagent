CC=gcc

TARGETS=hpiSubagent
OBJ= saHpiSystemEventLogTable.o saHpiWatchdogTable.o saHpiSensorTable.o saHpiInventoryTable.o saHpiCtrlTable.o hpiSubagent.o saHpiTable.o saHpiRdrTable.o epath_utils.o alarm.o

CFLAGS=-I. `net-snmp-config --cflags` `glib-config --cflags` -ggdb -I/usr/local/include/openhpi
BUILDAGENTLIBS=`net-snmp-config --agent-libs` `glib-config --libs` -lopenhpi

# shared library flags (assumes gcc)
DLFLAGS=-fPIC -shared

all: $(TARGETS)

hpiSubagent: $(OBJ)
	$(CC) -o hpiSubagent $(OBJ)  $(BUILDAGENTLIBS)

clean:
	rm $(TARGETS) $(OBJ)

epath_utils.so: epath_utils.c Makefile
	$(CC) $(CFLAGS) $(DLFLAGS) -c -o epath_utils.o epath_utils.c
	$(CC) $(CFLAGS) $(DLFLAGS) -o epath_utils.so epath_utils.o

hpiSubagent.so: hpiSubagent.c Makefile
	$(CC) $(CFLAGS) $(DLFLAGS) -c -o hpiSubagent.o hpiSubagent.c
	$(CC) $(CFLAGS) $(DLFLAGS) -o hpiSubagent.so hpiSubagent.o

saHpiSystemEventLogTable.so: saHpiSystemEventLogTable.c Makefile
	$(CC) $(CFLAGS) $(DLFLAGS) -c -o saHpiSystemEventLogTable.o saHpiSystemEventLogTable.c
	$(CC) $(CFLAGS) $(DLFLAGS) -o saHpiSystemEventLogTable.so saHpiSystemEventLogTable.o
saHpiWatchdogTable.so: saHpiWatchdogTable.c Makefile
	$(CC) $(CFLAGS) $(DLFLAGS) -c -o saHpiWatchdogTable.o saHpiWatchdogTable.c
	$(CC) $(CFLAGS) $(DLFLAGS) -o saHpiWatchdogTable.so saHpiWatchdogTable.o
saHpiInventoryTable.so: saHpiInventoryTable.c Makefile
	$(CC) $(CFLAGS) $(DLFLAGS) -c -o saHpiInventoryTable.o saHpiInventoryTable.c
	$(CC) $(CFLAGS) $(DLFLAGS) -o saHpiInventoryTable.so saHpiInventoryTable.o
saHpiCtrlTable.so: saHpiCtrlTable.c Makefile
	$(CC) $(CFLAGS) $(DLFLAGS) -c -o saHpiCtrlTable.o saHpiCtrlTable.c
	$(CC) $(CFLAGS) $(DLFLAGS) -o saHpiCtrlTable.so saHpiCtrlTable.o
saHpiTable.so: saHpiTable.c Makefile
	$(CC) $(CFLAGS) $(DLFLAGS) -c -o saHpiTable.o saHpiTable.c
	$(CC) $(CFLAGS) $(DLFLAGS) -o saHpiTable.so saHpiTable.o

saHpiRdrTable.so: saHpiRdrTable.c Makefile
	$(CC) $(CFLAGS) $(DLFLAGS) -c -o saHpiRdrTable.o saHpiRdrTable.c
	$(CC) $(CFLAGS) $(DLFLAGS) -o saHpiRdrTable.so saHpiRdrTable.o

saHpiSensorTable.so: saHpiSensorTable.c Makefile
	$(CC) $(CFLAGS) $(DLFLAGS) -c -o saHpiSensorTable.o saHpiSensorTable.c
	$(CC) $(CFLAGS) $(DLFLAGS) -o saHpiSensorTable.so saHpiSensorTable.o
alarm.so: alarm.c Makefile
	$(CC) $(CFLAGS) $(DLFLAGS) -c -o alarm.o alarm.c
	$(CC) $(CFLAGS) $(DLFLAGS) -o alarm.so alarm.o
