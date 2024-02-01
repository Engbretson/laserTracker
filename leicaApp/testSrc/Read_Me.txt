Ok, the first generation test code worked fine with a simulator since everything valid  and fake values exist for everything.

Talking to real hardware . . . i.e., a laser tracker right out of the box - there are no active tragets, warning messages might be spit out that might be signicicant,
the laser might not be on, and a whole load of other  issues - 'not supported features' for this model . . .  so everything really needs to be heavily try blocked.

So instead of implementing and testing things in alphabetical order, need to work though these things in a more logical order.

Connecting and initializing the device
Dumping out the values and state of things which don't actually require things be running,
and have a different test routine get called dealing with doing things that actually require "working" hardware.

So redo the test program(s) here to deal with 

Original version up this is point is/was
LaserTrackerTestMain

New Versions are 
QueryTrackerState

Which now seems to implement everytjhing that we can reasonably implement at the instant.

The main loop has been looped out and the main epics program can call into this - and get to the simulator since the hardware is actually in use.


So could potentially have all the epics code be in 1 source file, have all the laser tracker be in a different one, 
and maybe have a third which is technically the glue holding the 2 together ?

=================================
D:\epics\LaserTracker-R1-0\iocBoot\iocxxx>start_epics

D:\epics\LaserTracker-R1-0\iocBoot\iocxxx>call dllpath.bat
< envPaths.windows-x64
epicsEnvSet("IOC","iocxxx")
epicsEnvSet("TOP","D:/epics/LaserTracker-R1-0")
epicsEnvSet("SUPPORT","d:/epics")
epicsEnvSet("EPICS_BASE","d:/epics/base-R7-0-7")
epicsEnvSet("ALIVE","d:/epics/alive-R1-4-0")
epicsEnvSet("ASYN","d:/epics/asyn-R4-44-2")
epicsEnvSet("AUTOSAVE","d:/epics/autosave-R5-10-2")
epicsEnvSet("DEVIOCSTATS","d:/epics/iocStats-3-1-16")
epicsEnvSet("SSCAN","d:/epics/sscan-R2-11-5")
epicsEnvSet("SNCSEQ","d:/epics/seq-R2-2-9")
epicsEnvSet("LASERTRACKER","d:/epics/LaserTracker-R1-0")
epicsEnvSet("LASER_TRACKER","d:/epics/LaserTracker-R1-0")
epicsEnvSet("ENGINEER","Engbretson")
epicsEnvSet("GROUP","8-id-e")
epicsEnvSet("LOCATION","8-id-e")
#epicsEnvSet("CAMERA_ID","AT403 394712 164.54.116.53")
epicsEnvSet("CAMERA_ID","AT930 750529 164.54.116.53")
epicsEnvSet("IOC","ioc433LT")
epicsEnvSet("PREFIX","433LT:")
#epicsEnvSet("PORT","At403Simulator")
epicsEnvSet("PORT","At930Simulator")
#epicsEnvSet("PORT","164.54.116.53")
dbLoadDatabase("../../dbd/windows-x64/iocxxxWin64.dbd")
iocxxxWin64_registerRecordDeviceDriver(pdbbase)
#LTAt403Configure("At403Simulator")
LTAt403Configure(At930Simulator)
Searching for trackers . . . :: ID = 3161B84E-7232-84BD-5824-D1C6AC86D131, Len = 120
:: ID = 3161B84E-7232-84BD-5824-D1C6AC86D131, Len = 76
:: ID = 3161B84E-7232-84BD-5824-D1C6AC86D131, Len = 76
Found : 1
 Tracker Name: AT930 750529 Serial Number: 750529 IP Address: 164.54.116.53 Type: AT930
:: ID = FE111979-09E8-E725-E551-37E9EF31847B, Len = 120
:: ID = FE111979-09E8-E725-E551-37E9EF31847B, Len = 76
:: ID = FE111979-09E8-E725-E551-37E9EF31847B, Len = 76
Is the hardware ready ?
Initialize . . .
Starting Day, Date and Time is = Tue Jun 20 16:44:09 2023
callback Initialization finished . . .
Current Day, Date and Time is = Tue Jun 20 16:44:14 2023
After Initialization . . . Check for Errors?

