
rem  CAQTDM_DISPLAY_PATH=D:\epics\areaDetector-R3-12-1\LaserTracker-R2-1\leicaApp\op\ui;D:\epics\areaDetector-R3-12-1\ADCore-R3-12-1\ADApp\op\ui\autoconvert;D:\epics\autosave-R5-10-2\asApp\op\ui\autoconvert
set CAQTDM_DISPLAY_PATH=c:\epics\areaDetector-R3-13\LaserTracker-R2-1-1\leicaApp\op\ui;c:\epics\areaDetector-R3-13\ADCore-R3-13\ADApp\op\ui\autoconvert;c:\epics\asyn-R4-44-2\opi\medm;c:\epics\autosave-R5-10-2\asApp\op\ui\autoconvert;c:\epics\iocStats-3-2-0\op\ui\autoconvert;c:\epics\areaDetector-R3-13\ADCore-R3-13\ADApp\op\ui;

#set EPICS_CA_AUTO_ADDR_LIST=NO
#set EPICS_CA_ADDR_LIST=164.54.104.22
C:\epics\asyn-R4-44-2\opi\caqtdm\autoconvert

start caqtdm -x  -macro "P=8idtracker:, R=LT1:" newdisplay.ui
start caqtdm -x -attach -macro "P=8idtracker:, R=LT1:,CONFIG=ADAutoSave" saver.ui
rem start caqtdm -x -attach -macro "P=8idtracker:, R=LT1:" ad_cam_image.ui


