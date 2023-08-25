TOP=../..
include $(TOP)/configure/CONFIG
#----------------------------------------
#  ADD MACRO DEFINITIONS AFTER THIS LINE

#USR_CXXFLAGS += /EHa  /DBW /D_AFXDLL  /AI.. /D_WIN32_WINNT=0x601 /AI"C:\Program Files (x86)\Reference Assemblies\Microsoft\Framework\.NETFramework\v4.5" /clr 
USR_CXXFLAGS += /EHa  /DBW /D_AFXDLL  /AI.. /D_WIN32_WINNT=0x601 /AI"C:\Program Files (x86)\Reference Assemblies\Microsoft\Framework\.NETFramework\v4.8" /clr 

USR_LDFLAGS += /IGNORE:4248
# /DBW
INC += leica.h

LIBRARY_IOC = leica
LIB_SRCS += leica.cpp

DBD += leicaSupport.dbd

include $(ADCORE)/ADApp/commonLibraryMakefile

#=============================

include $(TOP)/configure/RULES
#----------------------------------------
#  ADD RULES AFTER THIS LINE

