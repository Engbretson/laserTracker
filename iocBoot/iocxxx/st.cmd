< envPaths.windows-x64

epicsEnvSet("ENGINEER","Engbretson")
epicsEnvSet("GROUP","8-id-e")
epicsEnvSet("LOCATION","8-id-e")
#epicsEnvSet("CAMERA_ID","AT403 394712 164.54.116.53")
epicsEnvSet("CAMERA_ID","AT930 750529 164.54.116.53")

epicsEnvSet("IOC","ioc433LT")
epicsEnvSet("PREFIX","433LT:")

#epicsEnvSet("PORT","At403Simulator")
#epicsEnvSet("PORT","At930Simulator")
epicsEnvSet("PORT","164.54.116.53")

epicsEnvSet("XSIZE","2560")
epicsEnvSet("YSIZE","1920")
epicsEnvSet("NELEMENTS","19660800")
# The search path for database files
epicsEnvSet("EPICS_DB_INCLUDE_PATH", "$(ADCORE)/db")

# The queue size for all plugins
epicsEnvSet("QSIZE",  "20")
# The maximum image width; used to set the maximum size for this driver and for row profiles in the NDPluginStats plugin
# The maximum number of time series points in the NDPluginStats plugin
epicsEnvSet("NCHANS", "2048")
# The maximum number of frames buffered in the NDPluginCircularBuff plugin
epicsEnvSet("CBUFFS", "500")
# The maximum number of threads for plugins which can run in multiple threads
epicsEnvSet("MAX_THREADS", "8")
# The search path for database files


dbLoadDatabase("../../dbd/windows-x64/iocxxxWin64.dbd")
iocxxxWin64_registerRecordDeviceDriver(pdbbase)

#LTAt403Configure("At403Simulator")
#LTAt403Configure($(PORT))

LTAt930Configure("$(PORT)", $(XSIZE), $(YSIZE), 1, 0, 0)

dbLoadRecords("$(TOP)/db/laserTracker.db","P=$(PREFIX),R=scope1:,PORT=$(PORT),ADDR=0,TIMEOUT=1")

# Create a standard arrays plugin
NDStdArraysConfigure("Image1", 5, 0, "$(PORT)", 0, 0)
# Use this line for 8-bit data only
#dbLoadRecords("$(ADCORE)/db/NDStdArrays.template", "P=$(PREFIX),R=image1:,PORT=Image1,ADDR=0,TIMEOUT=1,NDARRAY_PORT=$(PORT),TYPE=Int8,FTVL=CHAR,NELEMENTS=$(NELEMENTS)")
# Use this line for 8-bit or 16-bit data
#dbLoadRecords("$(ADCORE)/db/NDStdArrays.template", "P=$(PREFIX),R=image1:,PORT=Image1,ADDR=0,TIMEOUT=1,NDARRAY_PORT=$(PORT),TYPE=Int16,FTVL=SHORT,NELEMENTS=$(NELEMENTS)")
# Lasrer Tracker Image data is 24 bit, so I assume that I need Int32 to hold it all
dbLoadRecords("$(ADCORE)/db/NDStdArrays.template", "P=$(PREFIX),R=image1:,PORT=Image1,ADDR=0,TIMEOUT=1,NDARRAY_PORT=$(PORT),TYPE=Int32,FTVL=LONG,NELEMENTS=$(NELEMENTS)")

# Load all other plugins using commonPlugins.cmd
# < $(ADCORE)/iocBoot/commonPlugins.cmd
 < ./commonPlugins.cmd


#asynSetTraceMask($(PORT),0,0xff)
asynSetTraceIOMask($(PORT),0,0x2)


### Load database record for alive heartbeating support.
# IOCNM is name of IOC, RHOST specifies the remote server accepting hearbeats
##dbLoadRecords("$(ALIVE)/aliveApp/Db/alive.db", "P=$(PREFIX),IOCNM=$(IOC),RHOST=164.54.100.11")
##dbLoadRecords("$(ALIVE)/aliveApp/Db/aliveMSGCalc.db", "P=$(PREFIX)")

iocInit()

# save things every thirty seconds
create_monitor_set("auto_settings.req", 30, "P=$(PREFIX)")

# write all the PV names to a local file
dbl > dbl-all.txt

# Diagnostic: CA links in all records
dbcar(0,1)

# print the time our boot was finished
date

