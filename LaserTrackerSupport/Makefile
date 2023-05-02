TOP=../
include $(TOP)/configure/CONFIG
#----------------------------------------
#  ADD MACRO DEFINITIONS AFTER THIS LINE
#=============================

ifeq (windows-x64, $(findstring windows-x64, $(T_A)))

BIN_INSTALLS    += ../LMF.Tracker.Connection.dll
BIN_INSTALLS    += ../LMF.Tracker.Connection.tlb
BIN_INSTALLS    += ../LMF.Tracker.Connection.xml

BIN_INSTALLS    += ../TrackerScope.exe

# ProcServ Related
BIN_INSTALLS    += ../msys-2.0.dll
BIN_INSTALLS    += ../msys-gcc_s-seh-1.dll
BIN_INSTALLS    += ../msys-stdc++-6.dll
BIN_INSTALLS    += ../procServ.exe


#
# Missing dll's that dll versions of XXX requires to work.
#These *should* be discovered, since they are on the dllPath, but that seems to find the top level dll's, not some of the secondary ones
#that some of those actually use/call. Maybe run time resolution??
#

BIN_INSTALLS    += ../cbw64.dll
BIN_INSTALLS    += ../libusb-1.0.dll
BIN_INSTALLS    += ../Packet.dll
BIN_INSTALLS    += ../wpcap.dll
BIN_INSTALLS    += ../vac.dll

endif

#=============================

include $(TOP)/configure/RULES
#----------------------------------------
#  ADD RULES AFTER THIS LINE

