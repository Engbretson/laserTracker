# Windows startup script

< envPaths


dbLoadDatabase("../../dbd/iocxxxWin64.dbd")
iocxxxWin64_registerRecordDeviceDriver(pdbbase)

LTAT403Config("At403Simulator")

###############################################################################
iocInit
###############################################################################

# write all the PV names to a local file
dbl > dbl-all.txt

# Diagnostic: CA links in all records
dbcar(0,1)

# print the time our boot was finished
date
