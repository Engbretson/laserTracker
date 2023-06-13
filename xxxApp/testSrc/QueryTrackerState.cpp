//
// LaserTracker.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <windows.h>
#pragma comment(lib, "User32.lib")

#include <iostream>
#include <ctime>
#include <sstream>


//laser tracker realted headers


#include <msclr\marshal.h>
#include <msclr\marshal_cppstd.h>


using namespace System;
using namespace msclr::interop;
using namespace std;

#using <LMF.Tracker.Connection.dll>

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


//NOTE: asking for the enum string values via the Object viewer returns the strings in alphanumeric order ??? not in the order that they actually are
// You need to exaine the TLH file, to get the correct order

void OnInclinationChanged(LMF::Tracker::Inclination::InclinationMonitoring^ sender);
void OnBubbleReadoutArrived(LMF::Tracker::Inclination::InclinationBubbleReadout^ sender, LMF::Tracker::Inclination::BubbleReadoutArrivedEventArgs^ paramBubbleReadout);
void OnGetInclinationToGravityFinished(LMF::Tracker::Inclination::InclinationSensor^ sender, LMF::Tracker::Inclination::InclinationToGravity^ paramInclinationToGravity, LMF::Tracker::ErrorHandling::LmfException^ ex);
void OnChanged(LMF::Tracker::Meteo::MeteoSource^ sender, LMF::Tracker::Enums::EMeteoSource paramNewValue);
void OnEnvironmentalValuesChanged(LMF::Tracker::Meteo::MeteoStation^ sender, double paramTemperature, double paramHumidity, double paramPressure);
void OnFinished(LMF::Tracker::Targets::TargetSearch^ sender, LMF::Tracker::Targets::Target^ foundTarget, LMF::Tracker::ErrorHandling::LmfException^ ex);

const char* EDisplayUnitSystemStrings[] = { "Metric","Imperial" }; //ok
const char* EPowerSourceStrings[] = { "Mains", "Battery", "PowerOverEthernet", "BatteryProblems" }; //ok
const char* EFaceStrings[] = { "Face1", "Face2" }; //ok
const char* EMeasurementStatusStrings[] = { "ReadyToMeasure","MeasurementInProgress","NotReady","Invalid" }; //ok
const char* EUnitTypeStrings[] = { "None", "Angle", "Humidity", "Length", "Ppressure", "Temperature", "Time", "Percent" }; //ok
const char* TFS[] = { "False", "True" }; //ok
const char* EMeteoSourceStrings[]= { "ManualMeteo", "LiveMeteo"};


void OnChanged(LMF::Tracker::BasicTypes::EnumTypes::ReadOnlyPowerSourceValue^ sender, LMF::Tracker::Enums::EPowerSource paramNewValue);
void OnChangeFinished(LMF::Tracker::Face^ sender, LMF::Tracker::Enums::EFace paramNewValue, LMF::Tracker::ErrorHandling::LmfException^ ex);
void OnChanged(LMF::Tracker::Face^ sender, LMF::Tracker::Enums::EFace paramNewValue);

void closeWindowByTitle(const char* title) {
	HWND window = FindWindowA(NULL, title);
	if (window != NULL) {
		//       PostMessage(window, WM_CLOSE, 0, 0);
		ShowWindow(window, SW_HIDE);
	}
}

void closeWindowByPartialTitle(const char* partialTitle) {
	HWND window = FindWindowA(NULL, NULL);
	while (window != NULL) {
		char title[1024];
		GetWindowTextA(window, title, sizeof(title));
		if (strstr(title, partialTitle) != NULL) {
			//            PostMessage(window, WM_CLOSE, 0, 0);
			ShowWindow(window, SW_HIDE);
		}
		window = FindWindowEx(NULL, window, NULL, NULL);
	}
}

// a slight space saver, since I have to do this *everywhere*, But I can't marshal null strings, so have to do something else like below
// to handle the couple of special cases when this actually happens
//#define decode msclr::interop::marshal_as<std::string>

std::string decode(System::String^ something)
{
	if (System::String::IsNullOrEmpty(something))
		//		return "N/A";
		return " ";
	else
		return (msclr::interop::marshal_as<std::string>(something));
}


// forward definitions, mostly callbacks

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

// common printable entity templates ??
/*
void Do_DV_ReadOnlyDoubleValue(String^ Title, LMF::Tracker::BasicTypes::DoubleValue::ReadOnlyDoubleValue^ thing)
{
//	thing += 
	cout << (decode)(Title) << ": Label: " << (decode)(thing->Label) <<
		" UnitString: " << (decode)(thing->UnitString) <<
		" UnitType: " << EUnitTypeStrings[(int)thing->UnitType] <<
		" UnitType: " << (int)thing->UnitType <<
		" Value: " << thing->Value <<
		" ValueInBaseUnits: " << thing->ValueInBaseUnits <<
		endl;

}
*/

/*
LMF.Tracker.BasicTypes.DoubleValue.ReadOnlyDoubleValue
LMF.Tracker.BasicTypes.DoubleValue.DoubleValue
LMF.Tracker.BasicTypes.DoubleValue.DoubleValueWithRange

LMF.Tracker.BasicTypes.BoolValue.BoolValue
LMF.Tracker.BasicTypes.BoolValue.ReadOnlyBoolValue

LMF.Tracker.Inclination.InclinationMeasurement
LMF.Tracker.Meteo.EnvironmentalSensor
LMF.Tracker.Meteo.ManualEnvironmentalSensor
*/


int CheckForErrors(LMF::Tracker::Tracker^ LMFTracker)
{
	// If and when something throws an error, this is how to decode it 
	Int32 ErrorNumber = -1;

	try {
		LmfError^ err = LMFTracker->GetErrorDescription(ErrorNumber);
		if ((err->Number > 0) && (err->Number != 200069)) {
			cout << "Is anything throwing an error code? \n";
			cout << (decode)(err->Description) << " " <<
				err->Number << " " <<
				(decode)(err->Solution) << " " <<
				(decode)(err->Title) << "\n";
			return err->Number;
		}
	}
	catch (LMF::Tracker::ErrorHandling::LmfException^ e)
	{
		cout << "Error exception " << e->Number << " " << (decode)(e->Description) << "\n";;
		//		cout << "Hit an exception trying to decode Check For Errors  \n";
	}

	return ErrorNumber;

}

int  CheckForMeasurementErrors(LMF::Tracker::Tracker^ LMFTracker)
{
	LMF::Tracker::Enums::EMeasurementStatus statusValue = LMFTracker->Measurement->Status->Value;

	cout << EMeasurementStatusStrings[(int)statusValue] << ". . ." << endl;

	//	if (statusValue == EMeasurementStatus::ReadyToMeasure) { cout << "Ready To Measure . . . \n"; }
	//	if (statusValue == EMeasurementStatus::MeasurementInProgress) { cout << "Measurement in Progress . . . \n"; }
	//	if (statusValue == EMeasurementStatus::NotReady) { cout << "Not Ready to Measure . . . \n"; }
	//	if (statusValue == EMeasurementStatus::Invalid) { cout << "Measurement Status Invalid . . . \n"; }

	if (statusValue == EMeasurementStatus::NotReady)
	{
		LMF::Tracker::MeasurementStatus::MeasurementPreconditionCollection^ Preconditions = LMFTracker->Measurement->Status->Preconditions;
		for (int i = 0; i < Preconditions->Count; ++i) {
			LMF::Tracker::MeasurementStatus::MeasurementPrecondition^ firstPrecondition = Preconditions[0];
			cout << (decode)(firstPrecondition->Title) << " " << (decode)(firstPrecondition->Description) << " " << (decode)(firstPrecondition->Solution) << "\n";
		}
	}
	return (int)statusValue;

}