Top Level  Parameters . . .
Comment:
ExpectedFirmware: 2.2
InstalledFirmware: 2.2.4.243
IPAddress: 164.54.116.53
IsCompatibleWithInstalledFirmware: True
Name: AT930 750529
ProductName: AT930
SerialNumber: 750529

Trying to pre-position to known position . . .
callback PositionTo finished . . . Found a Red Ring Reflector 1.5 Inch
ReadyToMeasure. . .

Compensations
Compensation Count: 5
Comment:   GUID: b6644850-d204-4e08-a904-1549e57a9fdb Name: 2022-03-17 20-48-54 TimeStamp: Thursday, 17. March 2022 19:48:54.192
Comment:   GUID: 95c85be0-73ac-43c3-8adc-e4854b7dfdbf Name: 2022-04-25 10-11-05 TimeStamp: Monday, 25. April 2022 10:11:05.234
Comment:   GUID: 80097fdd-c08a-480e-a17f-86494a533d7c Name: 2022-05-17 08-00-46 TimeStamp: Tuesday, 17. May 2022 08:00:46.325
Comment:   GUID: 2c39bf4f-7419-4e1c-b2bb-54a76dc17de2 Name: 2022-05-17 13-10-16 TimeStamp: Tuesday, 17. May 2022 13:10:16.875
Comment:   GUID: 1c39d2a9-a610-4fbb-9cf3-0ca2459bb2ad Name: 2022-05-17 13-55-03 TimeStamp: Tuesday, 17. May 2022 13:55:03.145
Selected Compensation . . .
Comment:   GUID: 80097fdd-c08a-480e-a17f-86494a533d7c Name: 2022-05-17 08-00-46 TimeStamp: Tuesday, 17. May 2022 08:00:46.325


Face
Face: isface1: True
Face: Value: Face1


InclinationSensor
Attemping InclinationToGravity . . . This is going to take a while . . .
Attemping StartBubbleReadoutStream . . .
Callback OnBubbleReadoutArrived . . .  InclinationL: 999 InclinationT: 999 InValidRange: False InWorkingRange: False TimeStamp: Tuesday, 20. June 2023 21:44:56.089

CurrentInclinationToGravity: TimeStamp: Tuesday, 20. June 2023 16:44:51.852
InclinationRotX Label: Rotation around x-axis UnitString: deg UnitType: Angle Value: -0.0090185 ValueInBaseUnits: -0.000157403
InclinationRotY Label: Rotation around y-axis UnitString: deg UnitType: Angle Value: 0.0162372 ValueInBaseUnits: 0.000283392
  Label: Inclined to gravity Value: False
ThresholdExceededTime: Monday, 01. January 0001 00:00:00.000
WorkingRangeExceededTime: Monday, 01. January 0001 00:00:00.000
  Label: Active Value: False
Current: TimeStamp: Tuesday, 20. June 2023 16:44:08.694
X Label: X UnitString: deg UnitType: Angle Value: 0 ValueInBaseUnits: 0
Y Label: X UnitString: deg UnitType: Angle Value: 0 ValueInBaseUnits: 0
Interval: Label: Interval UnitString: ms UnitType: Time Value: 60000 ValueInBaseUnits: 60000
Theshold: Label: Threshold UnitString: deg UnitType: Angle Value: 0.000859437 ValueInBaseUnits: 1.5e-05
  Label: Threshold Exceeded Value: False
  Label: Working range exceeded Value: False
Callback OnBubbleReadoutArrived . . .  InclinationL: 999 InclinationT: 999 InValidRange: False InWorkingRange: False TimeStamp: Tuesday, 20. June 2023 21:44:56.089



Laser
Wakeup Time: Monday, 01. January 0001 00:00:00.000
Is Laser Warmed Up: Label: Is laser warmed up Value: True
Is on: Is laser on Value : True


Measurement
MeasurementInProgress Label: MeasurementInProgress Value: False
Profiles: Count: 4

StationaryMeasurementProfile
GUID: 5ae054bd-9257-4b00-a124-430d0a5be34d
Name: Stationary
Accuracy: Label: Accuracy Value: Standard
TwoFace Label: TwoFace Value: False

