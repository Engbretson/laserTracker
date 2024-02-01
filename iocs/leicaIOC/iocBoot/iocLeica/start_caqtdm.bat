
#set CAQTDM_DISPLAY_PATH=D:\epics\areaDetector-R3-12-1\LaserTracker-R2-1\leicaApp\op\ui;D:\epics\areaDetector-R3-12-1\ADCore-R3-12-1\ADApp\op\ui\autoconvert;D:\epics\autosave-R5-10-2\asApp\op\ui\autoconvert
set CAQTDM_DISPLAY_PATH=D:\epics\areaDetector-R3-12-1\LaserTracker-R2-1\leicaApp\op\ui;D:\epics\areaDetector-R3-12-1\ADCore-R3-12-1\ADApp\op\ui;d:\epics\asyn-R4-44-2\opi\ui\autoconvert;d:\epics\autosave-R5-10-2\asApp\op\ui\autoconvert;d:\epics\iocStats-3-1-16\op\ui\autoconvert;D:\epics\areaDetector-R3-12-1\ADCore-R3-12-1\ADApp\op\ui

#set EPICS_CA_AUTO_ADDR_LIST=NO
#set EPICS_CA_ADDR_LIST=164.54.104.22


start caqtdm -x -attach -macro "P=8idtracker:, R=LT1:" newdisplay.ui
start caqtdm -x -attach -macro "P=8idtracker:, R=LT1:,CONFIG=ADAutoSave" saver.ui
start caqtdm -x -attach -macro "P=8idtracker:, R=LT1:" ad_cam_image.ui


