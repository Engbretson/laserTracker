
for /r %%a in (*.adl) do "C:\Program Files\caQTDM\bin\windows-x64\adl2ui.exe" "%%a" 

set OLDDIR=%CD%
cd d:\phoebus-4.7.3
e:

for  %%a in (D:\epics\areaDetector-R3-12-1\LaserTracker-R2-1\leicaApp\op\adl\*.adl) do call phoebus -main org.csstudio.display.converter.medm.Converter -force  "%%a" 

cd %OLDDIR%
c:
c:
move /Y *.bob ../bob
move /Y *.ui ../ui

cd ../bob
call doit.bat