int main()
{

	/*
	The steps that this new test program will follow
	.1) Create the laser Tracker Object
	.2) Do a check to see if Any lasrer trackers  are found via search
	.3) Connect to a Simulated tracker or a real tracker
	.4) Initialze the Hardware
	.5) Return simple top level values
	6) do

	// Do commands that return reasonable values if there is no valid target So Read values,
	// next test program actually moves things and writes values
	// epics PV's can start being generated after the first pass of readable values exist

	A) List Compensations
	B) List Face
	C) List InclinationSensor
	D) List Laser
	E) ?? Measurement Status
	F) MeteoStaion
	G) ?? OverViewCamera could be useful finding targets
	H) List Powerlock
	I) List PowerSource
	J) List QuickRelease
	K) List Tagets
	L) ?? list Triggers
	M) ?? List WrtlBoxes

	7) Clean shutdown

	*/

	// 1

	LMF::Tracker::Tracker^ LMFTracker;
	Connection^ con = gcnew Connection();

	// 2	

	cout << "Searching for trackers . . . ";

	// The TrackerFinder holds a list of found Trackers, but doesn't seem to find any when the hardware is on a private address
	// hard encoding a static address  . . . doesn't seem to work, but dhcp on the last device *did* work, I think

	TrackerFinder^ trackerFinder = gcnew TrackerFinder();
	TrackerInfoCollection^ foundTrackers = trackerFinder->Trackers;

	cout << "Found : " << foundTrackers->Count << "\n";

	String^ UseLastTracker;

	for (int i = 0; i < foundTrackers->Count; i++)
	{
		TrackerInfo^ tracker = foundTrackers[i];

		cout << " Tracker Name: " << (decode)(tracker->Name);
		cout << " Serial Number: " << (decode)(tracker->SerialNumber);
		cout << " IP Address: " << (decode)(tracker->IPAddress);
		UseLastTracker = tracker->IPAddress;
		cout << " Type: " << (decode)(tracker->Type) << "\n";
	}

	// 3

		// Possible simulators, according to some of the docs

			//	LMFTracker = con->Connect("AT401Simulator");
			//	LMFTracker = con->Connect("AT402Simulator");
			//	LMFTracker = con->Connect("AT403Simulator");
			//	LMFTracker = con->Connect("AT500Simulator"); 
			//	LMFTracker = con->Connect("ATS600Simulator"); 
			//	LMFTracker = con->Connect("AT901LRSimulator");
			//	LMFTracker = con->Connect("AT960MRSimulator"); 
			//	LMFTracker = con->Connect("AT960LRSimulator"); 
			//	LMFTracker = con->Connect("AT930Simulator");

	//	cout << "Connecting to At403Simulator \n";
	//	cout << "Connecting to At930Simulator \n";
	//	cout << "Connecting to 164.54.116.53 \n";
	//	cout << "Connecting to " << (decode)(UseLastTracker) < "\n";

	//	LMFTracker = con->Connect("At403Simulator");
	//	LMFTracker = con->Connect("At930Simulator");

	try {
		//	LMFTracker = con->Connect("164.54.116.53");
		LMFTracker = con->Connect(UseLastTracker);
	}
	catch (LMF::Tracker::ErrorHandling::LmfException^ e)
	{
		cout << "Error code: " << e->Number << " " << (decode)(e->Description) << "\n";;
		//		cout << "Hit an exception trying to perform a Connect call, Exiting. \n";

		//		exit(-1);
		cout << "No actual Hardware seen . . . . using Simulator \n";
		LMFTracker = con->Connect("At930Simulator");
	}

	////
	//// I do not want the Simulator Control screen up at all, even when talking to Simulated hardware
	//// This does not apply at all to real hardware 
	////

	//  closeWindowByTitle("AT403 Simulator 1.8.0.2250");
	//	closeWindowByTitle("AT930 Simulator 1.8.0.2250");
	// Not sure what a normal laser tracker might show , maybe key on the SDK value?

	closeWindowByPartialTitle(" Simulator ");

	// Top Level Callbacks

	LMFTracker->InformationArrived += gcnew LMF::Tracker::Tracker::InformationArrivedHandler(&OnInformationArrived);
	LMFTracker->WarningArrived += gcnew LMF::Tracker::Tracker::WarningArrivedHandler(&OnWarningArrived);
	LMFTracker->ErrorArrived += gcnew LMF::Tracker::Tracker::ErrorArrivedHandler(&OnErrorArrived);
	LMFTracker->InitializeFinished += gcnew LMF::Tracker::Tracker::InitializeFinishedHandler(&OnInitializeFinished);
	LMFTracker->GetPrismPositionFinished += gcnew LMF::Tracker::Tracker::GetPrismPositionFinishedHandler(&OnGetPrismPositionFinished);
	LMFTracker->GetDirectionFinished += gcnew LMF::Tracker::Tracker::GetDirectionFinishedHandler(&OnGetDirectionFinished);
	LMFTracker->GoHomePositionFinished += gcnew LMF::Tracker::Tracker::GoHomePositionFinishedHandler(&OnGoHomePositionFinished);
	LMFTracker->PositionToFinished += gcnew LMF::Tracker::Tracker::PositionToFinishedHandler(&OnPositionToFinished);
	LMFTracker->PositionToTargetFinished += gcnew LMF::Tracker::Tracker::PositionToFinishedHandler(&OnPositionToTargetFinished);
	LMFTracker->Disconnected += gcnew LMF::Tracker::Tracker::DisconnectedHandler(&OnDisconnected);

	// 4

	cout << "Is the hardware ready ?\n";

	CheckForErrors(LMFTracker);

	// not valid here, yet
	//	CheckForMeasurementErrors(LMFTracker);


	// This actually gets tricky , since none of this code actually works correctly if the device is warming up 
	// You can initialze, so can't move anything, or do anything target related
	// do ideally, you need to just sit here and wait and retry until it is . . .  happy?
	// the docs say that this could be as long as 2 *hours*, or more typically 5 7 minutes
	//


	cout << "Initialize . . . \n";

	// depricated warning that actually make the compiler and/pr Intellicade very unhappy . . . 

#pragma warning(disable : 4996)

	time_t rawtime;
	struct tm* timeinfo;
	time(&rawtime);
	timeinfo = localtime(&rawtime);
	cout << "Current Day, Date and Time is = " << asctime(timeinfo);


	int check = 1;
	while (check) {
		try {
			check = 0;
			LMFTracker->Initialize();
		}
		catch (LMF::Tracker::ErrorHandling::LmfException^ e)
		{
			check = 1;
			cout << "Initialization Error Code: " << e->Number << " " << (decode)(e->Description) << "\n";
			//		cout << "Hit an exception trying to perform an Initialize  call \n";
			//		cout << "waiting for no Initialze exceptions . . . .\n";
			Sleep(15000);

		}

	}

	timeinfo = localtime(&rawtime);
	cout << "Current Day, Date and Time is = " << asctime(timeinfo);

	// do 1 or the other - trying both queues up the asyn to try happening *after* the current program ends and various code faults out with 
	//	"Initialization not done yet" errors
	/*
		cout << "Initialize Async . . . \n";
		try {
			LMFTracker->InitializeAsync();
		}
		catch (LMF::Tracker::ErrorHandling::LmfException^ e)
		{
			cout << "Error code: " << e->Number << " " << (decode)(e->Description) << "\n";
			cout << "Hit an exception trying to perform an Initilize Async call \n";
		}
	*/
	cout << "After Initialization . . . Check for Errors? \n\n";

	CheckForErrors(LMFTracker);

	// not valid code to check, yet
	//	CheckForMeasurementErrors(LMFTracker);

	// 5

	cout << "Top Level  Parameters . . . \n";

	String^ Comment = LMFTracker->Comment;
	cout << "Comment: " << (decode)(Comment) << "\n";

	String^ Firmware = LMFTracker->ExpectedFirmware;
	cout << "ExpectedFirmware: " << (decode)(Firmware) << "\n";

	String^ InstalledFirmware = LMFTracker->InstalledFirmware;
	cout << "InstalledFirmware: " << (decode)(InstalledFirmware) << "\n";

	String^ IP = LMFTracker->IPAddress;
	cout << "IPAddress: " << (decode)(IP) << "\n";

	Boolean CompatFirmware = LMFTracker->IsCompatibleWithInstalledFirmware;
	cout << "IsCompatibleWithInstalledFirmware: " << TFS[CompatFirmware] << "\n";

	String^ Name = LMFTracker->Name;
	cout << "Name: " << (decode)(Name) << "\n";

	String^ ProductName = LMFTracker->ProductName;
	cout << "ProductName: " << (decode)(ProductName) << "\n";

	String^ Serial = LMFTracker->SerialNumber;
	cout << "SerialNumber: " << (decode)(Serial) << "\n\n";

	// Top Level additional Info

	// Get Direction
	// Get Direction Async
	// Get Error Description (probably typically used betwen/after every command)
	// Get Prism Position - Nope, this actually requires a locked on target and measureming
	/*
		cout << "Attempting a GetPrismPosition call \n";

		try
		{
					Measurement^ measure = LMFTracker->GetPrismPosition(); //says not supported by this tracker
					cout << "Prism Position Humidity: " << measure->Humidity->Value << " Pressure: " << measure->Pressure->Value << " Temperature: " << measure->Temperature->Value << "\n";
		}
		catch (LMF::Tracker::ErrorHandling::LmfException^ e)
		{
			cout << "Error code: " << e->Number << " " << (decode)(e->Description) << "\n";
			cout << "Hit an exception trying to perform a Get Prism Position  call \n";
		}


	// Get Prism Position Async

		cout << "Attempting a GetPrismPositionAsync call \n";

		try
		{
			LMFTracker->GetPrismPositionAsync();
		}
		catch (LMF::Tracker::ErrorHandling::LmfException^ e)
		{
			cout << "Error code: " << e->Number << " " << (decode)(e->Description) << "\n";
			cout << "Hit an exception trying to perform a Get Prism Position Async call \n";
		}

		Sleep(1000);

	*/

	Do_Compensations(LMFTracker);

	Do_Face(LMFTracker);



	//C) List InclinationSensor
	// can not do until the thing can be leveled
	Do_InclinationSensor(LMFTracker);

	//D) List Laser
	Do_Laser(LMFTracker);

	// 
	//E) ?? Measurement Status
	//F) MeteoStaion
	Do_MeteoStation(LMFTracker);

	//G) ?? OverViewCamera could be useful finding targets
	//H) List Powerlock
	Do_PowerLock(LMFTracker);

	//I) List PowerSource
	Do_PowerSource(LMFTracker);
	//J) List QuickRelease
	Do_QuickRelease(LMFTracker);
	//K) Settings

	Do_Settings(LMFTracker);

	//K) List Tagets

	Do_Targets(LMFTracker);
	//TargetSearch
	Do_TargetSearch(LMFTracker);
	//TrackerAlignment

	//L) ?? list Triggers
	//M) ?? List WrtlBoxes

	Do_WrtlBoxes(LMFTracker);



	// This actually power downs the Unit!!!!!!!!	
	//	cout << "Shutdown . . . \n";
	//	LMFTracker->ShutDown();

	cout << "Disconnect . . . \n";
	LMFTracker->Disconnect();
	return 0;
}

