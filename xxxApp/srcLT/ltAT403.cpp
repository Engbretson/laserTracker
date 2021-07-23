
#include <iocsh.h> 
#include <epicsExport.h>
#include <asynPortDriver.h> 


// Laser Tracker Includes
#include <iostream>
#include <msclr\marshal.h>
#include <msclr\marshal_cppstd.h>

using namespace System;
using namespace msclr::interop;
using namespace std;

#using <LMF.Tracker.Connection.dll>

using namespace LMF::Tracker;
using namespace LMF::Tracker::Measurements;
using namespace LMF::Tracker::MeasurementStatus;
using namespace LMF::Tracker::MeasurementResults;
using namespace LMF::Tracker::ErrorHandling;
using namespace LMF::Tracker::Targets;
using namespace LMF::Tracker::Triggers;
using namespace LMF::Tracker::Enums;
using namespace LMF::Tracker::BasicTypes;

// end of Laser Tracker Includes

// may not need any of these actually, could hard encode it at point of actual use


#define commentString "comment"
#define expectedFirmwareString "expectedfirmware"
#define installedFirmwareString "installedfirmware"
#define ipAddressString "ipAddress"
#define iscompatibleFirmwareString "iscompatibleFirmware"
#define nameString "name"
#define productNameString "productName"
#define serialNumberString "serialNumber"



static const char *driverName = "LTAT403"; 

ref class GlobalObjects {
public:

static LMF::Tracker::Tracker^ LMFTracker;   
static Connection^ con = gcnew Connection();

};

/** Class definition for the  class*/

 class LTAT403 : public asynPortDriver {
public: 
LTAT403(const char *trackerName); 

/* These are the methods that we override from asynPortDriver */

virtual asynStatus readInt32(asynUser *pasynUser, epicsInt32 *value);

//virtual asynStatus writeInt32(asynUser *pasynUser, epicsInt32 value);
//virtual asynStatus readInt32(asynUser *pasynUser, epicsInt32 *value);
//virtual asynStatus writeFloat64(asynUser *pasynUser, epicsFloat64 value);


//virtual asynStatus writeUInt32Digital(asynUser *pasynUser, epicsUInt32 value, epicsUInt32 mask);
//virtual void report(FILE *fp, int details);

static void initializeHardware();
protected: 


private: 




int comment;
#define FIRST_lsAT403_PARAM comment
int expectedFirmware;
int installedFirmware;
int ipAddress;
int iscompatibleFirmware;
int name;
int productName;
int serialNumber;
#define LAST_lsAT403_PARAM serialNumber



}; 

#define NUM_PARAMS (&LAST_lsAT403_PARAM - &FIRST_lsAT403_PARAM + 1) 


/** Constructor for the  class*/

LTAT403::LTAT403(const char *trackerName)
: asynPortDriver(trackerName, 0, 0,
asynInt32Mask | asynDrvUserMask, // Interfaces that we implement
0, // Interfaces that do callbacks
ASYN_MULTIDEVICE | ASYN_CANBLOCK, 1,
/* ASYN_CANBLOCK=1, ASYN_MULTIDEVICE=1, autoConnect=1 */
0, 0) /* Default priority and stack size */

{

createParam(commentString, asynParamOctet, &comment);
createParam(expectedFirmwareString, asynParamOctet, &expectedFirmware);
createParam(installedFirmwareString, asynParamOctet, &installedFirmware);
createParam(ipAddressString, asynParamOctet, &ipAddress);
createParam(iscompatibleFirmwareString, asynParamInt32, &iscompatibleFirmware);
createParam(nameString, asynParamOctet, &name);
createParam(productNameString, asynParamOctet, &productName);
createParam(serialNumberString, asynParamOctet, &serialNumber);

printf("Does this code actually get hit (in Constructor)?\n");

	setIntegerParam(iscompatibleFirmware, 42); 
    callParamCallbacks();	


epicsInt32 temp;

	getIntegerParam(iscompatibleFirmware, &temp); 
		
	printf("Did this code actually get hit? (in Constructor) %d\n", temp);

}


void LTAT403::initializeHardware()
{
 printf("Before Initialization and before prints . . . \n");
 
  GlobalObjects::LMFTracker->Initialize();
  
   
 // lets do some test commands
 printf("After Initialization and before prints . . . \n");
 
 
     String^ Comment = GlobalObjects::LMFTracker->Comment;
    cout << "Comment: " << msclr::interop::marshal_as<std::string>(Comment) << "\n";   
//	setStringParam(comment,msclr::interop::marshal_as<std::string>(Comment);
     
    String^ Firmware = GlobalObjects::LMFTracker->ExpectedFirmware;
    cout << "Firmware: " << msclr::interop::marshal_as<std::string>(Firmware) << "\n";
//	setStringParam(firmware,msclr::interop::marshal_as<std::string>(Firmware);

    String^ InstalledFirmware = GlobalObjects::LMFTracker->InstalledFirmware;
    cout << "InstalledFirmware: " << msclr::interop::marshal_as<std::string>(InstalledFirmware) << "\n";
//	setStringParam(installedFirmware,msclr::interop::marshal_as<std::string>(InstalledFirmware);
	
    String^ IP = GlobalObjects::LMFTracker->IPAddress;
    cout << "IP: " << msclr::interop::marshal_as<std::string>(IP) << "\n";
//	setStringParam(ipAddress,msclr::interop::marshal_as<std::string>(IP);

    Boolean CompatFirmware = GlobalObjects::LMFTracker->IsCompatibleWithInstalledFirmware;
    cout << "Is Compatible With Installed Firmware: " << CompatFirmware << "\n";
//	setIntegerParam(LTAT403::iscompatibleFirmware, CompatFirmware); 

    String^ Name = GlobalObjects::LMFTracker->Name;
    cout << "Name: " << msclr::interop::marshal_as<std::string>(Name) << "\n";
//	setStringParam(name,msclr::interop::marshal_as<std::string>(Name);
    
    String^ ProductName = GlobalObjects::LMFTracker->ProductName;
    cout << "ProductName: " << msclr::interop::marshal_as<std::string>(ProductName) << "\n";
//	setStringParam(productName,msclr::interop::marshal_as<std::string>(ProductName);

    String^ Serial = GlobalObjects::LMFTracker->SerialNumber;
    cout << "Serial: " << msclr::interop::marshal_as<std::string>(Serial) << "\n";
//	setStringParam(serial,msclr::interop::marshal_as<std::string>(Serial);

    GlobalObjects::LMFTracker->GetDirectionAsync();    
    Direction^ dir1 = GlobalObjects::LMFTracker->GetDirection(); 
    cout << "Direction H Angle: " << dir1->HorizontalAngle->Value << " V Angle: " << dir1->VerticalAngle->Value << "\n";

// end of test commands 

//	  callParamCallbacks();
}