ContinuousTimeProfile
GUID: 5eb61749-563e-4f80-8ecc-affc6d5648ce
Name: Continuous Time
PacketRate: Label: Packet Rate MaxValue: 50 MaxValueInBaseUnits: 50 MinValue: 1 MinValueInBaseUnits: 1 UnitString:   UnitType: None Value: 10 ValueInBaseUnits: 10
TimeSeparation: Label: Time Separation MaxValue: 100000 MaxValueInBaseUnits: 100000 MinValue: 1 MinValueInBaseUnits: 1 UnitString: ms UnitType: Time Value: 100 ValueInBaseUnits: 100

ContinuousDistanceProfile
GUID: 0b1229a5-b261-4e78-8e19-3e5534a5c22e
Name: Continuous Distance
DistanceSeparation: Label: DistanceSeparation MaxValue: 1e+06 MaxValueInBaseUnits: 1000 MinValue: 0.01 MinValueInBaseUnits: 1e-05 UnitString: mm UnitType: Length Value: 1 ValueInBaseUnits: 0.001

CustomTriggerProfile
GUID: 02750812-ea0d-4236-b1be-4b56777dacb5
Name: Custom Trigger
ClockSource: Label: ClockSource Value: Internal
ClockTransmission: Label: ClockTransmission Value: Negative
MinimalTimeDelay: Label: Minimal Time Delay MaxValue: 250 MaxValueInBaseUnits: 250 MinValue: 0 MinValueInBaseUnits: 0 UnitString: ms UnitType: Time Value: 0.5 ValueInBaseUnits: 0.5
PacketRate: Label: PacketRate MaxValue: 50 MaxValueInBaseUnits: 50 MinValue: 1 MinValueInBaseUnits: 1 UnitString:   UnitType: None Value: 10 ValueInBaseUnits: 10
StartStopActiveLevel: Label: StartStopActiveLevel Value: Low
StartStopSource: Label: StartStopSource Value: Active

Selected
GUID: 5ae054bd-9257-4b00-a124-430d0a5be34d
Name: Stationary
StationaryMeasurementProfile
Accuracy: Label: Accuracy Value: Standard
TwoFace Label: TwoFace Value: False
Status
Label: Measurement Status
Value: ReadyToMeasure
Preconditions: Count: 0


MeteoStation
HardwareHumidity: Label: Humidity SerialNumber: 098.1015.1202.02.13 UnitString: % UnitType: Humidity Value: 45.3 ValueInBaseUnits: 45.3
Available: Available Value: True
HardwarePressure: Label: Pressure SerialNumber: 098.1015.1202.02.13 UnitString: mBar UnitType: Pressure Value: 990.897 ValueInBaseUnits: 990.897
Available: Available Value: True
HardwareTemperature: Label: Temperature SerialNumber: 098.1015.1202.02.13 UnitString: ░C UnitType: Temperature Value: 21.8725 ValueInBaseUnits: 21.8725
Available: Available Value: True
IsAirSensorConnected: Label: External air sensor connected Value: False
ManualHumidity: Label: Humidity MaxValue: 100 MaxValueInBaseUnits: 100 MinValue: 0 MinValueInBaseUnits: 0 SerialNumber:   UnitString: % UnitType: Humidity Value: 45.3 ValueInBaseUnits: 45.3
Available: Available Value: True
ManualPressure: Label: Pressure MaxValue: 1400 MaxValueInBaseUnits: 1400 MinValue: 330 MinValueInBaseUnits: 330 SerialNumber:   UnitString: mBar UnitType: Pressure Value: 990.897 ValueInBaseUnits: 990.897
Available: Available Value: True
ManualTemperature: Label: Temperature MaxValue: 60 MaxValueInBaseUnits: 60 MinValue: -40 MinValueInBaseUnits: -40 SerialNumber:   UnitString: ░C UnitType: Temperature Value: 21.8725 ValueInBaseUnits: 21.8725
Available: Available Value: True
ObjectTemperature: Label: Temperature SerialNumber:   UnitString: ░C UnitType: Temperature Value: 0 ValueInBaseUnits: 0
Available: Available Value: False
Source: Label: MeteoSource Value: LiveMeteo


