call dllPath.bat

rem set CAQTDM_DISPLAY_PATH=D:\epics\LaserTracker-R1-1\xxxApp\op\ui\autoconvert;
set CAQTDM_DISPLAY_PATH=set EPICS_DISPLAY_PATH=C:\epics\areaDetector-R3-12-1\LaserTracker-R1-3\leicaApp\op\ui\autoconvert;C:\epics\areaDetector-R3-12-1\ADCore-R3-12-1\ADApp\op\ui\autoconvert;C:\epics\asyn-R4-44-2\opi\ui\autoconvert;C:\epics\autosave-R5-10-2\asApp\op\ui\autoconvert;C:\epics\iocStats-3-1-16\op\ui\autoconvert

rem set EPICS_CA_AUTO_ADDR_LIST=NO
rem set EPICS_CA_ADDR_LIST=164.54.116.42


start caqtdm -x -attach -macro "P=433LT:, R=LT1:" LaserTracker.ui
start caqtdm -attach -x -macro "P=433LT:, R=LT1:" LT_measurement.ui
start caqtdm -attach -x -macro "P=433LT:, R=LT1:" ADBase.ui 