void Do_InclinationSensor(LMF::Tracker::Tracker^ LMFTracker)
{
	cout << endl;
	cout << "InclinationSensor\n";

	LMFTracker->InclinationSensor->GetInclinationToGravityFinished += gcnew LMF::Tracker::Inclination::InclinationSensor::GetInclinationToGravityFinishedHandler(&OnGetInclinationToGravityFinished);

	// methods
	/*
		LMFTracker->InclinationSensor->GetInclinationToGravity();
		LMFTracker->InclinationSensor->GetInclinationToGravityAsync();
		LMFTracker->InclinationSensor->Measure();
	*/

	LMFTracker->InclinationSensor->BubbleReadout->BubbleReadoutArrived += gcnew LMF::Tracker::Inclination::InclinationBubbleReadout::BubbleReadoutArrivedHandler(&OnBubbleReadoutArrived);
//methods 
/*
	LMFTracker->InclinationSensor->BubbleReadout->StartBubbleReadoutStream();
	LMFTracker->InclinationSensor->BubbleReadout->StopBubbleReadoutStream();
*/
	DateTime^ dt;
	dt = LMFTracker->InclinationSensor->CurrentInclinationToGravity->TimeStamp;
	cout << "CurrentInclinationToGravity: TimeStamp: " << (decode)(dt->ToString("dddd, dd. MMMM yyyy HH:mm:ss.fff")) << "\n";

	cout << "InclinationRotX: Label: " << (decode)(LMFTracker->InclinationSensor->CurrentInclinationToGravity->InclinationRotX->Label) <<
		" UnitString: " << (decode)(LMFTracker->InclinationSensor->CurrentInclinationToGravity->InclinationRotX->UnitString) <<
		" UnitType: " << EUnitTypeStrings[(int)LMFTracker->InclinationSensor->CurrentInclinationToGravity->InclinationRotX->UnitType] <<
		" Value: " << LMFTracker->InclinationSensor->CurrentInclinationToGravity->InclinationRotX->Value <<
		" ValueInBaseUnits: " << LMFTracker->InclinationSensor->CurrentInclinationToGravity->InclinationRotX->ValueInBaseUnits <<
		endl;

	cout << "InclinationRotY: Label: " << (decode)(LMFTracker->InclinationSensor->CurrentInclinationToGravity->InclinationRotY->Label) <<
		" UnitString: " << (decode)(LMFTracker->InclinationSensor->CurrentInclinationToGravity->InclinationRotY->UnitString) <<
		" UnitType: " << EUnitTypeStrings[(int)LMFTracker->InclinationSensor->CurrentInclinationToGravity->InclinationRotY->UnitType] <<
		" Value: " << LMFTracker->InclinationSensor->CurrentInclinationToGravity->InclinationRotY->Value <<
		" ValueInBaseUnits: " << LMFTracker->InclinationSensor->CurrentInclinationToGravity->InclinationRotY->ValueInBaseUnits <<
		endl;

	LMF::Tracker::BasicTypes::BoolValue::BoolValue^ me = LMFTracker->InclinationSensor->InclinedToGravity;
	

	LMFTracker->InclinationSensor->InclinedToGravity->Changed += gcnew LMF::Tracker::BasicTypes::BoolValue::ReadOnlyBoolValue::ChangedEventHandler(&OnChanged);

//	cout << "InclinationRotY: Label: " << (decode)(LMFTracker->InclinationSensor->InclinedToGravity->Label) <<
//		" Value: " << (int)LMFTracker->InclinationSensor->InclinedToGravity->Value <<
		cout << endl;

	LMFTracker->InclinationSensor->Monitoring->InclinationChanged += gcnew LMF::Tracker::Inclination::InclinationMonitoring::InclinationChangedHandler(&OnInclinationChanged);


	cout << endl;

}

