

#include <windows.h>
#pragma comment(lib, "User32.lib")

#include <iostream>
#include <ctime>
#include <sstream>

//laser tracker realted headers

#include <msclr\marshal.h>
#include <msclr\marshal_cppstd.h>

using namespace System;
#include "termcolor.hpp"
using namespace termcolor;

using namespace msclr::interop;
// using namespace std; 

#using <LMF.Tracker.Connection.dll>
// These are required to get correct CSharp references back that the imaging system uses

#using <PresentationCore.dll>
#using <PresentationFramework.dll>
#using <Windowsbase.dll>


using namespace LMF::Tracker;
using namespace LMF::Tracker::Compensations;
using namespace LMF::Tracker::Measurements;
using namespace LMF::Tracker::MeasurementStatus;
using namespace LMF::Tracker::MeasurementResults;
using namespace LMF::Tracker::ErrorHandling;
using namespace LMF::Tracker::Targets;
using namespace LMF::Tracker::Triggers;
using namespace LMF::Tracker::Enums;
using namespace LMF::Tracker::BasicTypes;


extern const char* EDisplayUnitSystemStrings[];
extern const char* EPowerSourceStrings[];
extern const char* EFaceStrings[];
extern const char* EMeasurementStatusStrings[];
extern const char* EUnitTypeStrings[];
extern const char* TFS[];
extern const char* EMeteoSourceStrings[];

#define CALL_AND_HANDLE(expr)           \
    try {                               \
        expr;                  \
    }                                   \
    catch (...) {                       \
        printf("Exception Trapped . . . %s at line %d \n",__FILE__,__LINE__);\
    }

//NOTE: asking for the enum string values via the Object viewer returns the strings in alphanumeric order ??? not in the order that they actually are
// You need to exaine the TLH file, to get the correct order

// forward definitions, mostly callbacks

void OnChanged(LMF::Tracker::BasicTypes::EnumTypes::ReadOnlyPowerSourceValue^ sender, LMF::Tracker::Enums::EPowerSource paramNewValue);
void OnChangeFinished(LMF::Tracker::Face^ sender, LMF::Tracker::Enums::EFace paramNewValue, LMF::Tracker::ErrorHandling::LmfException^ ex);
void OnChanged(LMF::Tracker::Face^ sender, LMF::Tracker::Enums::EFace paramNewValue);

void OnWPFBitmapImageArrived(LMF::Tracker::OVC::OverviewCamera^ sender, System::Windows::Media::Imaging::BitmapImage^ image, LMF::Tracker::OVC::ATRCoordinateCollection^ atrCoordinates);
void OnImageArrived(LMF::Tracker::OVC::OverviewCamera^ sender, array<unsigned char, 1>^% image, LMF::Tracker::OVC::ATRCoordinateCollection^ atrCoordinates);
void OnClosed(LMF::Tracker::OVC::Dialog^ sender);
void OnTriggeredMeasurementsArrived(LMF::Tracker::Measurements::Profiles::CustomTriggerProfile^ sender, LMF::Tracker::MeasurementResults::TriggeredMeasurementCollection^ paramMeasurements);
void OnChanged(LMF::Tracker::BasicTypes::EnumTypes::StartStopSourceValue^ sender, LMF::Tracker::Enums::EStartStopSource paramNewValue);
void OnChanged(LMF::Tracker::BasicTypes::EnumTypes::StartStopActiveLevelValue^ sender, LMF::Tracker::Enums::EStartStopActiveLevel paramNewValue);
void OnChanged(LMF::Tracker::BasicTypes::EnumTypes::ClockTransmissionValue^ sender, LMF::Tracker::Enums::EClockTransmission paramNewValue);
void OnChanged(LMF::Tracker::BasicTypes::EnumTypes::ClockSourceValue^ sender, LMF::Tracker::Enums::EClockSource paramNewValue);
void OnChanged(LMF::Tracker::BasicTypes::IntValue::ReadOnlyIntValue^ sender, int paramNewValue);
void OnChanged(LMF::Tracker::BasicTypes::EnumTypes::AccuracyValue^ sender, LMF::Tracker::Enums::EAccuracy paramNewValue);
void OnInclinationChanged(LMF::Tracker::Inclination::InclinationMonitoring^ sender);
void OnBubbleReadoutArrived(LMF::Tracker::Inclination::InclinationBubbleReadout^ sender, LMF::Tracker::Inclination::BubbleReadoutArrivedEventArgs^ paramBubbleReadout);
void OnGetInclinationToGravityFinished(LMF::Tracker::Inclination::InclinationSensor^ sender, LMF::Tracker::Inclination::InclinationToGravity^ paramInclinationToGravity, LMF::Tracker::ErrorHandling::LmfException^ ex);
void OnChanged(LMF::Tracker::Meteo::MeteoSource^ sender, LMF::Tracker::Enums::EMeteoSource paramNewValue);
void OnEnvironmentalValuesChanged(LMF::Tracker::Meteo::MeteoStation^ sender, double paramTemperature, double paramHumidity, double paramPressure);
void OnFinished(LMF::Tracker::Targets::TargetSearch^ sender, LMF::Tracker::Targets::Target^ foundTarget, LMF::Tracker::ErrorHandling::LmfException^ ex);


