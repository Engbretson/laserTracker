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
endif

#=============================

include $(TOP)/configure/RULES
#----------------------------------------
#  ADD RULES AFTER THIS LINE