void Do_MeteoStation(LMF::Tracker::Tracker^ LMFTracker)
{
	cout << endl;
	cout << "MeteoStation\n";

	LMFTracker->MeteoStation->EnvironmentalValuesChanged += gcnew LMF::Tracker::Meteo::MeteoStation::EnvironmentalValuesChangedEventHandler(&OnEnvironmentalValuesChanged);

	LMFTracker->MeteoStation->HardwareHumidity->Changed += gcnew LMF::Tracker::BasicTypes::DoubleValue::ReadOnlyDoubleValue::ChangedEventHandler(&OnChanged);
	LMFTracker->MeteoStation->HardwarePressure->Changed += gcnew LMF::Tracker::BasicTypes::DoubleValue::ReadOnlyDoubleValue::ChangedEventHandler(&OnChanged);
	LMFTracker->MeteoStation->HardwareTemperature->Changed += gcnew LMF::Tracker::BasicTypes::DoubleValue::ReadOnlyDoubleValue::ChangedEventHandler(&OnChanged);
	LMFTracker->MeteoStation->IsAirSensorConnected->Changed += gcnew LMF::Tracker::BasicTypes::BoolValue::ReadOnlyBoolValue::ChangedEventHandler(&OnChanged);

	LMFTracker->MeteoStation->ManualHumidity->Changed += gcnew LMF::Tracker::BasicTypes::DoubleValue::ReadOnlyDoubleValue::ChangedEventHandler(&OnChanged);
	LMFTracker->MeteoStation->ManualPressure->Changed += gcnew LMF::Tracker::BasicTypes::DoubleValue::ReadOnlyDoubleValue::ChangedEventHandler(&OnChanged);
	LMFTracker->MeteoStation->ManualTemperature->Changed += gcnew LMF::Tracker::BasicTypes::DoubleValue::ReadOnlyDoubleValue::ChangedEventHandler(&OnChanged);
	LMFTracker->MeteoStation->ObjectTemperature->Changed += gcnew LMF::Tracker::BasicTypes::DoubleValue::ReadOnlyDoubleValue::ChangedEventHandler(&OnChanged);
	LMFTracker->MeteoStation->Source->Changed += gcnew LMF::Tracker::Meteo::MeteoSource::ChangedEventHandler(&OnChanged);

	cout << "HardwareHumidity: Label: " << (decode)(LMFTracker->MeteoStation->HardwareHumidity->Label) <<
		" SerialNumber: " << (decode)(LMFTracker->MeteoStation->HardwareHumidity->SerialNumber) <<
		" UnitString: " << (decode)(LMFTracker->MeteoStation->HardwareHumidity->UnitString) <<
		" UnitType: " << EUnitTypeStrings[(int)LMFTracker->MeteoStation->HardwareHumidity->UnitType] <<
		" Value: " << LMFTracker->MeteoStation->HardwareHumidity->Value <<
		" ValueInBaseUnits: " << LMFTracker->MeteoStation->HardwareHumidity->ValueInBaseUnits <<
		endl;

	LMFTracker->MeteoStation->HardwareHumidity->Available->Changed += gcnew LMF::Tracker::BasicTypes::BoolValue::ReadOnlyBoolValue::ChangedEventHandler(&OnChanged);

	cout << "Available: " << (decode)(LMFTracker->MeteoStation->HardwareHumidity->Available->Label) <<
		" Value: " << TFS[(int)LMFTracker->MeteoStation->HardwareHumidity->Available->Value] <<
		endl;

	cout << "HardwarePressure: Label: " << (decode)(LMFTracker->MeteoStation->HardwarePressure->Label) <<
		" SerialNumber: " << (decode)(LMFTracker->MeteoStation->HardwarePressure->SerialNumber) <<
		" UnitString: " << (decode)(LMFTracker->MeteoStation->HardwarePressure->UnitString) <<
		" UnitType: " << EUnitTypeStrings[(int)LMFTracker->MeteoStation->HardwarePressure->UnitType] <<
		" Value: " << LMFTracker->MeteoStation->HardwarePressure->Value <<
		" ValueInBaseUnits: " << LMFTracker->MeteoStation->HardwarePressure->ValueInBaseUnits <<
		endl;

	LMFTracker->MeteoStation->HardwarePressure->Available->Changed += gcnew LMF::Tracker::BasicTypes::BoolValue::ReadOnlyBoolValue::ChangedEventHandler(&OnChanged);

	cout << "Available: " << (decode)(LMFTracker->MeteoStation->HardwarePressure->Available->Label) <<
		" Value: " << TFS[(int)LMFTracker->MeteoStation->HardwarePressure->Available->Value] <<
		endl;

	cout << "HardwareTemperature: Label: " << (decode)(LMFTracker->MeteoStation->HardwareTemperature->Label) <<
		" SerialNumber: " << (decode)(LMFTracker->MeteoStation->HardwareTemperature->SerialNumber) <<
		" UnitString: " << (decode)(LMFTracker->MeteoStation->HardwareTemperature->UnitString) <<
		" UnitType: " << EUnitTypeStrings[(int)LMFTracker->MeteoStation->HardwareTemperature->UnitType] <<
		" Value: " << LMFTracker->MeteoStation->HardwareTemperature->Value <<
		" ValueInBaseUnits: " << LMFTracker->MeteoStation->HardwareTemperature->ValueInBaseUnits <<
		endl;

	LMFTracker->MeteoStation->HardwareTemperature->Available->Changed += gcnew LMF::Tracker::BasicTypes::BoolValue::ReadOnlyBoolValue::ChangedEventHandler(&OnChanged);

	cout << "Available: " << (decode)(LMFTracker->MeteoStation->HardwareTemperature->Available->Label) <<
		" Value: " << TFS[(int)LMFTracker->MeteoStation->HardwareTemperature->Available->Value] <<
		endl;

	LMFTracker->MeteoStation->IsAirSensorConnected->Changed += gcnew LMF::Tracker::BasicTypes::BoolValue::ReadOnlyBoolValue::ChangedEventHandler(&OnChanged);
	cout << "IsAirSensorConnected: Label: " << (decode)(LMFTracker->MeteoStation->IsAirSensorConnected->Label) <<
		" Value: " << TFS[(int)LMFTracker->MeteoStation->IsAirSensorConnected->Value] <<
		endl;

	//
	cout << "ManualHumidity: Label: " << (decode)(LMFTracker->MeteoStation->ManualHumidity->Label) <<

		// extra fields on manual readouts

		" MaxValue: " << LMFTracker->MeteoStation->ManualHumidity->MaxValue <<
		" MaxValueInBaseUnits: " << LMFTracker->MeteoStation->ManualHumidity->MaxValueInBaseUnits <<
		" MinValue: " << LMFTracker->MeteoStation->ManualHumidity->MinValue <<
		" MinValueInBaseUnits: " << LMFTracker->MeteoStation->ManualHumidity->MinValueInBaseUnits <<


		" SerialNumber: " << (decode)(LMFTracker->MeteoStation->ManualHumidity->SerialNumber) <<
		" UnitString: " << (decode)(LMFTracker->MeteoStation->ManualHumidity->UnitString) <<
		" UnitType: " << EUnitTypeStrings[(int)LMFTracker->MeteoStation->ManualHumidity->UnitType] <<
		" Value: " << LMFTracker->MeteoStation->ManualHumidity->Value <<
		" ValueInBaseUnits: " << LMFTracker->MeteoStation->ManualHumidity->ValueInBaseUnits <<
		endl;

	LMFTracker->MeteoStation->ManualHumidity->Available->Changed += gcnew LMF::Tracker::BasicTypes::BoolValue::ReadOnlyBoolValue::ChangedEventHandler(&OnChanged);

	cout << "Available: " << (decode)(LMFTracker->MeteoStation->ManualHumidity->Available->Label) <<
		" Value: " << TFS[(int)LMFTracker->MeteoStation->ManualHumidity->Available->Value] <<
		endl;

	cout << "ManualPressure: Label: " << (decode)(LMFTracker->MeteoStation->ManualPressure->Label) <<

		// extra fields on manual readouts

		" MaxValue: " << LMFTracker->MeteoStation->ManualPressure->MaxValue <<
		" MaxValueInBaseUnits: " << LMFTracker->MeteoStation->ManualPressure->MaxValueInBaseUnits <<
		" MinValue: " << LMFTracker->MeteoStation->ManualPressure->MinValue <<
		" MinValueInBaseUnits: " << LMFTracker->MeteoStation->ManualPressure->MinValueInBaseUnits <<

		" SerialNumber: " << (decode)(LMFTracker->MeteoStation->ManualPressure->SerialNumber) <<
		" UnitString: " << (decode)(LMFTracker->MeteoStation->ManualPressure->UnitString) <<
		" UnitType: " << EUnitTypeStrings[(int)LMFTracker->MeteoStation->ManualPressure->UnitType] <<
		" Value: " << LMFTracker->MeteoStation->ManualPressure->Value <<
		" ValueInBaseUnits: " << LMFTracker->MeteoStation->ManualPressure->ValueInBaseUnits <<
		endl;

	LMFTracker->MeteoStation->ManualPressure->Available->Changed += gcnew LMF::Tracker::BasicTypes::BoolValue::ReadOnlyBoolValue::ChangedEventHandler(&OnChanged);

	cout << "Available: " << (decode)(LMFTracker->MeteoStation->ManualPressure->Available->Label) <<
		" Value: " << TFS[(int)LMFTracker->MeteoStation->ManualPressure->Available->Value] <<
		endl;

	cout << "ManualTemperature: Label: " << (decode)(LMFTracker->MeteoStation->ManualTemperature->Label) <<

		// extra fields on manual readouts

		" MaxValue: " << LMFTracker->MeteoStation->ManualTemperature->MaxValue <<
		" MaxValueInBaseUnits: " << LMFTracker->MeteoStation->ManualTemperature->MaxValueInBaseUnits <<
		" MinValue: " << LMFTracker->MeteoStation->ManualTemperature->MinValue <<
		" MinValueInBaseUnits: " << LMFTracker->MeteoStation->ManualTemperature->MinValueInBaseUnits <<


		" SerialNumber: " << (decode)(LMFTracker->MeteoStation->ManualTemperature->SerialNumber) <<
		" UnitString: " << (decode)(LMFTracker->MeteoStation->ManualTemperature->UnitString) <<
		" UnitType: " << EUnitTypeStrings[(int)LMFTracker->MeteoStation->ManualTemperature->UnitType] <<
		" Value: " << LMFTracker->MeteoStation->ManualTemperature->Value <<
		" ValueInBaseUnits: " << LMFTracker->MeteoStation->ManualTemperature->ValueInBaseUnits <<
		endl;

	LMFTracker->MeteoStation->ManualTemperature->Available->Changed += gcnew LMF::Tracker::BasicTypes::BoolValue::ReadOnlyBoolValue::ChangedEventHandler(&OnChanged);

	cout << "Available: " << (decode)(LMFTracker->MeteoStation->ManualTemperature->Available->Label) <<
		" Value: " << TFS[(int)LMFTracker->MeteoStation->ManualTemperature->Available->Value] <<
		endl;

	cout << "ObjectTemperature: Label: " << (decode)(LMFTracker->MeteoStation->ObjectTemperature->Label) <<
		" SerialNumber: " << (decode)(LMFTracker->MeteoStation->ObjectTemperature->SerialNumber) <<
		" UnitString: " << (decode)(LMFTracker->MeteoStation->ObjectTemperature->UnitString) <<
		" UnitType: " << EUnitTypeStrings[(int)LMFTracker->MeteoStation->ObjectTemperature->UnitType] <<
		" Value: " << LMFTracker->MeteoStation->ObjectTemperature->Value <<
		" ValueInBaseUnits: " << LMFTracker->MeteoStation->ObjectTemperature->ValueInBaseUnits <<
		endl;

	LMFTracker->MeteoStation->ObjectTemperature->Available->Changed += gcnew LMF::Tracker::BasicTypes::BoolValue::ReadOnlyBoolValue::ChangedEventHandler(&OnChanged);

	cout << "Available: " << (decode)(LMFTracker->MeteoStation->ObjectTemperature->Available->Label) <<
		" Value: " << TFS[(int)LMFTracker->MeteoStation->ObjectTemperature->Available->Value] <<
		endl;

	LMFTracker->MeteoStation->Source->Changed += gcnew LMF::Tracker::Meteo::MeteoSource::ChangedEventHandler(&OnChanged);
	cout << "Source: Label: " << (decode)(LMFTracker->MeteoStation->Source->Label) <<
		" Value: " << EMeteoSourceStrings[(int)LMFTracker->MeteoStation->Source->Value] <<
		 endl;

	cout << endl;

}