OverviewCamera
Callback OnWPFBitmapImageArrived . . . 0 Targets seen in Image.
Callback OnImageArrived . . . 0 Targets seen in Image.
Callback OnWPFBitmapImageArrived . . . 0 Targets seen in Image.
Callback OnImageArrived . . . 0 Targets seen in Image.
Callback OnWPFBitmapImageArrived . . . 3 Targets seen in Image.
Callback OnImageArrived . . . 3 Targets seen in Image.
Callback OnWPFBitmapImageArrived . . . 3 Targets seen in Image.
Callback OnImageArrived . . . 3 Targets seen in Image.
Callback OnWPFBitmapImageArrived . . . 3 Targets seen in Image.
Callback OnImageArrived . . . 3 Targets seen in Image.
Callback OnWPFBitmapImageArrived . . . 3 Targets seen in Image.
Callback OnImageArrived . . . 3 Targets seen in Image.
Callback OnWPFBitmapImageArrived . . . 3 Targets seen in Image.
Callback OnImageArrived . . . 3 Targets seen in Image.
Brightness: Label: Brightness MaxValue: 100 MaxValueInBaseUnits: 1 MinValue: 0 MinValueInBaseUnits: 0 UnitString: % UnitType: Percent Value: 24.7059 ValueInBaseUnits: 0.247059
Constrast: Label: Contrast MaxValue: 100 MaxValueInBaseUnits: 1 MinValue: 0 MinValueInBaseUnits: 0 UnitString: % UnitType: Percent Value: 50.1961 ValueInBaseUnits: 0.501961


PowerLock
Use Power Lock: Label: Use PowerLock Value: True
Get Target Directions Count: 3


PowerSource
SensorPowerStatus: Label: Level UnitString: % UnitType: Percent Value: 0 ValueInBaseUnits: 0
RunsOn: Label: Runs on Value: Mains


QuickRelease
QuickRelease: Label: Quick Release closed Value: True


Settings
 CoordinateType : Cartesian
 RotationType : RotationAngles
 AngleUnits : Degree
 HumidityUnits : RelativeHumidity
 LengthUnits : Millimeter
 PercentUnits : Percent
 PressureUnits : mBar
 TemperatureUnits : Celsius
 TimeUnits : Millisecond

 Get Orientation
  CoordinateType : Cartesian
  RotationType : RotationAngles
  Rotation0 : Label: Q0  UnitString:    Value: 0
  Rotation1 : Label: XAngle  UnitString: deg  Value: 0
  Rotation2 : Label: YAngle  UnitString: deg  Value: 0
  Rotation3 : Label: ZAngle  UnitString: deg  Value: 0
  Translation1 : Label: X  UnitString: mm  Value: 0
  Translation2 : Label: Y  UnitString: mm  Value: 0
  Translation3 : Label: Z  UnitString: mm  Value: 0

 Get Transformation
  CoordinateType : Cartesian
  RotationType : RotationAngles
  Rotation0 : Label: Q0  UnitString:    Value: 0
  Rotation1 : Label: XAngle  UnitString: deg  Value: 0
  Rotation2 : Label: YAngle  UnitString: deg  Value: 0
  Rotation3 : Label: ZAngle  UnitString: deg  Value: 0
  Scale : Label: Scale  UnitString:    Value: 1
  Translation1 : Label: X  UnitString: mm  Value: 0
  Translation2 : Label: Y  UnitString: mm  Value: 0
  Translation3 : Label: Z  UnitString: mm  Value: 0



Targets
Target Count: 1

Reflector Comment:
GUID: 174d58b6-ac44-41b8-a6b3-d85befbeca90
IsSelectable: True
Name: RRR 1.5in
Product Name: Red Ring Reflector 1.5 Inch
TimeStamp: Monday, 15. May 2023 15:27:38.970
ADMOffset:: Label: ADM Offset UnitString: mm UnitType: Length Value: 0 ValueInBaseUnits: 0
SurfaceOffset:: Label: Surface Offset UnitString: mm UnitType: Length Value: 19.05 ValueInBaseUnits: 0.01905



