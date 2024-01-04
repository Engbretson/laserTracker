TOP=../..
include $(TOP)/configure/CONFIG
#----------------------------------------
#  ADD MACRO DEFINITIONS AFTER THIS LINE

#USR_CXXFLAGS += /EHa  /DBW /D_AFXDLL  /AI.. /D_WIN32_WINNT=0x601 /AI"C:\Program Files (x86)\Reference Assemblies\Microsoft\Framework\.NETFramework\v4.5" /clr 
USR_CXXFLAGS += /EHa  /D_AFXDLL  /AI.. /D_WIN32_WINNT=0x601 /AI"C:\Program Files (x86)\Reference Assemblies\Microsoft\Framework\.NETFramework\v4.8" /clr 

# /DBW  do B&W images versus Color, which the client needs to use pva to get

USR_LDFLAGS += /IGNORE:4248

INC += leica.h

LIBRARY_IOC = leica
LIB_SRCS += leica.cpp

DBD += leicaSupport.dbd

include $(ADCORE)/ADApp/commonLibraryMakefile

#=============================

include $(TOP)/configure/RULES
#----------------------------------------
#  ADD RULES AFTER THIS LINE