void OnChanged(LMF::Tracker::MeasurementStatus::MeasurementStatusValue^ sender, LMF::Tracker::Enums::EMeasurementStatus paramNewValue);
void OnMeasurementArrived(LMF::Tracker::Measurements::MeasurementSettings^ sender, LMF::Tracker::MeasurementResults::MeasurementCollection^ paramMeasurements, LMF::Tracker::ErrorHandling::LmfException^ paramException);
void OnWarningArrived(LMF::Tracker::Tracker^ sender, LMF::Tracker::ErrorHandling::LmfWarning^ warning);
void OnPositionToTargetFinished(LMF::Tracker::Tracker^ sender, LMF::Tracker::Targets::Target^ foundTarget, LMF::Tracker::ErrorHandling::LmfException^ ex);
void OnPositionToFinished(LMF::Tracker::Tracker^ sender, LMF::Tracker::Targets::Target^ foundTarget, LMF::Tracker::ErrorHandling::LmfException^ ex);
void OnInitializeFinished(LMF::Tracker::Tracker^ sender, LMF::Tracker::ErrorHandling::LmfException^ ex);
void OnInformationArrived(LMF::Tracker::Tracker^ sender, LMF::Tracker::ErrorHandling::LmfInformation^ paramInfo);
void OnGoHomePositionFinished(LMF::Tracker::Tracker^ sender, LMF::Tracker::ErrorHandling::LmfException^ ex);
void OnGetPrismPositionFinished(LMF::Tracker::Tracker^ sender, LMF::Tracker::MeasurementResults::Measurement^ paramMeasurement, LMF::Tracker::ErrorHandling::LmfException^ ex);
void OnGetDirectionFinished(LMF::Tracker::Tracker^ sender, LMF::Tracker::Direction^ bm, LMF::Tracker::ErrorHandling::LmfException^ ex);
void OnErrorArrived(LMF::Tracker::Tracker^ sender, LMF::Tracker::ErrorHandling::LmfError^ error);
void OnDisconnected(LMF::Tracker::Tracker^ sender, LMF::Tracker::ErrorHandling::LmfException^ ex);

void OnChanged(LMF::Tracker::BasicTypes::DoubleValue::ReadOnlyDoubleValue^ sender, double paramNewValue);
void OnChanged(LMF::Tracker::BasicTypes::BoolValue::ReadOnlyBoolValue^ sender, bool paramNewValue);

int CheckForErrors(LMF::Tracker::Tracker^ LMFTracker);
int CheckForMeasurementErrors(LMF::Tracker::Tracker^ LMFTracker);
std::string decode(System::String^ something);
void closeWindowByPartialTitle(const char* partialTitle);

// breaking functions down by Lasertracker sub class, generate all the callbacks, print the paramters that can be printed, methods will be implemented later

void Do_Compensation(LMF::Tracker::Compensations::Compensation^ thing);
void Do_Compensations(LMF::Tracker::Tracker^ LMFTracker);
void Do_Settings(LMF::Tracker::Tracker^ LMFTracker);
void Do_Targets(LMF::Tracker::Tracker^ LMFTracker);
void Do_Face(LMF::Tracker::Tracker^ LMFTracker);
void Do_WrtlBoxes(LMF::Tracker::Tracker^ LMFTracker);
void Do_Laser(LMF::Tracker::Tracker^ LMFTracker);
void Do_PowerLock(LMF::Tracker::Tracker^ LMFTracker);
void Do_PowerSource(LMF::Tracker::Tracker^ LMFTracker);
void Do_QuickRelease(LMF::Tracker::Tracker^ LMFTracker);
void Do_TargetSearch(LMF::Tracker::Tracker^ LMFTracker);
void Do_MeteoStation(LMF::Tracker::Tracker^ LMFTracker);
void Do_InclinationSensor(LMF::Tracker::Tracker^ LMFTracker);
void Do_Triggers(LMF::Tracker::Tracker^ LMFTracker);
void Do_Measurement(LMF::Tracker::Tracker^ LMFTracker);
void Do_OverviewCamera(LMF::Tracker::Tracker^ LMFTracker);
void Do_TrackerAlignment(LMF::Tracker::Tracker^ LMFTracker);

void Do_Reflector(LMF::Tracker::Targets::Reflectors::Reflector^ thing);
void Do_GenericTarget(LMF::Tracker::Targets::Target^ thing);
