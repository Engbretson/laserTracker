call dllPath.bat

set CAQTDM_DISPLAY_PATH=C:\epics\LaserTracker-R1-0\xxxApp\op\ui\autoconvert
set EPICS_CA_AUTO_ADDR_LIST=NO
set EPICS_CA_ADDR_LIST=192.168.0.53

caget 433LT:scope1:serialNumber_RBV

call caqtdm -x -macro "P=433LT:, R=scope1:" LaserTracker.ui