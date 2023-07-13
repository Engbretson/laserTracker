#include <epicsEvent.h>
#include "ADDriver.h"

#define DRIVER_VERSION      1
#define DRIVER_REVISION     3
#define DRIVER_MODIFICATION 0

#define MIN_DELAY 1e-5

#ifdef xxx

// Laser Tracker Includes
#include <iostream>
#include <msclr\marshal.h>
#include <msclr\marshal_cppstd.h>
#include <stdio.h>

using namespace System;
using namespace msclr::interop;
using namespace std;

#using <LMF.Tracker.Connection.dll>

// These are required to get correct CSharp references back that the imaging system uses

#using <PresentationCore.dll>
#using <PresentationFramework.dll>
#using <Windowsbase.dll>
#using <System.Xaml.dll>

using namespace System::Windows::Media::Imaging;
using namespace System::Windows::Media;
using namespace System::IO;

using namespace LMF::Tracker;
using namespace LMF::Tracker::Measurements;
using namespace LMF::Tracker::MeasurementStatus;
using namespace LMF::Tracker::MeasurementResults;
using namespace LMF::Tracker::ErrorHandling;
using namespace LMF::Tracker::Targets;
using namespace LMF::Tracker::Triggers;
using namespace LMF::Tracker::Enums;
using namespace LMF::Tracker::BasicTypes;
#endif


class epicsShareClass leica : public ADDriver {
public:
    leica(const char *portName, int maxSizeX, int maxSizeY, NDDataType_t dataType,
                int maxBuffers, size_t maxMemory,
                int priority, int stackSize);
	~leica(void);

    /* These are the methods that we override from ADDriver */
    virtual asynStatus writeInt32(asynUser *pasynUser, epicsInt32 value);
    virtual asynStatus writeFloat64(asynUser *pasynUser, epicsFloat64 value);

    virtual void report(FILE *fp, int details);
    void simTask(); /**< Should be private, but gets called from C, so must be public */
//	void initializeHardware(const char* portName);

//protected:
private:
	
		/** Values used for pasynUser->reason, and indexes into the parameter library. */
	int L_iscompatibleFirmware;
	#define FIRST_LEICA_PARAM L_iscompatibleFirmware

	int L_comment;
	int L_expectedFirmware;
	int L_installedFirmware;
	int L_ipAddress;
	int L_name;
	int L_productName;
	int L_serialNumber;

	int L_angleUnits;
	int L_humidityUnits;
	int L_pressureUnits;
	int L_temperatureUnits;
	int L_xUnits;
	int L_yUnits;
	int L_zUnits;

	int L_horizontalAngle;
	int L_verticalAngle;
	int L_humidity;
	int L_pressure;
	int L_temperature;
	int L_x;
	int L_y;
	int L_z;

	int L_face;
	int L_face_command;

	int L_islaseron;
	int L_islaserwarm;
	int L_laseronoff_command;

	int L_measonoff_command;
	int L_meas_in_prog;
	#define LAST_LEICA_PARAM L_meas_in_prog;
	
#define NUM_PARAMS (&LAST_LEICA_PARAM - &FIRST_LEICA_PARAM + 1) 	


//private:
    /* These are the methods that are new to this class */
    int computeImage();

    /* Our data */
    epicsEventId startEventId_;
    epicsEventId stopEventId_;
    NDArray *pRaw_;
    NDArray *pBackground_;
    bool useBackground_;
    NDArray *pRamp_;
    NDArray *pPeak_;
    NDArrayInfo arrayInfo_;
	
//	 static void OnErrorArrived(LMF::Tracker::Tracker^ sender, LMF::Tracker::ErrorHandling::LmfError^ error);
//	 static void OnInformationArrived(LMF::Tracker::Tracker^ sender, LMF::Tracker::ErrorHandling::LmfInformation^ paramInfo);
//	 static void OnWarningArrived(LMF::Tracker::Tracker^ sender, LMF::Tracker::ErrorHandling::LmfWarning^ warning);
//	 static void OnLaserChanged(LMF::Tracker::BasicTypes::BoolValue::ReadOnlyBoolValue^ sender, bool paramNewValue);

};

#ifdef xxx
 static leica* leica_;
 
 static void OnErrorArrived(LMF::Tracker::Tracker^ sender, LMF::Tracker::ErrorHandling::LmfError^ error);
 static void OnInformationArrived(LMF::Tracker::Tracker^ sender, LMF::Tracker::ErrorHandling::LmfInformation^ paramInfo);
 static void OnWarningArrived(LMF::Tracker::Tracker^ sender, LMF::Tracker::ErrorHandling::LmfWarning^ warning);
 static void OnLaserChanged(LMF::Tracker::BasicTypes::BoolValue::ReadOnlyBoolValue^ sender, bool paramNewValue);


// Not sure if this is actually required, or if this is a carryover from other attempts that did not work without having to do this . . .
// but too lazy to remove it and see if I can live without it or not.

ref class GlobalObjects {
public:
	static LMF::Tracker::Tracker^ LMFTracker;
	static Connection^ con = gcnew Connection();
};


// a slight space saver, since I have to do this *everywhere*, But I can't marshal null strings, so have to do something else like below
// to handle the couple of special cases when this actually happens
//#define decode msclr::interop::marshal_as<std::string>

const char* EDisplayUnitSystemStrings[] = { "Metric","Imperial" }; //ok
const char* EPowerSourceStrings[] = { "Mains", "Battery", "PowerOverEthernet", "BatteryProblems" }; //ok
const char* EFaceStrings[] = { "Face1", "Face2" }; //ok
const char* EMeasurementStatusStrings[] = { "ReadyToMeasure","MeasurementInProgress","NotReady","Invalid" }; //ok
const char* EUnitTypeStrings[] = { "None", "Angle", "Humidity", "Length", "Pressure", "Temperature", "Time", "Percent" }; //ok
const char* TFS[] = { "False", "True" }; //ok
const char* EMeteoSourceStrings[] = { "ManualMeteo", "LiveMeteo" };
#endif

// end of Laser Tracker Includes

//ints/bools
#define L_iscompatibleFirmwareString "iscompatibleFirmware"

#define L_faceString "face"
#define L_face_commandString "face_command"

#define L_islaseronString "islaseron"
#define L_islaserwarmString "islaserwarm"
#define L_laseronoff_commandString "laseronoff_command"

#define L_measonoff_commandString "measonoff_command"

#define L_meas_in_progString "meas_in_prog"


//strings
#define L_commentString "comment"
#define L_expectedFirmwareString "expectedFirmware"
#define L_installedFirmwareString "installedFirmware"
#define L_ipAddressString "ipAddress"
#define L_nameString "name"
#define L_productNameString "productName"
#define L_serialNumberString "serialNumber"

#define L_angleUnitsString "angleunits"
#define L_humidityUnitsString "humidityunits"
#define L_pressureUnitsString "pressureunits"
#define L_temperatureUnitsString "temperatureunits"
#define L_xUnitsString "xunits"
#define L_yUnitsString "yunits"
#define L_zUnitsString "zunits"

//doubles
#define L_verticalAngleString "verticalAngle"
#define L_horizontalAngleString "horizontalAngle"
#define L_humidityString "humidity"
#define L_pressureString "pressure"
#define L_temperatureString "ltemperature"
#define L_xString "x"
#define L_yString "y"
#define L_zString "z"