void Do_TargetSearch(LMF::Tracker::Tracker^ LMFTracker)
{
	cout << endl;
	cout << "TargetSearch\n";

	LMFTracker->TargetSearch->Finished += gcnew LMF::Tracker::Targets::TargetSearch::FinishedEventHandler(&OnFinished);
	LMFTracker->TargetSearch->ApproximateDistance->Changed += gcnew LMF::Tracker::BasicTypes::DoubleValue::ReadOnlyDoubleValue::ChangedEventHandler(&OnChanged);
	LMFTracker->TargetSearch->Radius->Changed += gcnew LMF::Tracker::BasicTypes::DoubleValue::ReadOnlyDoubleValue::ChangedEventHandler(&OnChanged);
	LMFTracker->TargetSearch->Timeout->Changed += gcnew LMF::Tracker::BasicTypes::DoubleValue::ReadOnlyDoubleValue::ChangedEventHandler(&OnChanged);

	cout << "AproximateDistance: Label: " << (decode)(LMFTracker->TargetSearch->ApproximateDistance->Label) <<
		" MaxValue: " << LMFTracker->TargetSearch->ApproximateDistance->MaxValue <<
		" MaxValueInBaseUnits: " << LMFTracker->TargetSearch->ApproximateDistance->MaxValueInBaseUnits <<
		" MinValue: " << LMFTracker->TargetSearch->ApproximateDistance->MinValue <<
		" MinValueInBaseUnits: " << LMFTracker->TargetSearch->ApproximateDistance->MinValueInBaseUnits <<
		" UnitString: " << (decode)(LMFTracker->TargetSearch->ApproximateDistance->UnitString) <<
		" UnitType: " << EUnitTypeStrings[(int)LMFTracker->TargetSearch->ApproximateDistance->UnitType] <<
		" Value: " << LMFTracker->TargetSearch->ApproximateDistance->Value <<
		" ValueInBaseUnits: " << LMFTracker->TargetSearch->ApproximateDistance->ValueInBaseUnits <<
		endl;

	cout << "Radius: Label: " << (decode)(LMFTracker->TargetSearch->Radius->Label) <<
		" MaxValue: " << LMFTracker->TargetSearch->Radius->MaxValue <<
		" MaxValueInBaseUnits: " << LMFTracker->TargetSearch->Radius->MaxValueInBaseUnits <<
		" MinValue: " << LMFTracker->TargetSearch->Radius->MinValue <<
		" MinValueInBaseUnits: " << LMFTracker->TargetSearch->Radius->MinValueInBaseUnits <<
		" UnitString: " << (decode)(LMFTracker->TargetSearch->Radius->UnitString) <<
		" UnitType: " << EUnitTypeStrings[(int)LMFTracker->TargetSearch->Radius->UnitType] <<
		" Value: " << LMFTracker->TargetSearch->Radius->Value <<
		" ValueInBaseUnits: " << LMFTracker->TargetSearch->Radius->ValueInBaseUnits <<
		endl;

	cout << "Timeout: Label: " << (decode)(LMFTracker->TargetSearch->Timeout->Label) <<
		" MaxValue: " << LMFTracker->TargetSearch->Timeout->MaxValue <<
		" MaxValueInBaseUnits: " << LMFTracker->TargetSearch->Timeout->MaxValueInBaseUnits <<
		" MinValue: " << LMFTracker->TargetSearch->Timeout->MinValue <<
		" MinValueInBaseUnits: " << LMFTracker->TargetSearch->Timeout->MinValueInBaseUnits <<
		" UnitString: " << (decode)(LMFTracker->TargetSearch->Timeout->UnitString) <<
		" UnitType: " << EUnitTypeStrings[(int)LMFTracker->TargetSearch->Timeout->UnitType] <<
		" Value: " << LMFTracker->TargetSearch->Timeout->Value <<
		" ValueInBaseUnits: " << LMFTracker->TargetSearch->Timeout->ValueInBaseUnits <<
		endl;

	// Methods
	/*
		LMFTracker->TargetSearch->Start();
		LMFTracker->TargetSearch->StartAsync();
		LMFTracker->TargetSearch->Stop();
	*/

	cout << endl;
}


void Do_QuickRelease(LMF::Tracker::Tracker^ LMFTracker)
{
	cout << endl;
	cout << "QuickRelease\n";

	LMFTracker->QuickRelease->Changed += gcnew LMF::Tracker::BasicTypes::BoolValue::ReadOnlyBoolValue::ChangedEventHandler(&OnChanged);

	cout << "QuickRelease: Label: " << (decode)(LMFTracker->QuickRelease->Label) << " Value: " << TFS[LMFTracker->QuickRelease->Value] << "\n";

	cout << endl;
}

void Do_PowerSource(LMF::Tracker::Tracker^ LMFTracker)
{
	cout << endl;
	cout << "PowerSource\n";

	// does not seem to exist (at run time!) and a try block doesn't correct trap it Maybe only valid on Battery operated units

	/*
		try {
			LMFTracker->PowerSource->SensorPowerStatus->Level->Changed += gcnew LMF::Tracker::BasicTypes::DoubleValue::ReadOnlyDoubleValue::ChangedEventHandler(&OnChanged);
			cout << "Sensor Power Status: Label: " << (decode)(LMFTracker->PowerSource->SensorPowerStatus->Level->Label) << " Value: " << LMFTracker->PowerSource->SensorPowerStatus->Level->Value << endl;
		}
		catch (LMF::Tracker::ErrorHandling::LmfException^ e)
		{
			cout << "Sensor Power Status Error Code: " << e->Number << " " << (decode)(e->Description) << "\n";
		}
	*/

	// TODO
	// Most units values are enum values . . . .The string values need to be global assignments, if required
	// I brute forced it in other places, but most probably need to do it right if there is more of this


	LMFTracker->PowerSource->ControllerPowerStatus->Level->Changed += gcnew LMF::Tracker::BasicTypes::DoubleValue::ReadOnlyDoubleValue::ChangedEventHandler(&OnChanged);
	LMFTracker->PowerSource->ControllerPowerStatus->RunsOn->Changed += gcnew LMF::Tracker::BasicTypes::EnumTypes::ReadOnlyPowerSourceValue::ChangedEventHandler(&OnChanged);

	cout << "Sensor Power Status: Label: " << (decode)(LMFTracker->PowerSource->ControllerPowerStatus->Level->Label) <<
		" UnitString: " << (decode)(LMFTracker->PowerSource->ControllerPowerStatus->Level->UnitString) <<
		" UnitType: " << EUnitTypeStrings[(int)LMFTracker->PowerSource->ControllerPowerStatus->Level->UnitType] <<
		" UnitType: " << (int)LMFTracker->PowerSource->ControllerPowerStatus->Level->UnitType <<
		" Value: " << LMFTracker->PowerSource->ControllerPowerStatus->Level->Value <<
		" ValueInBaseUnits: " << LMFTracker->PowerSource->ControllerPowerStatus->Level->ValueInBaseUnits <<
		endl;
	cout << "RunsOn: Label: " << (decode)(LMFTracker->PowerSource->ControllerPowerStatus->RunsOn->Label) <<
		" Value: " << EPowerSourceStrings[(int)LMFTracker->PowerSource->ControllerPowerStatus->RunsOn->Value] <<
		" Value: " << (int)LMFTracker->PowerSource->ControllerPowerStatus->RunsOn->Value <<
		endl;

	cout << endl;

}

void Do_PowerLock(LMF::Tracker::Tracker^ LMFTracker)
{
	cout << endl;
	cout << "PowerLock\n";
	LMFTracker->PowerLock->UsePowerLock->Changed += gcnew LMF::Tracker::BasicTypes::BoolValue::ReadOnlyBoolValue::ChangedEventHandler(&OnChanged);
	cout << "Use Power Lock: Label: " << (decode)(LMFTracker->PowerLock->UsePowerLock->Label) << " Value: " << TFS[LMFTracker->PowerLock->UsePowerLock->Value] << "\n";
	LMF::Tracker::OVC::ATRCoordinateCollection^ gettargetdirections = LMFTracker->PowerLock->GetTargetDirections();

	cout << "Get Target Directions Count: " << gettargetdirections->Count << endl;
	for (int i = 0; i < gettargetdirections->Count; i++)
	{
		// And this part now starts to get wierd . . .  since the OVC class is the camera system . . .  and values only seem to exist if you actually collect an image
		// and it reports how to find the target in that picture. So why is this under powerlock? Manual doesn't explain what this is, or why it might exist at all.
		// Also, The method that *should* return the items so that you can actually read the AngleHz, AngleVt, PixelX, and PixelY values *claims* to be in the TLB, 
		// but isn't something that I can actually access.

		// punt this until later . . .
	}

	cout << endl;

}
void Do_Laser(LMF::Tracker::Tracker^ LMFTracker)
{
	cout << endl;	
	cout << "Laser\n";

	DateTime^ dt;

	LMFTracker->Laser->IsLaserWarmedUp->Changed += gcnew LMF::Tracker::BasicTypes::BoolValue::ReadOnlyBoolValue::ChangedEventHandler(&OnChanged);
	LMFTracker->Laser->IsOn->Changed += gcnew LMF::Tracker::BasicTypes::BoolValue::ReadOnlyBoolValue::ChangedEventHandler(&OnChanged);


	dt = LMFTracker->Laser->WakeUpTime;
	cout << "Wakeup Time: " << (decode)(dt->ToString("dddd, dd. MMMM yyyy HH:mm:ss.fff")) << "\n";
	cout << "Is Laser Warmed Up: Label: " << (decode)(LMFTracker->Laser->IsLaserWarmedUp->Label) << " Value: " << TFS[LMFTracker->Laser->IsLaserWarmedUp->Value] << "\n";
	cout << "Is on: " << (decode)(LMFTracker->Laser->IsOn->Label) << " Value : " << TFS[LMFTracker->Laser->IsOn->Value] << "\n";
	// ToDo GoToSleep
	cout << endl;
}


