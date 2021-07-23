# Windows startup script

< envPaths

epicsEnvSet("IOC","ioc433LT1")
epicsEnvSet("PREFIX","lom433:")
epicsEnvSet("R","dev1:")

epicsEnvSet("PORT","At403Simulator")


dbLoadDatabase("../../dbd/iocxxxWin64.dbd")
iocxxxWin64_registerRecordDeviceDriver(pdbbase)
dbLoadRecords("$(TOP)/db/laserTrackerSupport.template", "P=$(PREFIX),PORT=$(PORT),R=dev1:")


LTAT403Config("At403Simulator")

#dbLoadRecords("$(TOP)/db/laserTrackerSupport.template", "P=$(PREFIX),PORT=$(PORT),R=dev1:")

### Load database record for alive heartbeating support.
# IOCNM is name of IOC, RHOST specifies the remote server accepting hearbeats
dbLoadRecords("$(ALIVE)/aliveApp/Db/alive.db", "P=$(PREFIX),IOCNM=$(IOC),RHOST=164.54.100.11")
dbLoadRecords("$(ALIVE)/aliveApp/Db/aliveMSGCalc.db", "P=$(PREFIX)")


###############################################################################
iocInit
###############################################################################

# write all the PV names to a local file
dbl > dbl-all.txt

# Diagnostic: CA links in all records
dbcar(0,1)

# print the time our boot was finished
date
