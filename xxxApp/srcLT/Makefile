TOP=../..
include $(TOP)/configure/CONFIG
#----------------------------------------
#  ADD MACRO DEFINITIONS AFTER THIS LINE


USR_CXXFLAGS += /EHa /clr /D_AFXDLL /AI.. /D_WIN32_WINNT=0x601

LIBRARY_IOC = ltAT403

ltAT403_SRCS += ltAT403.cpp

#DBDINC += ltAT403Support

ltAT403_DBD += ltAT403Support.dbd
DBD += ltAT403Support.dbd
DBD += ltAT403.dbd

ltAT403_LIBS += alive
ltAT403_LIBS += asyn


ltAT403_LIBS += $(EPICS_BASE_IOC_LIBS)

#=============================

include $(TOP)/configure/RULES
#----------------------------------------
#  ADD RULES AFTER THIS LINE

