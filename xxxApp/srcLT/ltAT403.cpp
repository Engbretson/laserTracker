

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <math.h>

#include <epicsTypes.h>
#include <epicsTime.h>
#include <epicsThread.h>
#include <epicsString.h>
#include <epicsTimer.h>
#include <epicsMutex.h>
#include <epicsEvent.h>
#include <iocsh.h>

#include "ltAt403.h"
#include <epicsExport.h>


static const char *driverName="LTAt403";



/** Constructor for the LTAt403 class.
  * Calls constructor for the asynPortDriver base class.
  * \param[in] portName The name of the asyn port driver to be created.
  */
LTAt403::LTAt403(const char *portName)
   : asynPortDriver(portName,
                    1, /* maxAddr */
                    asynInt32Mask | asynFloat64Mask | asynFloat64ArrayMask | asynEnumMask | asynDrvUserMask | asynOctetMask, /* Interface mask */
                    asynInt32Mask | asynFloat64Mask | asynFloat64ArrayMask | asynEnumMask | asynOctetMask,  /* Interrupt mask */
                    0, /* asynFlags.  This driver does not block and it is not multi-device, so flag is 0 */
                    1, /* Autoconnect */
                    0, /* Default priority */
                    0) /* Default stack size*/
{
//    asynStatus status;
//    int i;
    const char *functionName = "LTAt403";

    initializeHardware(portName);	
 
	createParam(L_iscompatibleFirmwareString,          asynParamInt32,         &L_iscompatibleFirmware);	
	
    createParam(L_commentString, asynParamOctet, &L_comment);
    createParam(L_expectedFirmwareString, asynParamOctet, &L_expectedFirmware);
    createParam(L_installedFirmwareString, asynParamOctet, &L_installedFirmware);
    createParam(L_ipAddressString, asynParamOctet, &L_ipAddress);
    createParam(L_nameString, asynParamOctet, &L_name);
    createParam(L_productNameString, asynParamOctet, &L_productName);
    createParam(L_serialNumberString, asynParamOctet, &L_serialNumber);
	
	createParam(L_horizontalAngleString, asynParamFloat64, &L_horizontalAngle);
	createParam(L_verticalAngleString, asynParamFloat64, &L_verticalAngle);
	createParam(L_humidityString, asynParamFloat64, &L_humidity);
	createParam(L_pressureString, asynParamFloat64, &L_pressure);
	createParam(L_temperatureString, asynParamFloat64, &L_temperature);
	createParam(L_xString, asynParamFloat64, &L_x);
	createParam(L_yString, asynParamFloat64, &L_y);
	createParam(L_zString, asynParamFloat64, &L_z);

	 // lets do some test commands 
 
    printf("After Initialization and before prints . . . \n");
 
 
    String^ Comment = GlobalObjects::LMFTracker->Comment;
    cout << "Comment: " << (decode)(Comment) << "\n";   
    setStringParam(L_comment,(decode)(Comment));
     
    String^ ExpectedFirmware = GlobalObjects::LMFTracker->ExpectedFirmware;
    cout << "Firmware: " << (decode)(ExpectedFirmware) << "\n";
    setStringParam(L_expectedFirmware,(decode)(ExpectedFirmware));

    String^ InstalledFirmware = GlobalObjects::LMFTracker->InstalledFirmware;
    cout << "InstalledFirmware: " << (decode)(InstalledFirmware) << "\n";
	setStringParam(L_installedFirmware,(decode)(InstalledFirmware));
	
    String^ IP = GlobalObjects::LMFTracker->IPAddress;
    cout << "IP: " << (decode)(IP) << "\n";
	setStringParam(L_ipAddress,(decode)(IP));

    Boolean CompatFirmware = GlobalObjects::LMFTracker->IsCompatibleWithInstalledFirmware;
    cout << "Is Compatible With Installed Firmware: " << CompatFirmware << "\n";
	setIntegerParam(L_iscompatibleFirmware, CompatFirmware); 
	
    String^ Name = GlobalObjects::LMFTracker->Name;
    cout << "Name: " << (decode)(Name) << "\n";
	setStringParam(L_name,(decode)(Name));
    
    String^ ProductName = GlobalObjects::LMFTracker->ProductName;
    cout << "ProductName: " << (decode)(ProductName) << "\n";
	setStringParam(L_productName,(decode)(ProductName));

    String^ SerialNumber = GlobalObjects::LMFTracker->SerialNumber;
    cout << "Serial: " << (decode)(SerialNumber) << "\n";
	setStringParam(L_serialNumber,(decode)(SerialNumber));
	
    GlobalObjects::LMFTracker->GetDirectionAsync();    
    Direction^ dir1 = GlobalObjects::LMFTracker->GetDirection(); 
    cout << "Direction H Angle: " << dir1->HorizontalAngle->Value << " V Angle: " << dir1->VerticalAngle->Value << "\n";
	setDoubleParam(L_horizontalAngle, dir1->HorizontalAngle->Value); 
	setDoubleParam(L_verticalAngle, dir1->VerticalAngle->Value);
	
// I *think* that the units tend to be a default angle in Degrees ? or have not run across the switch units commands . . .  yet.
	cout <<"HLabel " << (decode)(dir1->HorizontalAngle->Label);
	cout << " HUnitString " << (decode)(dir1->HorizontalAngle->UnitString);
//    cout << "HUnitType " << (decode)(dir1->HorizontalAngle->UnitType) << "\n"; // some off enum type
    cout << " HValueInBaseUnits " << dir1->HorizontalAngle->ValueInBaseUnits << "\n";
	
    cout << "VLabel " << (decode)(dir1->VerticalAngle->Label);
    cout << " VUnitString " << (decode)(dir1->VerticalAngle->UnitString);
//    cout << "VUnitType " << (decode)(dir1->VerticalAngle->UnitType) << "\n"; // some odd enum type 
    cout << " VValueInBaseUnits " << dir1->VerticalAngle->ValueInBaseUnits << "\n";


LMF::Tracker::MeasurementResults::Measurement^ data = GlobalObjects::LMFTracker->Measurement->MeasureStationary();

 cout << "Measurment Humidity: " << data->Humidity->Value << " Pressure: " << data->Pressure->Value << " Temperature: " << data->Temperature->Value << "\n";
 
 	setDoubleParam(L_humidity, data->Humidity->Value);
	setDoubleParam(L_pressure, data->Pressure->Value);
	setDoubleParam(L_temperature, data->Temperature->Value);
 
 StationaryMeasurement3D^ stationaryMeas3D = dynamic_cast<StationaryMeasurement3D^>(data);
 cout << " X = " << stationaryMeas3D->Position->Coordinate1->Value ;
 cout << " Y = " << stationaryMeas3D->Position->Coordinate2->Value ;
 cout << " Z = " << stationaryMeas3D->Position->Coordinate3->Value << "\n";
 
 	setDoubleParam(L_x, stationaryMeas3D->Position->Coordinate1->Value);
	setDoubleParam(L_y, stationaryMeas3D->Position->Coordinate2->Value);
	setDoubleParam(L_z, stationaryMeas3D->Position->Coordinate3->Value);


// end of test commands 

    callParamCallbacks();	


}

