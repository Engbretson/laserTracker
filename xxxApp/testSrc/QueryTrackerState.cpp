//
// LaserTracker.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <windows.h>
#pragma comment(lib, "User32.lib")

#include <iostream>

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
		return "N/A";
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
		cout << (decode)(e->Description) << "\n";;
		cout << "Hit an exception trying to decode Check For Errors  \n";
	}

	return ErrorNumber;

}

int  CheckForMeasurementErrors(LMF::Tracker::Tracker^ LMFTracker)
{
	LMF::Tracker::Enums::EMeasurementStatus statusValue = LMFTracker->Measurement->Status->Value;

	if (statusValue == EMeasurementStatus::ReadyToMeasure) { cout << " Ready To Measure . . . \n"; }
	if (statusValue == EMeasurementStatus::MeasurementInProgress) { cout << " Measurement in Progress . . . \n"; }
	if (statusValue == EMeasurementStatus::NotReady) { cout << " Not Ready to Measure . . . \n"; }
	if (statusValue == EMeasurementStatus::Invalid) { cout << " Measurement Status Invalid . . . \n"; }

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
1) Create the laser Tracker Object
2) Do a check to see if Any lasrer trackers  are found via search
3) Connect to a Simulated tracker or a real tracker
4) Initialze the Hardware
5) Return simple top level values 
6) do 

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

	for (int i = 0; i < foundTrackers->Count; i++)
	{
		TrackerInfo^ tracker = foundTrackers[i];

		cout << " Tracker Name: " << (decode)(tracker->Name);
		cout << " Serial Number: " << (decode)(tracker->SerialNumber);
		cout << " IP Address: " << (decode)(tracker->IPAddress);
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
	cout << "Connecting to 192.168.0.1 \n";

//	LMFTracker = con->Connect("At403Simulator");
//	LMFTracker = con->Connect("At930Simulator");
    try {
	LMFTracker = con->Connect("192.168.0.1");
	}
			catch (LMF::Tracker::ErrorHandling::LmfException^ e)
	{
		cout << (decode)(e->Description) << "\n";;
		cout << "Hit an exception trying to perform a Connect call \n";
		exit(-1);
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
	CheckForMeasurementErrors(LMFTracker);

	cout << "Initialize . . . \n";

try {
	LMFTracker->Initialize();
}	
catch (LMF::Tracker::ErrorHandling::LmfException^ e)
	{
		cout << (decode)(e->Description) << "\n";;
		cout << "Hit an exception trying to perform an Initialize  call \n";
	}


	
	cout << "Initialize Async . . . \n";
	try {
	LMFTracker->InitializeAsync();
	}
		catch (LMF::Tracker::ErrorHandling::LmfException^ e)
	{
		cout << (decode)(e->Description) << "\n";;
		cout << "Hit an exception trying to perform an Initilize Async call \n";
	}

	cout << "After Initialization . . . Check for Errors? \n\n";

	CheckForErrors(LMFTracker);
	CheckForMeasurementErrors(LMFTracker);

// 5

	String^ Comment = LMFTracker->Comment;
	cout << "Comment: " << (decode)(Comment) << "\n";

	String^ Firmware = LMFTracker->ExpectedFirmware;
	cout << "ExpectedFirmware: " << (decode)(Firmware) << "\n";

	String^ InstalledFirmware = LMFTracker->InstalledFirmware;
	cout << "InstalledFirmware: " << (decode)(InstalledFirmware) << "\n";

	String^ IP = LMFTracker->IPAddress;
	cout << "IPAddress: " << (decode)(IP) << "\n";

	Boolean CompatFirmware = LMFTracker->IsCompatibleWithInstalledFirmware;
	cout << "IsCompatibleWithInstalledFirmware: " << CompatFirmware << "\n";

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
// Get Prism Position

	cout << "Attempting a GetPrismPosition call \n";

	try
	{
				Measurement^ measure = LMFTracker->GetPrismPosition(); //says not supported by this tracker
				cout << "Prism Position Humidity: " << measure->Humidity->Value << " Pressure: " << measure->Pressure->Value << " Temperature: " << measure->Temperature->Value << "\n";
	}
	catch (LMF::Tracker::ErrorHandling::LmfException^ e)
	{
		cout << (decode)(e->Description) << "\n";;
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
		cout << (decode)(e->Description) << "\n";;
		cout << "Hit an exception trying to perform a Get Prism Position Async call \n";
	}

	Sleep(1000);




//A) List Compensations
//B) List Face
//C) List InclinationSensor
//D) List Laser
//E) ?? Measurement Status
//F) MeteoStaion
//G) ?? OverViewCamera could be useful finding targets
//H) List Powerlock
//I) List PowerSource
//J) List QuickRelease
//K) Settings

	cout << "Info from Settings . . .  \n";

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


//K) List Tagets
 
 	   	cout << "Targets . . . \n";

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
			cout << " IsSelectable: " << thisReflector->IsSelectable << "\n";
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
			cout << " IsSelectable: " << thisProbe->IsSelectable << "\n";
			// I should be able to 'see' FaceCompensation and TipCompensation params if the type and/or cast is correct.
			cout << "     FaceCompensation: " << (decode)(thisProbe->FaceCompensation->Comment);
			cout << " Label: " << (decode)(thisProbe->FaceCompensation->IsCompensated->Label);
			cout << (int)thisProbe->FaceCompensation->IsCompensated->Value;
			cout << " Name: " << (decode)(thisProbe->FaceCompensation->Name);
			cout << " Product Name: " << (decode)(thisProbe->FaceCompensation->ProductName) << "\n";

			cout << "     TipCompensation BallRadius: " << thisProbe->TipCompensation->BallRadius;
			cout << " Comment: " << (decode)(thisProbe->TipCompensation->Comment);
			cout << "Label: " << (decode)(thisProbe->TipCompensation->IsCompensated->Label);
			cout << (int)thisProbe->TipCompensation->IsCompensated->Value;
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
			cout << " IsSelectable: " << thisTarget->IsSelectable << "\n";


		}


	}



//L) ?? list Triggers
//M) ?? List WrtlBoxes



	cout << "Shutdown . . . \n";
	LMFTracker->ShutDown();

	cout << "Disconnect . . . \n";
	LMFTracker->Disconnect();
	return 0;
}



void OnDisconnected(LMF::Tracker::Tracker^ sender, LMF::Tracker::ErrorHandling::LmfException^ ex)
{
	//   throw gcnew System::NotImplementedException();
	cout << "callback exception " << (decode)(ex->Description) << "\n";;
	cout << "callback Disconnected finished . . . \n";
}

void OnErrorArrived(LMF::Tracker::Tracker^ sender, LMF::Tracker::ErrorHandling::LmfError^ error)
{
	//   throw gcnew System::NotImplementedException();
	cout << "callback exception " << (decode)(error->Description) << "\n";;
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
	cout << "callback exception " << (decode)(ex->Description) << "\n";;
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
	cout << "callback Got some sort of Warning message . . . " << decode(warning->Description) << "\n";
	

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

	if (paramNewValue == EMeasurementStatus::ReadyToMeasure) { cout << " Ready To Measure . . . \n"; }
	if (paramNewValue == EMeasurementStatus::MeasurementInProgress) { cout << " Measurement in Progress . . . \n"; }
	if (paramNewValue == EMeasurementStatus::NotReady) { cout << " Not Ready . . . \n"; }
	if (paramNewValue == EMeasurementStatus::Invalid) { cout << " Measurement Status Invalid . . . \n"; }
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