asynStatus LTAT403::readInt32(asynUser *pasynUser, epicsInt32 *value)
{
  int addr;
  int function = pasynUser->reason;
  int status=0;
  unsigned short shortVal;
  int range;
  epicsInt32 temp;
  //static const char *functionName = "readInt32";

  this->getAddress(pasynUser, &addr);
  
  printf("in readInt32 . . . function %d \n",function);

  if (function == iscompatibleFirmware) {
	getIntegerParam(iscompatibleFirmware, &temp); 
	printf("ReadInt32 values is  %d\n", temp);
	status = asynPortDriver::readInt32(pasynUser, &temp);
   }

  // Other functions we call the base class method
  else {
     status = asynPortDriver::readInt32(pasynUser, value);
  }

  callParamCallbacks(addr);
  return (status==0) ? asynSuccess : asynError;
}

/** Configuration command, called directly or from iocsh */

extern "C" int LTAT403Config(const char *trackerName)
{ 
    asynStatus status = asynError;
	
LTAT403 *pLTAT403 = new LTAT403(trackerName);

//pLTAT403 = NULL; 

GlobalObjects::con = gcnew Connection();
GlobalObjects::LMFTracker = GlobalObjects::con->Connect(marshal_as<String^>(trackerName));

 printf("Before Initialization and before prints (in Config). . . \n");
 
  GlobalObjects::LMFTracker->Initialize();
//  LTAT403::initializeHardware();
  
  
   
 // lets do some test commands
 printf("After Initialization and before prints (in Config). . . \n");
 
 
     String^ Comment = GlobalObjects::LMFTracker->Comment;
    cout << "Comment: " << msclr::interop::marshal_as<std::string>(Comment) << "\n";   
//	setStringParam(comment,msclr::interop::marshal_as<std::string>(Comment);
     
    String^ Firmware = GlobalObjects::LMFTracker->ExpectedFirmware;
    cout << "Firmware: " << msclr::interop::marshal_as<std::string>(Firmware) << "\n";
//	setStringParam(firmware,msclr::interop::marshal_as<std::string>(Firmware);

    String^ InstalledFirmware = GlobalObjects::LMFTracker->InstalledFirmware;
    cout << "InstalledFirmware: " << msclr::interop::marshal_as<std::string>(InstalledFirmware) << "\n";
//	setStringParam(installedFirmware,msclr::interop::marshal_as<std::string>(InstalledFirmware);
	
    String^ IP = GlobalObjects::LMFTracker->IPAddress;
    cout << "IP: " << msclr::interop::marshal_as<std::string>(IP) << "\n";
//	setStringParam(ipAddress,msclr::interop::marshal_as<std::string>(IP);

    Boolean CompatFirmware = GlobalObjects::LMFTracker->IsCompatibleWithInstalledFirmware;
    cout << "Is Compatible With Installed Firmware: " << CompatFirmware << "\n";
//	setIntegerParam(iscompatibleFirmware, CompatFirmware); 

    String^ Name = GlobalObjects::LMFTracker->Name;
    cout << "Name: " << msclr::interop::marshal_as<std::string>(Name) << "\n";
//	setStringParam(name,msclr::interop::marshal_as<std::string>(Name);
    
    String^ ProductName = GlobalObjects::LMFTracker->ProductName;
    cout << "ProductName: " << msclr::interop::marshal_as<std::string>(ProductName) << "\n";
//	setStringParam(productName,msclr::interop::marshal_as<std::string>(ProductName);

    String^ Serial = GlobalObjects::LMFTracker->SerialNumber;
    cout << "Serial: " << msclr::interop::marshal_as<std::string>(Serial) << "\n";
//	setStringParam(serial,msclr::interop::marshal_as<std::string>(Serial);

    GlobalObjects::LMFTracker->GetDirectionAsync();    
    Direction^ dir1 = GlobalObjects::LMFTracker->GetDirection(); 
    cout << "Direction H Angle: " << dir1->HorizontalAngle->Value << " V Angle: " << dir1->VerticalAngle->Value << "\n";

// end of test commands 



/* This is just to avoid compiler warnings */


return(asynSuccess);

} 


static const iocshArg configArg0 = { "Tracker name", iocshArgString};
static const iocshArg * const configArgs[] = {&configArg0};
static const iocshFuncDef configFuncDef = {"LTAT403Config", 1, configArgs};
static void configCallFunc(const iocshArgBuf *args)
{
LTAT403Config(args[0].sval);
}

void drvLTAT403Register(void)
{
iocshRegister(&configFuncDef,configCallFunc);
}

extern "C" {
epicsExportRegistrar(drvLTAT403Register);
} 

