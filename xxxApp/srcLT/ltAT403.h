
#include "asynPortDriver.h"

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

#define L_iscompatibleFirmwareString "iscompatibleFirmware"

#define L_commentString "comment"
#define L_expectedFirmwareString "expectedFirmware"
#define L_installedFirmwareString "installedFirmware"
#define L_ipAddressString "ipAddress"
#define L_nameString "name"
#define L_productNameString "productName"
#define L_serialNumberString "serialNumber"

#define L_verticalAngleString "verticalAngle"
#define L_horizontalAngleString "horizontalAngle"



ref class GlobalObjects {
public:

static LMF::Tracker::Tracker^ LMFTracker;   
static Connection^ con = gcnew Connection();

};


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
static void initializeHardware(const char *portName);


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
	
	int L_horizontalAngle;
	int L_verticalAngle;

};
#define FIRST_lsAT403_PARAM comment
#define LAST_lsAT403_PARAM serialNumber
#define NUM_PARAMS (&LAST_lsAT403_PARAM - &FIRST_lsAT403_PARAM + 1) 
