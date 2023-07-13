

rem set EPICS_DISPLAY_PATH=D:\epics\LaserTracker-R1-2\xxxApp\op\adl;D:\epics\areaDetector\ADCore\ADApp\op\adl
set EPICS_DISPLAY_PATH=C:\epics\areaDetector-R3-12-1\LaserTracker-R1-3\leicaApp\op\adl;C:\epics\areaDetector-R3-12-1\ADCore-R3-12-1\ADApp\op\adl;C:\epics\asyn-R4-44-2\opi\medm;C:\epics\autosave-R5-10-2\asApp\op\adl;C:\epics\iocStats-3-1-16\op\adl
rem set EPICS_CA_AUTO_ADDR_LIST=NOC:\epics\areaDetector-R3-12-1\ADCore-R3-12-1\ADApp\op\adl
rem set EPICS_CA_ADDR_LIST=192.168.0.53

call "C:\Program Files\EPICS Windows Tools\medm.exe" -x -attach -macro "P=433LT:, R=LT1:" LaserTracker.adl LT_measurement.adl ADBase.adl adbase.adl