void Do_WrtlBoxes(LMF::Tracker::Tracker^ LMFTracker)
{
	cout << endl;
	// does not seem to be implemented on the 930's . . .	
	cout << "WrtlBoxes . . . Not Implemented in this SDK.\n";
	cout << endl;
}

void Do_Targets(LMF::Tracker::Tracker^ LMFTracker)
{
	cout << endl;
	cout << "Targets\n";

	cout << " Target Count: " << LMFTracker->Targets->Count << "\n";


	LMF::Tracker::Targets::TargetCollection^ foundTargets = LMFTracker->Targets;


	for (int i = 0; i < LMFTracker->Targets->Count; i++)
	{

		//Maybe display a list of all Targets and let the user choose.

		//		Target^ thisTarget = foundTargets[i];


		if (LMF::Tracker::Targets::Reflectors::Reflector^ thisReflector = dynamic_cast<LMF::Tracker::Targets::Reflectors::Reflector^>(foundTargets[i]))
		{

			cout << " Reflector Comment: " << (decode)(thisReflector->Comment);
			cout << " Target Name: " << (decode)(thisReflector->Name);
			cout << " Product Name: " << (decode)(thisReflector->ProductName);
			cout << " IsSelectable: " << TFS[thisReflector->IsSelectable] << "\n";
			cout << "     " << (decode)(thisReflector->ADMOffset->Label);
			cout << " UnitString: " << (decode)(thisReflector->ADMOffset->UnitString);
			cout << " UnitType: " << (int)(thisReflector->ADMOffset->UnitType);
			cout << " Value: " << thisReflector->ADMOffset->Value << "\n";
			cout << "     " << (decode)(thisReflector->SurfaceOffset->Label);
			cout << " UnitString: " << (decode)(thisReflector->SurfaceOffset->UnitString);
			cout << " UnitType: " << (int)(thisReflector->SurfaceOffset->UnitType);
			cout << " Value: " << thisReflector->SurfaceOffset->Value << "\n";

		}

		else if (LMF::Tracker::Targets::Probes::PassiveProbes::BProbes::BProbe^ thisProbe = dynamic_cast<LMF::Tracker::Targets::Probes::PassiveProbes::BProbes::BProbe^>(foundTargets[i]))
		{

			cout << " Probe Comment: " << (decode)(thisProbe->Comment);
			cout << " Target Name: " << (decode)(thisProbe->Name);
			cout << " Product Name: " << (decode)(thisProbe->ProductName);
			cout << " Serial Number: " << (decode)(thisProbe->SerialNumber);
			cout << " IsSelectable: " << TFS[thisProbe->IsSelectable] << "\n";
			// I should be able to 'see' FaceCompensation and TipCompensation params if the type and/or cast is correct.
			cout << "     FaceCompensation: " << (decode)(thisProbe->FaceCompensation->Comment);
			cout << " Label: " << (decode)(thisProbe->FaceCompensation->IsCompensated->Label);
			cout << "IsCompensated: " << TFS[(int)thisProbe->FaceCompensation->IsCompensated->Value];
			cout << " Name: " << (decode)(thisProbe->FaceCompensation->Name);
			cout << " Product Name: " << (decode)(thisProbe->FaceCompensation->ProductName) << "\n";

			cout << "     TipCompensation BallRadius: " << thisProbe->TipCompensation->BallRadius;
			cout << " Comment: " << (decode)(thisProbe->TipCompensation->Comment);
			cout << "Label: " << (decode)(thisProbe->TipCompensation->IsCompensated->Label);
			cout << "IsComensated: " << TFS[(int)thisProbe->TipCompensation->IsCompensated->Value];
			cout << " Length: " << thisProbe->TipCompensation->Length;
			cout << " Name: " << (decode)(thisProbe->TipCompensation->Name);
			cout << " ProductName: " << (decode)(thisProbe->TipCompensation->ProductName);
			cout << " x: " << thisProbe->TipCompensation->X;
			cout << " y: " << thisProbe->TipCompensation->Y;
			cout << " z: " << thisProbe->TipCompensation->Z << "\n";


		}
		else if (Target^ thisTarget = foundTargets[i])
		{
			cout << " Target Comment: " << (decode)(thisTarget->Comment);
			cout << " Target Name: " << (decode)(thisTarget->Name);
			cout << " Product Name: " << (decode)(thisTarget->ProductName);
			cout << " IsSelectable: " << TFS[thisTarget->IsSelectable] << "\n";


		}


	}
	cout << endl;
}

