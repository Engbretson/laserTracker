call dllPath.bat

rem set CAQTDM_DISPLAY_PATH=D:\epics\LaserTracker-R1-1\xxxApp\op\ui\autoconvert;
set CAQTDM_DISPLAY_PATH=C:\epics\areaDetector-R3-12-1\LaserTracker-R1-3\leicaApp\op\ui\autoconvert
rem set EPICS_CA_AUTO_ADDR_LIST=NO
rem set EPICS_CA_ADDR_LIST=164.54.116.42


start caqtdm -x -attach -macro "P=433LT:, R=scope1:" LaserTracker.ui
call caqtdm -attach -x -macro "P=433LT:, R=scope1:" LT_measurement.ui 
