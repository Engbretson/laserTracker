# This is an example file for creating plugins
# It uses the following environment variable macros
# Many of the parameters defined in this file are also in commonPlugins_settings.req so if autosave is being
# use the autosave value will replace the value passed to this file.

# $(PREFIX)      Prefix for all records
# $(PORT)        The port name for the detector.  In autosave.
# $(QSIZE)       The queue size for all plugins.  In autosave.
# $(XSIZE)       The maximum image width; used to set the maximum size for row profiles in the NDPluginStats plugin and 1-D FFT
#                   profiles in NDPluginFFT.
# $(YSIZE)       The maximum image height; used to set the maximum size for column profiles in the NDPluginStats plugin
# $(NCHANS)      The maximum number of time series points in the NDPluginStats, NDPluginROIStats, and NDPluginAttribute plugins
# $(CBUFFS)      The maximum number of frames buffered in the NDPluginCircularBuff plugin
# $(MAX_THREADS) The maximum number of threads for plugins which can run in multiple threads. Defaults to 5.


set_requestfile_path("./")
set_requestfile_path("$(ADCORE)/db")
set_requestfile_path("$(ADCORE)/iocBoot")
set_requestfile_path("$(AUTOSAVE)/db")
set_savefile_path("./autosave")
set_pass0_restoreFile("auto_settings.sav")
set_pass1_restoreFile("auto_settings.sav")
save_restoreSet_status_prefix("$(PREFIX)")
dbLoadRecords("$(AUTOSAVE)/db/save_restoreStatus.db", "P=$(PREFIX)")
dbLoadRecords("$(AUTOSAVE)/db/configMenu.db", "P=$(PREFIX), CONFIG=ADAutoSave")

# Load NDPluginPva plugin
NDPvaConfigure("PVA1", $(QSIZE), 0, "$(PORT)", 0, $(PREFIX)Pva1:Image, 0, 0, 0)
dbLoadRecords("NDPva.template",  "P=$(PREFIX),R=Pva1:, PORT=PVA1,ADDR=0,TIMEOUT=1,NDARRAY_PORT=$(PORT)")
#Must start PVA server if this is enabled
startPVAServer

# Optional: load devIocStats records (requires DEVIOCSTATS module)
dbLoadRecords("$(DEVIOCSTATS)/db/iocAdminSoft.db", "IOC=$(PREFIX)")

# Optional: load alive record (requires ALIVE module)
dbLoadRecords("$(ALIVE)/db/alive.db", "P=$(PREFIX),RHOST=192.168.1.254")

# Set the callback queue size to 5000, up from default of 2000 in base.
# This can be needed to avoid errors "callbackRequest: cbLow ring buffer full".
callbackSetQueueSize(5000)
