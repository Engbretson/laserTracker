
#pragma managed(push,off)
#include "ADDriver.h"
#include <epicsEvent.h>
#include <epicsThread.h>
#include <epicsSignal.h>
#include <epicsExit.h>
#pragma managed(pop)

#define DRIVER_VERSION      2
#define DRIVER_REVISION     1
#define DRIVER_MODIFICATION 0

#define MIN_DELAY 1e-5



////////////////////////
// Laser Tracker Specific header files 
// 

#include <stdio.h>


// Laser Tracker Includes

#include <iostream>
#include <msclr\marshal.h>
#include <msclr\marshal_cppstd.h>
#include <chrono>


using namespace System;

using namespace msclr::interop;
using namespace std;


// These are required to get correct CSharp references back that the imaging system uses
#pragma managed(push, off)
#using <PresentationCore.dll>
#using <PresentationFramework.dll>
#using <Windowsbase.dll>
#using <System.Xaml.dll>
#pragma managed(pop)

using namespace System::Windows::Media::Imaging;
using namespace System::Windows::Media;
using namespace System::Collections::Generic;
using namespace System::IO;

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


// I have to try block various things better at startup if the laser does not have access to a 
// valid tracker that I can point to at startup.

#define CALL_AND_HANDLE(expr)           \
    try {                               \
        expr;                           \
    }                                   \
    catch (...) {                       \
        printf("Exception Trapped . . . %s at line %d \n",__FILE__,__LINE__); \
    }


////////////////////////


/** Simulation detector driver; demonstrates most of the features that areaDetector drivers can support. */
class /*epicsShareClass*/ leica : public ADDriver {
public:
	leica(const char* portName, int maxSizeX, int maxSizeY, NDDataType_t dataType,
		int maxBuffers, size_t maxMemory,
		int priority, int stackSize);
	~leica(void);

	/* These are the methods that we override from ADDriver */
	virtual asynStatus readInt32(asynUser* pasynUser, epicsInt32* value);
	virtual asynStatus writeInt32(asynUser* pasynUser, epicsInt32 value);
	virtual asynStatus writeOctet(asynUser* pasynUser, const char* value,
		size_t maxChars, size_t* nActual);


	virtual void report(FILE* fp, int details);
	//   void simTask(); /**< Should be private, but gets called from C, so must be public */
	void initializeHardware(const char* portName);
	void Do_Face();
	void Do_Laser();
	void Do_Settings();
	void Do_OverviewCamera();
	void Do_PowerLock();
	void Do_PowerSource();
	void Do_QuickRelease();
	void Do_InclinationSensor();


	int L_noop;
#define L_noopString "L_noop"  
#define FIRST_LEICA_PARAM L_noop

#include "leica.inc"

	int L_noop2;
#define L_noop2String "L_noop2" 
#define LAST_LEICA_PARAM L_noop2;
#define NUM_PARAMS (&LAST_LEICA_PARAM - &FIRST_LEICA_PARAM + 1) 	

protected:

private:
	/* These are the methods that are new to this class */
//    int computeImage();

	/* Our data */
	epicsEventId startEventId_;
	epicsEventId stopEventId_;
	NDArray* pRaw_;
	NDArray* pBackground_;
	bool useBackground_;
	NDArray* pRamp_;
	NDArray* pPeak_;
	NDArrayInfo arrayInfo_;
	NDArray* pImage;

	// laser tracker related callbacks

	static void OnMeasurementArrived(LMF::Tracker::Measurements::MeasurementSettings^ sender, LMF::Tracker::MeasurementResults::MeasurementCollection^ paramMeasurements, LMF::Tracker::ErrorHandling::LmfException^ paramException);
	static void OnEnvironmentalValuesChanged(LMF::Tracker::Meteo::MeteoStation^ sender, double paramTemperature, double paramHumidity, double paramPressure);
	static void OnErrorArrived(LMF::Tracker::Tracker^ sender, LMF::Tracker::ErrorHandling::LmfError^ error);
	static void OnInformationArrived(LMF::Tracker::Tracker^ sender, LMF::Tracker::ErrorHandling::LmfInformation^ paramInfo);
	static void OnWarningArrived(LMF::Tracker::Tracker^ sender, LMF::Tracker::ErrorHandling::LmfWarning^ warning);
	static void OnChanged(LMF::Tracker::BasicTypes::DoubleValue::ReadOnlyDoubleValue^ sender, double paramNewValue);
	static void OnChanged(LMF::Tracker::BasicTypes::BoolValue::ReadOnlyBoolValue^ sender, bool paramNewValue);
	static void OnMeasChanged(LMF::Tracker::BasicTypes::BoolValue::ReadOnlyBoolValue^ sender, bool paramNewValue);
	static void OnLaserChanged(LMF::Tracker::BasicTypes::BoolValue::ReadOnlyBoolValue^ sender, bool paramNewValue);
	static void OnWPFBitmapImageArrived(LMF::Tracker::OVC::OverviewCamera^ sender, System::Windows::Media::Imaging::BitmapImage^ image, LMF::Tracker::OVC::ATRCoordinateCollection^ atrCoordinates);
	static void OnChanged(LMF::Tracker::MeasurementStatus::MeasurementStatusValue^ sender, LMF::Tracker::Enums::EMeasurementStatus paramNewValue);

