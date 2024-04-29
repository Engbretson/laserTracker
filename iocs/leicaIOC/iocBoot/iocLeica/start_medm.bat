

rem set EPICS_DISPLAY_PATH=D:\epics\LaserTracker-R1-2\xxxApp\op\adl;D:\epics\areaDetector\ADCore\ADApp\op\adl
set EPICS_DISPLAY_PATH=c:\epics\areaDetector-R3-13\LaserTracker-R2-1-1\leicaApp\op\adl;c:\epics\areaDetector-R3-13\ADCore-R3-13\ADApp\op\adl;c:\epics\asyn-R4-44-2\opi\medm;c:\epics\autosave-R5-10-2\asApp\op\adl;c:\epics\iocStats-3-2-0\op\adl
rem set EPICS_CA_AUTO_ADDR_LIST=NOC:\epics\areaDetector-R3-12-1\ADCore-R3-12-1\ADApp\op\adl
rem set EPICS_CA_ADDR_LIST=192.168.0.53

rem call "C:\Program Files\EPICS Windows Tools\medm.exe" -x -attach -macro "P=433LT:, R=LT1:" LaserTracker.adl LT_measurement.adl ADBase.adl master.adl
call "C:\Program Files\EPICS Windows Tools\medm.exe" -x -attach -macro "P=8idtracker:, R=LT1:,CONFIG=ADAutoSave" newdisplay.adl saver.adl
