
set CAQTDM_DISPLAY_PATH=C:\epics\areaDetector-R3-12-1\LaserTracker-R2-0\leicaApp\op\ui;C:\epics\areaDetector-R3-12-1\ADCore-R3-12-1\ADApp\op\ui\autoconvert;C:\epics\autosave-R5-10-2\asApp\op\ui\autoconvert

set EPICS_CA_AUTO_ADDR_LIST=NO
set EPICS_CA_ADDR_LIST=164.54.104.22


start caqtdm -x  -macro "P=433LT:, R=LT1:" LaserTracker.ui
start caqtdm -attach -x -macro "P=433LT:, R=LT1:" LT_measurement.ui
start caqtdm -attach -x -macro "P=433LT:, R=LT1:" ad_cam_image.ui 
start caqtdm -attach -x -macro "P=433LT:, R=LT1:" main.ui
