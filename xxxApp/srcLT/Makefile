TOP=../..

include $(TOP)/configure/CONFIG
#----------------------------------------
#  ADD MACRO DEFINITIONS AFTER THIS LINE
#=============================

USR_CXXFLAGS += /EHa /clr /D_AFXDLL /AI.. /D_WIN32_WINNT=0x601

LIBRARY_IOC += ltAt403
DBD += ltAT403.dbd


ltAt403_SRCS += ltAT403.cpp
ltAt403_LIBS += asyn alive

ltAt403_LIBS += $(EPICS_BASE_IOC_LIBS)

#=============================

#PROD_IOC += testAsynPortDriver

# <name>_registerRecordDeviceDriver.cpp will be created from <name>.dbd
#testAsynPortDriver_SRCS_DEFAULT += testAsynPortDriver_registerRecordDeviceDriver.cpp testAsynPortDriverMain.cpp
#testAsynPortDriverVx_SRCS_vxWorks  += testAsynPortDriver_registerRecordDeviceDriver.cpp
#testAsynPortDriver_LIBS += testAsynPortDriverSupport asyn
#testAsynPortDriver_LIBS += $(EPICS_BASE_IOC_LIBS)

#testAsynPortDriver_OBJS_vxWorks += $(EPICS_BASE_BIN)/vxComLibrary

#===========================

include $(TOP)/configure/RULES
#----------------------------------------
#  ADD RULES AFTER THIS LINE
