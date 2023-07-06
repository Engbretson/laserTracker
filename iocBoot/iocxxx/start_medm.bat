

set EPICS_DISPLAY_PATH=D:\epics\LaserTracker-R1-2\xxxApp\op\adl;D:\epics\areaDetector\ADCore\ADApp\op\adl
rem set EPICS_CA_AUTO_ADDR_LIST=NO
rem set EPICS_CA_ADDR_LIST=192.168.0.53


call "C:\Program Files\EPICS Windows Tools\medm.exe" -x -attach -macro "P=433LT:, R=scope1:" LaserTracker.adl LT_measurement.adl ADBase.adl
