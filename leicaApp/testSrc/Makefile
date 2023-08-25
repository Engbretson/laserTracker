
TOP=../..

include $(TOP)/configure/CONFIG
#----------------------------------------
#  ADD MACRO DEFINITIONS AFTER THIS LINE
#=============================

# For Kate Feng's implementation of devScaler.c, which uses Till
# Straumann's basicIoOps.h to bridge any mismatch between endianness
# of Joerger scaler hardware and endianness of processor.
#USR_CFLAGS += -DHAS_IOOPS_H

# find the local laser tracker assemmbly, but also find the system level 4.5 support files
#USR_CXXFLAGS += /EHa /clr /D_AFXDLL /AI.. /D_WIN32_WINNT=0x601 /AI"C:\Program Files (x86)\Reference Assemblies\Microsoft\Framework\.NETFramework\v4.5" /DDO_MAIN
USR_CXXFLAGS += /EHa /clr /D_AFXDLL /AI.. /D_WIN32_WINNT=0x601 /AI"C:\Program Files (x86)\Reference Assemblies\Microsoft\Framework\.NETFramework\v4.8" /DDO_MAIN

#=============================
# build an ioc application

# PROD_NAME is used by ADCORE to add libraries
# DBD_PREFIX is used by ADCORE to add dbd files

$(PROD_NAME)_SRCS  +=  $(PROD_NAME)Main.cpp
$(PROD_NAME)_SRCS  +=  LaserTrackerCode.cpp
   
PROD_NAME = QueryTrackerState
DBD_PREFIX = iocQueryTrackerState

PROD_IOC = $(PROD_NAME)

$(PROD_NAME)_SRCS  +=  $(PROD_NAME).cpp
$(PROD_NAME)_SRCS  +=  LaserTrackerCode.cpp

#===========================

include $(TOP)/configure/RULES
#----------------------------------------
#  ADD RULES AFTER THIS LINE
