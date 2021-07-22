
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

/* Other local calls */

//LMF::Tracker::Tracker^ LMFTracker;
//Connection^ con;

protected: 

// Analog output parameters

///int analogOutValue_;
#define FIRST_lsAT403_PARAM analogOutValue_
#define LAST_lsAT403_PARAM analogOutValue_ 

private: 
char *comment;
char *expectedFirmware;
char *installedFirmware;
char *ipAddress;
bool iscompatibleFirmware = false;
char *name;
char *productName;
char *serialNumber;

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
///boardNum_(boardNum)
{
// Analog output parameters
///createParam(analogOutValueString, asynParamInt32, &analogOutValue_);
}






/** Configuration command, called directly or from iocsh */

extern "C" int LTAT403Config(const char *trackerName)
{ 
LTAT403 *pLTAT403 = new LTAT403(trackerName);
pLTAT403 = NULL; 

GlobalObjects::con = gcnew Connection();
GlobalObjects::LMFTracker = GlobalObjects::con->Connect(marshal_as<String^>(trackerName));

 printf("Before Initialization and before prints . . . \n");
 
  GlobalObjects::LMFTracker->Initialize();
   
 // lets do some test commands
 printf("After Initialization and before prints . . . \n");
 
 
     String^ Comment = GlobalObjects::LMFTracker->Comment;
    cout << "Comment: " << msclr::interop::marshal_as<std::string>(Comment) << "\n";   
     
    String^ Firmware = GlobalObjects::LMFTracker->ExpectedFirmware;
    cout << "Firmware: " << msclr::interop::marshal_as<std::string>(Firmware) << "\n";

    String^ InstalledFirmware = GlobalObjects::LMFTracker->InstalledFirmware;
    cout << "InstalledFirmware: " << msclr::interop::marshal_as<std::string>(InstalledFirmware) << "\n";

    String^ IP = GlobalObjects::LMFTracker->IPAddress;
    cout << "IP: " << msclr::interop::marshal_as<std::string>(IP) << "\n";

    Boolean CompatFirmware = GlobalObjects::LMFTracker->IsCompatibleWithInstalledFirmware;
    cout << "Is Compatible With Installed Firmware: " << CompatFirmware << "\n";

    String^ Name = GlobalObjects::LMFTracker->Name;
    cout << "Name: " << msclr::interop::marshal_as<std::string>(Name) << "\n";
    
    String^ ProductName = GlobalObjects::LMFTracker->ProductName;
    cout << "ProductName: " << msclr::interop::marshal_as<std::string>(ProductName) << "\n";

    String^ Serial = GlobalObjects::LMFTracker->SerialNumber;
    cout << "Serial: " << msclr::interop::marshal_as<std::string>(Serial) << "\n";

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

