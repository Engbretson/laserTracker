
#include "asynPortDriver.h"

// Laser Tracker Includes
#include <iostream>
#include <msclr\marshal.h>
#include <msclr\marshal_cppstd.h>

#define DRIVER_VERSION "1.0.0"

// a slight space saver, since I have to do this *everywhere*, But I can't marshal null strings, so have to do something else like below
// to handle the couple of special cases when this actually happens
//#define decode msclr::interop::marshal_as<std::string>


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

//ints
#define L_iscompatibleFirmwareString "iscompatibleFirmware"

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
#define L_temperatureString "temperature"
#define L_xString "x"
#define L_yString "y"
#define L_zString "z"


//void OnMeasurementArrived(LMF::Tracker::Measurements::MeasurementSettings^ sender, LMF::Tracker::MeasurementResults::MeasurementCollection^ paramMeasurements, LMF::Tracker::ErrorHandling::LmfException^ paramException);


/** Class that demonstrates the use of the asynPortDriver base class to greatly simplify the task
  * of writing an asyn port driver.
  * This class does a simple simulation of a digital oscilloscope.  It computes a waveform, computes
  * statistics on the waveform, and does callbacks with the statistics and the waveform data itself.
  * I have made the methods of this class public in order to generate doxygen documentation for them,
  * but they should really all be private. */
class LTAt403 : public asynPortDriver {
public:
    LTAt403(const char *portName);
    virtual asynStatus readInt32(asynUser *pasynUser, epicsInt32 *value);
    /* These are the methods that we override from asynPortDriver */
 //   virtual asynStatus writeInt32(asynUser *pasynUser, epicsInt32 value);
//    virtual asynStatus writeFloat64(asynUser *pasynUser, epicsFloat64 value);
//    virtual asynStatus readFloat64Array(asynUser *pasynUser, epicsFloat64 *value, size_t nElements, size_t *nIn);

    /* These are the methods that are new to this class */
//    void simTask(void);
//static
 void initializeHardware(const char *portName);
 static void myOnMeasurementArrived(LMF::Tracker::MeasurementResults::MeasurementCollection^ paramMeasurements);
//void OnMeasurementArrived(LMF::Tracker::Measurements::MeasurementSettings^ sender, LMF::Tracker::MeasurementResults::MeasurementCollection^ paramMeasurements, LMF::Tracker::ErrorHandling::LmfException^ paramException);/ static void myOnMeasurementArrived(LMF::Tracker::MeasurementResults::MeasurementCollection^);
protected:

private:
    /** Values used for pasynUser->reason, and indexes into the parameter library. */
	 int L_iscompatibleFirmware;
	
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
							

//	  void OnInformationArrived(LMF::Tracker::Tracker^ sender, LMF::Tracker::ErrorHandling::LmfInformation^ paramInfo);
//	 void OnMeasurementArrived(LMF::Tracker::Measurements::MeasurementSettings^ sender, LMF::Tracker::MeasurementResults::MeasurementCollection^ paramMeasurements, LMF::Tracker::ErrorHandling::LmfException^ paramException);
	 static void OnMeasurementArrived(LMF::Tracker::Measurements::MeasurementSettings^ sender, LMF::Tracker::MeasurementResults::MeasurementCollection^ paramMeasurements, LMF::Tracker::ErrorHandling::LmfException^ paramException);
};
#define FIRST_lsAT403_PARAM L_iscompatibleFirmware
#define LAST_lsAT403_PARAM L_z;
#define NUM_PARAMS (&LAST_lsAT403_PARAM - &FIRST_lsAT403_PARAM + 1) 

    static LTAt403* LTAt403_;

	ref class GlobalObjects {
public:
	static LMF::Tracker::Tracker^ LMFTracker;
	static Connection^ con = gcnew Connection();

};