void LTAt403::initializeHardware(const char *portName)
{
 
 	GlobalObjects::con = gcnew Connection();
    GlobalObjects::LMFTracker = GlobalObjects::con->Connect(marshal_as<String^>(portName));
 
    GlobalObjects::LMFTracker->Initialize();

};

asynStatus LTAt403::readInt32(asynUser *pasynUser, epicsInt32 *value)
{
  int addr;
  int function = pasynUser->reason;
  int status=0;
  epicsInt32 temp;
  
  static const char *functionName = "readInt32";

  this->getAddress(pasynUser, &addr);
  
//  printf("in readInt32 . . . function %d \n",function);

//  if (function == L_iscompatibleFirmware) {
//	getIntegerParam(L_iscompatibleFirmware, &temp); 
//	printf("ReadInt32 values is  %d\n", temp);
//    temp = temp +1;
//	asynPortDriver::readInt32(pasynUser, &temp);
//		asynPortDriver::writeInt32(pasynUser, temp);
//	callParamCallbacks(addr);
//   }

  // Other functions we call the base class method
//  else {
     status = asynPortDriver::readInt32(pasynUser, value);
 // }

  callParamCallbacks(addr);
  return (status==0) ? asynSuccess : asynError;
}

/* Configuration routine.  Called directly, or from the iocsh function below */

extern "C" {

/** EPICS iocsh callable function to call constructor for the LTAt403 class.
  * \param[in] portName The name of the asyn port driver to be created.
  */
int LTAt403Configure(const char *portName)
{	
    new LTAt403(portName);
	
    return(asynSuccess);
}


/* EPICS iocsh shell commands */

static const iocshArg initArg0 = { "portName",iocshArgString};

static const iocshArg * const initArgs[] = {&initArg0};
static const iocshFuncDef initFuncDef = {"LTAt403Configure",1,initArgs};
static void initCallFunc(const iocshArgBuf *args)
{
    LTAt403Configure(args[0].sval);
}

void LTAt403Register(void)
{
    iocshRegister(&initFuncDef,initCallFunc);
}

epicsExportRegistrar(LTAt403Register);

}