TargetSearch
ApproximateDistance: Label: Approximate distance MaxValue: 100000 MaxValueInBaseUnits: 100 MinValue: 0 MinValueInBaseUnits: 0 UnitString: mm UnitType: Length Value: 8000 ValueInBaseUnits: 8
Radius: Label: Radius MaxValue: 2000 MaxValueInBaseUnits: 2 MinValue: 0 MinValueInBaseUnits: 0 UnitString: mm UnitType: Length Value: 100 ValueInBaseUnits: 0.1
TimeOut: Label: TimeOut MaxValue: 120000 MaxValueInBaseUnits: 120000 MinValue: 10000 MinValueInBaseUnits: 10000 UnitString: ms UnitType: Time Value: 45000 ValueInBaseUnits: 45000


TrackerAlignment . . . basically requires real number to calculate anything.


Triggers
Triggers: Count: 1
GUID: 6332db08-8715-463b-9733-740bbeb2d7d1
Name: Stable Probing
IsEnabled: Label: Is enabled
Value: False
TriggerRegion: Label: Trigger Region MaxValue: 100 MaxValueInBaseUnits: 0.1 MinValue: 0.01 MinValueInBaseUnits: 1e-05 UnitString: mm UnitType: Length Value: 0.05 ValueInBaseUnits: 5e-05
TriggerTime: Label: Trigger Time MaxValue: 25000 MaxValueInBaseUnits: 25000 MinValue: 300 MinValueInBaseUnits: 300 UnitString: ms UnitType: Time Value: 300 ValueInBaseUnits: 300


WrtlBoxes . . . Not Implemented in this SDK.

Press any key to continue . . .
Bool Value changed:
Disconnect . . .
Measurement Status Value changed:
Not Ready . . .
callback Disconnected finished . . .

***********************************

dbLoadRecords("D:/epics/LaserTracker-R1-0/db/laserTracker.db","P=433LT:,R=scope1:,PORT=At930Simulator,ADDR=0,TIMEOUT=1")
#asynSetTraceMask($(PORT),0,0xff)
asynSetTraceIOMask(At930Simulator,0,0x2)
### Load database record for alive heartbeating support.
# IOCNM is name of IOC, RHOST specifies the remote server accepting hearbeats
dbLoadRecords("d:/epics/alive-R1-4-0/aliveApp/Db/alive.db", "P=433LT:,IOCNM=ioc433LT,RHOST=164.54.100.11")
dbLoadRecords("d:/epics/alive-R1-4-0/aliveApp/Db/aliveMSGCalc.db", "P=433LT:")
iocInit()
Starting iocInit
############################################################################
## EPICS R7.0.7.1-DEV
## Rev. R7.0.7-4-gf62f68fd663647d79d38-dirty
## Rev. Date Git: 2022-09-07 17:00:45 -0500
############################################################################
2023/06/20 16:45:08.420 433LT:scope1:humidity_RBV devAsynFloat64::processCallbackInput process read error asynPortDriver:readFloat64: status=10, function=17, name=humidity, value is undefined
2023/06/20 16:45:08.420 433LT:scope1:pressure_RBV devAsynFloat64::processCallbackInput process read error asynPortDriver:readFloat64: status=10, function=18, name=pressure, value is undefined
2023/06/20 16:45:08.421 433LT:scope1:temperature_RBV devAsynFloat64::processCallbackInput process read error asynPortDriver:readFloat64: status=10, function=19, name=temperature, value is undefined
2023/06/20 16:45:08.421 433LT:scope1:x_RBV devAsynFloat64::processCallbackInput process read error asynPortDriver:readFloat64: status=10, function=20, name=x, value is undefined
2023/06/20 16:45:08.422 433LT:scope1:y_RBV devAsynFloat64::processCallbackInput process read error asynPortDriver:readFloat64: status=10, function=21, name=y, value is undefined
2023/06/20 16:45:08.422 433LT:scope1:z_RBV devAsynFloat64::processCallbackInput process read error asynPortDriver:readFloat64: status=10, function=22, name=z, value is undefined
iocRun: All initialization complete
# write all the PV names to a local file
dbl > dbl-all.txt
# Diagnostic: CA links in all records
dbcar(0,1)
CA links in record named '0'

Total 0 CA links; 0 connected, 0 not connected.
    0 can't read, 0 can't write.  (0 disconnects, 0 writes prohibited)

# print the time our boot was finished
date
2023/06/20 16:45:08.428751
epics>