void Do_Settings(LMF::Tracker::Tracker^ LMFTracker)
{
	cout << endl;

	cout << "Settings\n";

	LMF::Units::ECoordinateType coordtype = LMFTracker->Settings->CoordinateType;
	const char* coordtypeNames[] = { "Spherical", "Cartesian", "Cylindrical" };
	cout << " CoordinateType : " << coordtypeNames[(int)coordtype] << "\n";

	LMF::Units::ERotationType rottype = LMFTracker->Settings->RotationType;
	const char* rottypeNames[] = { "RotationAngles", "RollPitchYaw", "Quarternion" };
	cout << " RotationType : " << rottypeNames[(int)coordtype] << "\n";

	LMF::Units::EAngleUnit angunit = LMFTracker->Settings->Units->AngleUnit;
	const char* angunitNames[] = { "Radian", "Millirad", "Degree", "Gon", "CC" };
	cout << " AngleUnits : " << angunitNames[(int)angunit] << "\n";

	LMF::Units::EHumidityUnit humunit = LMFTracker->Settings->Units->HumidityUnit;
	const char* humunitNames[] = { "RelativeHumidity" };
	cout << " HumidityUnits : " << humunitNames[(int)humunit] << "\n";

	LMF::Units::ELengthUnit lenunit = LMFTracker->Settings->Units->LengthUnit;
	const char* lenunitNames[] = { "Meter", "Millimeter", "Micrometer","Foot","Yard", "Inch" };
	cout << " LengthUnits : " << lenunitNames[(int)humunit] << "\n";

	LMF::Units::EPercentUnit perunit = LMFTracker->Settings->Units->PercentUnit;
	const char* perunitNames[] = { "Percent", "None" };
	cout << " PercentUnits : " << perunitNames[(int)perunit] << "\n";

	LMF::Units::EPressureUnit presunit = LMFTracker->Settings->Units->PressureUnit;
	const char* presunitNames[] = { "mBar", "HPascal","KPascal","MmHg", "Psi", "InH2O","InHg" };
	cout << " PressureUnits : " << presunitNames[(int)presunit] << "\n";

	LMF::Units::ETemperatureUnit tempunit = LMFTracker->Settings->Units->TemperatureUnit;
	const char* tempunitNames[] = { "Celsius", "Fahrenheit" };
	cout << " TemperatureUnits : " << tempunitNames[(int)tempunit] << "\n";

	LMF::Units::ETimeUnit timeunit = LMFTracker->Settings->Units->TimeUnit;
	const char* timeunitNames[] = { "Millisecond", "Second", "Minute", "Hour" };
	cout << " TimeUnits : " << timeunitNames[(int)timeunit] << "\n\n";


	cout << " Get Orientation \n";
	LMF::Tracker::Alignment^ orient = LMFTracker->Settings->GetOrientation();

	cout << "  CoordinateType : " << coordtypeNames[(int)orient->CoordinateType] << "\n";
	cout << "  RotationType : " << rottypeNames[(int)orient->RotationType] << "\n";
	cout << "  Rotation0 : Label: " << (decode)(orient->Rotation0->Label)
		<< "  UnitString: " << (decode)(orient->Rotation0->UnitString) // Note: the marshalling conversion code throws an exception if the starting string is NULL, which it is here
		<< "  Value: " << orient->Rotation0->Value << "\n";
	cout << "  Rotation1 : Label: " << (decode)(orient->Rotation1->Label)
		<< "  UnitString: " << (decode)(orient->Rotation1->UnitString)
		<< "  Value: " << orient->Rotation1->Value << "\n";
	cout << "  Rotation2 : Label: " << (decode)(orient->Rotation2->Label)
		<< "  UnitString: " << (decode)(orient->Rotation2->UnitString)
		<< "  Value: " << orient->Rotation2->Value << "\n";
	cout << "  Rotation3 : Label: " << (decode)(orient->Rotation3->Label)
		<< "  UnitString: " << (decode)(orient->Rotation3->UnitString)
		<< "  Value: " << orient->Rotation3->Value << "\n";
	cout << "  Translation1 : Label: " << (decode)(orient->Translation1->Label)
		<< "  UnitString: " << (decode)(orient->Translation1->UnitString)
		<< "  Value: " << orient->Translation1->Value << "\n";
	cout << "  Translation2 : Label: " << (decode)(orient->Translation2->Label)
		<< "  UnitString: " << (decode)(orient->Translation2->UnitString)
		<< "  Value: " << orient->Translation2->Value << "\n";
	cout << "  Translation3 : Label: " << (decode)(orient->Translation3->Label)
		<< "  UnitString: " << (decode)(orient->Translation3->UnitString)
		<< "  Value: " << orient->Translation3->Value << "\n\n";

	cout << " Get Transformation \n";
	LMF::Tracker::AlignmentWithScale^ transf = LMFTracker->Settings->GetTransformation();

	cout << "  CoordinateType : " << coordtypeNames[(int)transf->CoordinateType] << "\n";
	cout << "  RotationType : " << rottypeNames[(int)transf->RotationType] << "\n";
	cout << "  Rotation0 : Label: " << (decode)(transf->Rotation0->Label)
		<< "  UnitString: " << (decode)(transf->Rotation0->UnitString)
		<< "  Value: " << transf->Rotation0->Value << "\n";
	cout << "  Rotation1 : Label: " << (decode)(transf->Rotation1->Label)
		<< "  UnitString: " << (decode)(transf->Rotation1->UnitString)
		<< "  Value: " << transf->Rotation1->Value << "\n";
	cout << "  Rotation2 : Label: " << (decode)(transf->Rotation2->Label)
		<< "  UnitString: " << (decode)(transf->Rotation2->UnitString)
		<< "  Value: " << transf->Rotation2->Value << "\n";
	cout << "  Rotation3 : Label: " << (decode)(transf->Rotation3->Label)
		<< "  UnitString: " << (decode)(transf->Rotation3->UnitString)
		<< "  Value: " << transf->Rotation3->Value << "\n";
	cout << "  Scale : Label: " << (decode)(transf->Scale->Label)
		<< "  UnitString: " << (decode)(transf->Scale->UnitString)
		<< "  Value: " << transf->Scale->Value << "\n";
	cout << "  Translation1 : Label: " << (decode)(transf->Translation1->Label)
		<< "  UnitString: " << (decode)(transf->Translation1->UnitString)
		<< "  Value: " << transf->Translation1->Value << "\n";
	cout << "  Translation2 : Label: " << (decode)(transf->Translation2->Label)
		<< "  UnitString: " << (decode)(transf->Translation2->UnitString)
		<< "  Value: " << transf->Translation2->Value << "\n";
	cout << "  Translation3 : Label: " << (decode)(transf->Translation3->Label)
		<< "  UnitString: " << (decode)(transf->Translation3->UnitString)
		<< "  Value: " << transf->Translation3->Value << "\n\n";

	cout << endl;

}

void Do_Compensations(LMF::Tracker::Tracker^ LMFTracker)
{

	cout << endl;
	cout << "Compensations \n";

	//A) List Compensations

	cout << "Compensation Count: " << LMFTracker->Compensations->Count << "\n";

	// there should be 	Changed and SelectedChanged callbacks that Tracker Scope says exists which the compiler says doesn't exist
	// BUT . . . what/who would be changing this except us, so we should already know about that, yes?
	// even the class LMF::Tracker::Compensations::CompensationCollection, which the tlb shows exports theses callbacks, don't actually
	// shows those if just created as a standalone thing.

	//	LMFTracker->Compensations->Changed
	//  LMFTracker->Compensations->SelectedChanged


	DateTime^ dt;

	for (int i = 0; i < LMFTracker->Compensations->Count; i++)
	{

		LMF::Tracker::Compensations::Compensation^ compensations = LMFTracker->Compensations[i];
		cout << "Comment: " << (decode)(compensations->Comment) << " ";
		cout << "GUID: " << (decode)(compensations->GUID) << " ";
		cout << "Name: " << (decode)(compensations->Name) << " ";
		dt = compensations->TimeStamp;
		cout << "TimeStamp: " << (decode)(dt->ToString("dddd, dd. MMMM yyyy HH:mm:ss.fff")) << "\n";

	}
	cout << "Selected Compensation . . .  \n";

	cout << "Comment: " << (decode)(LMFTracker->Compensations->Selected->Comment) << " ";
	cout << "GUID: " << (decode)(LMFTracker->Compensations->Selected->GUID) << " ";
	cout << "Name: " << (decode)(LMFTracker->Compensations->Selected->Name) << " ";
	dt = LMFTracker->Compensations->Selected->TimeStamp;
	cout << "TimeStamp: " << (decode)(dt->ToString("dddd, dd. MMMM yyyy HH:mm:ss.fff")) << "\n";

	cout << endl;

}

void Do_Face(LMF::Tracker::Tracker^ LMFTracker)
{
	cout << endl;
	cout << "Face \n";

	//B) List Face

	LMFTracker->Face->Changed += gcnew LMF::Tracker::Face::ChangedHandler(&OnChanged);
	LMFTracker->Face->ChangeFinished += gcnew LMF::Tracker::Face::ChangeFinishedHandler(&OnChangeFinished);

	cout << "Face: isface1: " << TFS[LMFTracker->Face->IsFace1] << "\n";
	cout << "Face: Value: " << EFaceStrings[(int)LMFTracker->Face->Value] << "\n";

	// Just doing the follow as a value check
		/*
		cout << "Face: Flip \n";
		LMFTracker->Face->Change();
		cout << "Face: isface1: " << LMFTracker->Face->IsFace1 << "\n";
		cout << "Face: Value: " << (int)LMFTracker->Face->Value << "\n";
		cout << "Face: UnFlip \n\n";
		LMFTracker->Face->Change();
	*/
	cout << endl;
}



void OnDisconnected(LMF::Tracker::Tracker^ sender, LMF::Tracker::ErrorHandling::LmfException^ ex)
{
	//   throw gcnew System::NotImplementedException();
	if (ex)
		cout << "callback exception code: " << ex->Number << " " << (decode)(ex->Description) << "\n";
	cout << "callback Disconnected finished . . . \n";
}

void OnErrorArrived(LMF::Tracker::Tracker^ sender, LMF::Tracker::ErrorHandling::LmfError^ error)
{
	//   throw gcnew System::NotImplementedException();
	cout << "callback exception code: " << error->Number << " " << (decode)(error->Description) << "\n";;
	cout << "callback Got some sort of error message . . . \n";
}

void OnGetDirectionFinished(LMF::Tracker::Tracker^ sender, LMF::Tracker::Direction^ bm, LMF::Tracker::ErrorHandling::LmfException^ ex)
{
	//   throw gcnew System::NotImplementedException();
	cout << "callback exception " << (decode)(ex->Description) << "\n";;
	cout << "callback Got some sort of Get Direction finished message . . . \n";

	cout << "Direction H Angle: " << bm->HorizontalAngle->Value << " V Angle: " << bm->VerticalAngle->Value << "\n";


}

void OnGetPrismPositionFinished(LMF::Tracker::Tracker^ sender, LMF::Tracker::MeasurementResults::Measurement^ paramMeasurement, LMF::Tracker::ErrorHandling::LmfException^ ex)
{
	//   throw gcnew System::NotImplementedException();
	cout << "callback exception " << (decode)(ex->Description) << "\n";;
	cout << "callback OnGetPosition Finished . . . \n";

}

void OnGoHomePositionFinished(LMF::Tracker::Tracker^ sender, LMF::Tracker::ErrorHandling::LmfException^ ex)
{
	//   throw gcnew System::NotImplementedException();
	cout << "callback exception " << (decode)(ex->Description) << "\n";;
	cout << "callback Asyn GoHomePosition finished . . . \n";
}

void OnInformationArrived(LMF::Tracker::Tracker^ sender, LMF::Tracker::ErrorHandling::LmfInformation^ paramInfo)
{
	//   throw gcnew System::NotImplementedException();
	cout << (decode)(paramInfo->Description) << "\n";;
	cout << "callback Got some sort of Information message . . . \n";
}

