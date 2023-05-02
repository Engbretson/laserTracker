< envPaths.windows-x64

< settings.iocsh 

dbLoadDatabase("../../dbd/windows-x64/iocxxxWin64.dbd")
iocxxxWin64_registerRecordDeviceDriver(pdbbase)

LTAt403Configure("At403Simulator")
#LTAt403Configure("At930Simulator")

dbLoadRecords("$(TOP)/db/laserTracker.db","P=$(PREFIX),R=scope1:,PORT=At403Simulator,ADDR=0,TIMEOUT=1")
#dbLoadRecords("$(TOP)/db/laserTracker.db","P=$(PREFIX),R=scope1:,PORT=At930Simulator,ADDR=0,TIMEOUT=1")

#asynSetTraceMask("At403Simulator",0,0xff)
asynSetTraceIOMask("At403Simulator",0,0x2)

#asynSetTraceMask("At930Simulator",0,0xff)
#asynSetTraceIOMask("At930Simulator",0,0x2)

 < common.iocsh


iocInit()

# write all the PV names to a local file
dbl > dbl-all.txt

# Diagnostic: CA links in all records
dbcar(0,1)

# print the time our boot was finished
date