	static void OnFaceChanged(LMF::Tracker::Face^ sender, LMF::Tracker::Enums::EFace paramNewValue);
	static void OnFaceChangeFinished(LMF::Tracker::Face^ sender, LMF::Tracker::Enums::EFace paramNewValue, LMF::Tracker::ErrorHandling::LmfException^ ex);

	static void OnWarmChanged(LMF::Tracker::BasicTypes::BoolValue::ReadOnlyBoolValue^ sender, bool paramNewValue);
	static void OnOnChanged(LMF::Tracker::BasicTypes::BoolValue::ReadOnlyBoolValue^ sender, bool paramNewValue);
	//    static void OnImageArrived(LMF::Tracker::OVC::OverviewCamera^ sender, array<unsigned char, 1>^% image, LMF::Tracker::OVC::ATRCoordinateCollection^ atrCoordinates);
	static void OnGetInclinationToGravityFinished(LMF::Tracker::Inclination::InclinationSensor^ sender, LMF::Tracker::Inclination::InclinationToGravity^ paramInclinationToGravity, LMF::Tracker::ErrorHandling::LmfException^ ex);
	static void OnBubbleReadoutArrived(LMF::Tracker::Inclination::InclinationBubbleReadout^ sender, LMF::Tracker::Inclination::BubbleReadoutArrivedEventArgs^ paramBubbleReadout);
	static void OnIncChanged(LMF::Tracker::BasicTypes::BoolValue::ReadOnlyBoolValue^ sender, bool paramNewValue);
	static void OnInclinationChanged(LMF::Tracker::Inclination::InclinationMonitoring^ sender);
	static void OnChanged1(LMF::Tracker::Face^ sender, LMF::Tracker::Enums::EFace paramNewValue);
	// can't do this from within epics
	//	static void OnImageArrived(LMF::Tracker::OVC::OverviewCamera^ sender, array<unsigned char, 1>^% image, LMF::Tracker::OVC::ATRCoordinateCollection^ atrCoordinates);
	static void OnBrightnessChanged(LMF::Tracker::BasicTypes::DoubleValue::ReadOnlyDoubleValue^ sender, double paramNewValue);
	static void OnDialogClosed(LMF::Tracker::OVC::Dialog^ sender);
	static void OnQuickChanged(LMF::Tracker::BasicTypes::BoolValue::ReadOnlyBoolValue^ sender, bool paramNewValue);
	static void OnPowerChanged(LMF::Tracker::BasicTypes::EnumTypes::ReadOnlyPowerSourceValue^ sender, LMF::Tracker::Enums::EPowerSource paramNewValue);
	static void OnSourceChanged(LMF::Tracker::BasicTypes::DoubleValue::ReadOnlyDoubleValue^ sender, double paramNewValue);
	static void OnPowerLockChanged(LMF::Tracker::BasicTypes::BoolValue::ReadOnlyBoolValue^ sender, bool paramNewValue);
};

static leica* leica_;


ref class GlobalObjects {
public:
	static LMF::Tracker::Tracker^ LMFTracker;
	static Connection^ con = gcnew Connection();
};

const char* EDisplayUnitSystemStrings[] = { "Metric","Imperial" }; //ok
const char* EPowerSourceStrings[] = { "Mains", "Battery", "PowerOverEthernet", "BatteryProblems" }; //ok
const char* EFaceStrings[] = { "Face1", "Face2" }; //ok
const char* EMeasurementStatusStrings[] = { "ReadyToMeasure","MeasurementInProgress","NotReady","Invalid" }; //ok
const char* EUnitTypeStrings[] = { "None", "Angle", "Humidity", "Length", "Pressure", "Temperature", "Time", "Percent" }; //ok
const char* TFS[] = { "False", "True" }; //ok
const char* EMeteoSourceStrings[] = { "ManualMeteo", "LiveMeteo" };