void OnInitializeFinished(LMF::Tracker::Tracker^ sender, LMF::Tracker::ErrorHandling::LmfException^ ex)
{
	//   throw gcnew System::NotImplementedException();
	if (ex)
		cout << "callback exception code: " << ex->Number << " " << (decode)(ex->Description) << "\n";
	cout << "callback Initialization finished . . . \n";

}

void OnPositionToFinished(LMF::Tracker::Tracker^ sender, LMF::Tracker::Targets::Target^ foundTarget, LMF::Tracker::ErrorHandling::LmfException^ ex)
{
	//  throw gcnew System::NotImplementedException();
	cout << (decode)(ex->Description) << "\n";;

	cout << "callback PositionTo finished . . . \n";


}

void OnPositionToTargetFinished(LMF::Tracker::Tracker^ sender, LMF::Tracker::Targets::Target^ foundTarget, LMF::Tracker::ErrorHandling::LmfException^ ex)
{
	//   throw gcnew System::NotImplementedException();
	cout << "callback PositionToTarget finished . . . \n";

}

void OnWarningArrived(LMF::Tracker::Tracker^ sender, LMF::Tracker::ErrorHandling::LmfWarning^ warning)
{
	//   throw gcnew System::NotImplementedException();
	cout << "callback Warning message: " << warning->Number << " . . . " << decode(warning->Description) << "\n";


}


void OnMeasurementArrived(LMF::Tracker::Measurements::MeasurementSettings^ sender, LMF::Tracker::MeasurementResults::MeasurementCollection^ paramMeasurements, LMF::Tracker::ErrorHandling::LmfException^ paramException)
{
	LMF::Tracker::MeasurementResults::Measurement^ LastMeasurement = nullptr;

	// throw gcnew System::NotImplementedException();
	cout << "callback Got a Measurement Value . . . \n";

	cout << "counts :" << paramMeasurements->Count << "\n";

	if (paramMeasurements)
	{
		//        if (paramMeasurements->Count > 0)
		for (int i = 0; i < paramMeasurements->Count; ++i) {
			{
				LastMeasurement = paramMeasurements[i];

				cout << "Measurment Humidity: " << LastMeasurement->Humidity->Value << " Pressure: " << LastMeasurement->Pressure->Value << " Temperature: " << LastMeasurement->Temperature->Value << "\n";


				if (StationaryMeasurement3D^ stationaryMeas3D = dynamic_cast<StationaryMeasurement3D^>(LastMeasurement))
				{
					cout << "I am a stationary3d measurement \n";

					cout << " X = " << stationaryMeas3D->Position->Coordinate1->Value << " " << (decode)(stationaryMeas3D->Position->Coordinate1->UnitString);
					cout << " Y = " << stationaryMeas3D->Position->Coordinate2->Value << " " << (decode)(stationaryMeas3D->Position->Coordinate2->UnitString);
					cout << " Z = " << stationaryMeas3D->Position->Coordinate3->Value << " " << (decode)(stationaryMeas3D->Position->Coordinate3->UnitString) << "\n";



				}
				else if (StationaryMeasurement6D^ stationaryMeas6D = dynamic_cast<StationaryMeasurement6D^>(LastMeasurement))
				{
					cout << "I am a stationary6d measurement \n";

					cout << " X = " << stationaryMeas6D->Position->Coordinate1->Value << " " << (decode)(stationaryMeas6D->Position->Coordinate1->UnitString);
					cout << " Y = " << stationaryMeas6D->Position->Coordinate2->Value << " " << (decode)(stationaryMeas6D->Position->Coordinate2->UnitString);
					cout << " Z = " << stationaryMeas6D->Position->Coordinate3->Value << " " << (decode)(stationaryMeas6D->Position->Coordinate3->UnitString) << "\n";


				}
				else if (SingleShotMeasurement3D^ singleshot3dD = dynamic_cast<SingleShotMeasurement3D^>(LastMeasurement))
				{
					cout << "I am a singleshot 3d measurement \n";

					cout << " X = " << singleshot3dD->Position->Coordinate1->Value << " " << (decode)(singleshot3dD->Position->Coordinate1->UnitString);
					cout << " Y = " << singleshot3dD->Position->Coordinate2->Value << " " << (decode)(singleshot3dD->Position->Coordinate2->UnitString);
					cout << " Z = " << singleshot3dD->Position->Coordinate3->Value << " " << (decode)(singleshot3dD->Position->Coordinate3->UnitString) << "\n";


				}
				else if (SingleShotMeasurement6D^ singleshot6dD = dynamic_cast<SingleShotMeasurement6D^>(LastMeasurement))
				{
					cout << "I am a singleshot 6d measurement \n";

					cout << " X = " << singleshot6dD->Position->Coordinate1->Value << " " << (decode)(singleshot6dD->Position->Coordinate1->UnitString);
					cout << " Y = " << singleshot6dD->Position->Coordinate2->Value << " " << (decode)(singleshot6dD->Position->Coordinate2->UnitString);
					cout << " Z = " << singleshot6dD->Position->Coordinate3->Value << " " << (decode)(singleshot6dD->Position->Coordinate3->UnitString) << "\n";

				}
			}
		}
	}
}

void OnChanged(LMF::Tracker::MeasurementStatus::MeasurementStatusValue^ sender, LMF::Tracker::Enums::EMeasurementStatus paramNewValue)
{
	//    throw gcnew System::NotImplementedException();
	cout << "Measurement Status Value changed: " << "\n";

	if (paramNewValue == EMeasurementStatus::ReadyToMeasure) { cout << "Ready To Measure . . . \n"; }
	if (paramNewValue == EMeasurementStatus::MeasurementInProgress) { cout << "Measurement in Progress . . . \n"; }
	if (paramNewValue == EMeasurementStatus::NotReady) { cout << "Not Ready . . . \n"; }
	if (paramNewValue == EMeasurementStatus::Invalid) { cout << "Measurement Status Invalid . . . \n"; }
}

//decoding these gets . . .  messy, since dozens of things all normally feed into these, so would have to parse through alot of senders to see what actual field and/or PV 
//we are talking about and might need to be updated. If you want to be more verbose, could make these unique

void OnChanged(LMF::Tracker::BasicTypes::BoolValue::ReadOnlyBoolValue^ sender, bool paramNewValue)
{
	cout << "Bool Value changed: " << "\n";
	//	throw gcnew System::NotImplementedException();
}

void OnChanged(LMF::Tracker::BasicTypes::DoubleValue::ReadOnlyDoubleValue^ sender, double paramNewValue)
{
	cout << "Double Value changed: " << "\n";
	//	throw gcnew System::NotImplementedException();
}

void OnChanged(LMF::Tracker::Face^ sender, LMF::Tracker::Enums::EFace paramNewValue)
{
	cout << "Callback Face value changed: " << "\n";
	//	throw gcnew System::NotImplementedException();
}

void OnChangeFinished(LMF::Tracker::Face^ sender, LMF::Tracker::Enums::EFace paramNewValue, LMF::Tracker::ErrorHandling::LmfException^ ex)
{
	cout << "Callback On Face Change Finished . . . \n";
	//throw gcnew System::NotImplementedException();
}

void OnChanged(LMF::Tracker::BasicTypes::EnumTypes::ReadOnlyPowerSourceValue^ sender, LMF::Tracker::Enums::EPowerSource paramNewValue)
{
	throw gcnew System::NotImplementedException();
}

void OnFinished(LMF::Tracker::Targets::TargetSearch^ sender, LMF::Tracker::Targets::Target^ foundTarget, LMF::Tracker::ErrorHandling::LmfException^ ex)
{
	throw gcnew System::NotImplementedException();
}

void OnEnvironmentalValuesChanged(LMF::Tracker::Meteo::MeteoStation^ sender, double paramTemperature, double paramHumidity, double paramPressure)
{
	throw gcnew System::NotImplementedException();
}

void OnChanged(LMF::Tracker::Meteo::MeteoSource^ sender, LMF::Tracker::Enums::EMeteoSource paramNewValue)
{
	throw gcnew System::NotImplementedException();
}

void OnGetInclinationToGravityFinished(LMF::Tracker::Inclination::InclinationSensor^ sender, LMF::Tracker::Inclination::InclinationToGravity^ paramInclinationToGravity, LMF::Tracker::ErrorHandling::LmfException^ ex)
{
	throw gcnew System::NotImplementedException();
}

void OnBubbleReadoutArrived(LMF::Tracker::Inclination::InclinationBubbleReadout^ sender, LMF::Tracker::Inclination::BubbleReadoutArrivedEventArgs^ paramBubbleReadout)
{
	throw gcnew System::NotImplementedException();
}

void OnInclinationChanged(LMF::Tracker::Inclination::InclinationMonitoring^ sender)
{
	throw gcnew System::NotImplementedException();
}
