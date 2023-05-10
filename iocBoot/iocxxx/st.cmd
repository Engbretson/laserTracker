< envPaths.windows-x64

epicsEnvSet("ENGINEER","Engbretson")
epicsEnvSet("GROUP","8-id-e")
epicsEnvSet("LOCATION","8-id-e")
epicsEnvSet("CAMERA_ID","AT403 394712 164.54.116.53")
epicsEnvSet("CAMERA_ID","AT930 750529 192.168.0.1")

epicsEnvSet("IOC","ioc433LT")
epicsEnvSet("PREFIX","433LT:")

dbLoadDatabase("../../dbd/windows-x64/iocxxxWin64.dbd")
iocxxxWin64_registerRecordDeviceDriver(pdbbase)

#LTAt403Configure("At403Simulator")
LTAt403Configure("192.168.0.1")

dbLoadRecords("$(TOP)/db/laserTracker.db","P=$(PREFIX),R=scope1:,PORT=At403Simulator,ADDR=0,TIMEOUT=1")

#asynSetTraceMask("At403Simulator",0,0xff)
asynSetTraceIOMask("At403Simulator",0,0x2)


### Load database record for alive heartbeating support.
# IOCNM is name of IOC, RHOST specifies the remote server accepting hearbeats
dbLoadRecords("$(ALIVE)/aliveApp/Db/alive.db", "P=$(PREFIX),IOCNM=$(IOC),RHOST=164.54.100.11")
dbLoadRecords("$(ALIVE)/aliveApp/Db/aliveMSGCalc.db", "P=$(PREFIX)")

iocInit()

# write all the PV names to a local file
dbl > dbl-all.txt

# Diagnostic: CA links in all records
dbcar(0,1)

# print the time our boot was finished
date
