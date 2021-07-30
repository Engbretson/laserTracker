//
// LaserTracker.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>

//laser tracker realted headers


#include <msclr\marshal.h>
#include <msclr\marshal_cppstd.h>

// a slight space saver, since I have to do this *everywhere*
#define decode msclr::interop::marshal_as<std::string>

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

// forward definitions

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

int CheckForErrors(LMF::Tracker::Tracker^ LMFTracker);
int CheckForMeasurementErrors(LMF::Tracker::Tracker^ LMFTracker);

int CheckForErrors(LMF::Tracker::Tracker^ LMFTracker)
{
	// If and when something throws and error, this is how to decode it 
	Int32 ErrorNumber = 0;
	LmfError^ err = LMFTracker->GetErrorDescription(ErrorNumber);
	if (err->Number > 0) {
		cout << "Is anything throwing an error code? \n";
		cout << (decode)(err->Description) << " " <<
			err->Number << " " <<
			(decode)(err->Solution) << " " <<
			(decode)(err->Title) << "\n";
	}
	return err->Number;

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
	LMF::Tracker::Tracker^ LMFTracker;

	Connection^ con = gcnew Connection();



	//	LMFTracker = con->Connect("AT401Simulator");
	//	LMFTracker = con->Connect("AT402Simulator");
	//	LMFTracker = con->Connect("AT403Simulator");
	//	LMFTracker = con->Connect("AT600Simulator"); //wrong name maybe?
	//	LMFTracker = con->Connect("AT901LRSimulator");
	//	LMFTracker = con->Connect("AT960MRSimulator"); 
	//	LMFTracker = con->Connect("AT960LRSimulator"); 
	//	LMFTracker = con->Connect("AT930Simulator");



	cout << "Searching for trackers . . . \n";
	// The TrackerFinder holds a list of found Trackers

	TrackerFinder^ trackerFinder = gcnew TrackerFinder();
	TrackerInfoCollection^ foundTrackers = trackerFinder->Trackers;

	for (int i = 0; i < foundTrackers->Count; i++)
	{

		//Maybe display a list of all Trackers and let the user choose.

		TrackerInfo^ tracker = foundTrackers[i];


		cout << " Tracker Name: " << (decode)(tracker->Name);
		cout << " Serial Number: " << (decode)(tracker->SerialNumber);
		cout << " IP Address: " << (decode)(tracker->IPAddress);
		cout << " Type: " << (decode)(tracker->Type) << "\n";
	}


	// Possible simulators, according to some of the docs

		//	LMFTracker = con->Connect("AT401Simulator");
		//	LMFTracker = con->Connect("AT402Simulator");
		//	LMFTracker = con->Connect("AT403Simulator");
		//	LMFTracker = con->Connect("AT600Simulator"); //wrong name maybe?
		//	LMFTracker = con->Connect("AT901LRSimulator");
		//	LMFTracker = con->Connect("AT960MRSimulator"); 
		//	LMFTracker = con->Connect("AT960LRSimulator"); 
		//	LMFTracker = con->Connect("AT930Simulator");

	cout << "Connecting to At403Simulator \n";

	LMFTracker = con->Connect("At403Simulator");

	// Callbacks and I do not see any advantage to using any of the async ones at the instant, many are not supportd on this simulated hardware

	LMFTracker->Disconnected += gcnew LMF::Tracker::Tracker::DisconnectedHandler(&OnDisconnected);
	LMFTracker->ErrorArrived += gcnew LMF::Tracker::Tracker::ErrorArrivedHandler(&OnErrorArrived);
	LMFTracker->GetDirectionFinished += gcnew LMF::Tracker::Tracker::GetDirectionFinishedHandler(&OnGetDirectionFinished);
	LMFTracker->GetPrismPositionFinished += gcnew LMF::Tracker::Tracker::GetPrismPositionFinishedHandler(&OnGetPrismPositionFinished);
	LMFTracker->GoHomePositionFinished += gcnew LMF::Tracker::Tracker::GoHomePositionFinishedHandler(&OnGoHomePositionFinished);
	LMFTracker->InformationArrived += gcnew LMF::Tracker::Tracker::InformationArrivedHandler(&OnInformationArrived);
	LMFTracker->InitializeFinished += gcnew LMF::Tracker::Tracker::InitializeFinishedHandler(&OnInitializeFinished);
	LMFTracker->PositionToFinished += gcnew LMF::Tracker::Tracker::PositionToFinishedHandler(&OnPositionToFinished);
	LMFTracker->PositionToTargetFinished += gcnew LMF::Tracker::Tracker::PositionToFinishedHandler(&OnPositionToTargetFinished);
	LMFTracker->WarningArrived += gcnew LMF::Tracker::Tracker::WarningArrivedHandler(&OnWarningArrived);

	LMFTracker->Measurement->MeasurementArrived += gcnew LMF::Tracker::Measurements::MeasurementSettings::MeasurementArrivedHandler(&OnMeasurementArrived);

	cout << "Is the hardware ready ?\n";

	CheckForErrors(LMFTracker);
	CheckForMeasurementErrors(LMFTracker);



	cout << "Initialize . . . using 'realistic' timings in the simulator  . . . . which is *slow* . . .  \n";

	LMFTracker->Initialize();
	LMFTracker->InitializeAsync();

	cout << "After Initialization . . . \n";

	CheckForErrors(LMFTracker);
	CheckForMeasurementErrors(LMFTracker);

	// 
	// in namespace LMF::tracker
	// the simple values 

	String^ Comment = LMFTracker->Comment;
	cout << "Comment: " << (decode)(Comment) << "\n";

	String^ Firmware = LMFTracker->ExpectedFirmware;
	cout << "Firmware: " << (decode)(Firmware) << "\n";

	String^ InstalledFirmware = LMFTracker->InstalledFirmware;
	cout << "InstalledFirmware: " << (decode)(InstalledFirmware) << "\n";

	String^ IP = LMFTracker->IPAddress;
	cout << "IP: " << (decode)(IP) << "\n";

	Boolean CompatFirmware = LMFTracker->IsCompatibleWithInstalledFirmware;
	cout << "Is Compatible With Installed Firmware: " << CompatFirmware << "\n";

	String^ Name = LMFTracker->Name;
	cout << "Name: " << (decode)(Name) << "\n";

	String^ ProductName = LMFTracker->ProductName;
	cout << "ProductName: " << (decode)(ProductName) << "\n";

	String^ Serial = LMFTracker->SerialNumber;
	cout << "Serial: " << (decode)(Serial) << "\n";


	// generates some binary debug file that has limited use to us
	// 
	//  String^ GenerateLFile = LMFTracker->GenerateLFile();
	//  cout << "GenerateLFile: " << (decode)(GenerateLFile) << "\n";    /*

	LMFTracker->GetDirectionAsync();
	Direction^ dir1 = LMFTracker->GetDirection();
	cout << "Direction H Angle: " << dir1->HorizontalAngle->Value << " V Angle: " << dir1->VerticalAngle->Value << "\n";

	cout << " HLabel " << (decode)(dir1->HorizontalAngle->Label);
	cout << " HUnitString " << (decode)(dir1->HorizontalAngle->UnitString);
	//    cout << "HUnitType "    << dir1->HorizontalAngle->UnitType;
	cout << " HValueInBaseUnits " << dir1->HorizontalAngle->ValueInBaseUnits << "\n";

	cout << " VLabel " << (decode)(dir1->VerticalAngle->Label);
	cout << " VUnitString " << (decode)(dir1->VerticalAngle->UnitString);
	//    cout << "VUnitType "    << dir1->VerticalAngle->UnitType;
	cout << " VValueInBaseUnits " << dir1->VerticalAngle->ValueInBaseUnits << "\n";


	cout << "Performing a Target search . . . which may prevents you from doing sync commands again on some simulators \n";

	LMF::Tracker::Targets::Target^ foundtargets = LMFTracker->TargetSearch->Start();

	cout << " Found " << (decode)(foundtargets->Comment);
	cout << " " << (decode)(foundtargets->Name);
	cout << " " << (decode)(foundtargets->ProductName) << "\n";

	Sleep(1000);


	cout << "Attempting a GetPrismPositionAsync call \n";
	LMFTracker->GetPrismPositionAsync();
	
	Sleep(1000);


	cout << "Attempting a GetPrismPosition call \n";
	try
	{
		Measurement^ measure = LMFTracker->GetPrismPosition(); //says not supported by this tracker
		cout << "Prism Position Humidity: " << measure->Humidity->Value << " Pressure: " << measure->Pressure->Value << " Temperature: " << measure->Temperature->Value << "\n";
	}
	catch (LMF::Tracker::ErrorHandling::LmfException^ e)
	{
		cout << (decode)(e->Description) << "\n";;
		cout << "Hit an exception trying to perform a Get Prism Position call \n";
	}

	//    cout << "Starting  GoHomePosition Async. . . \n";
	try
	{
		//       LMFTracker->GoHomePositionAsync();
	}
	catch (LMF::Tracker::ErrorHandling::LmfException^ e)
	{
		cout << (decode)(e->Description) << "\n";;
		cout << "Hit an exception trying to perform a GoHomePositionAsync call \n";
	}


	//    cout << "Starting GoHomePosition . . . \n";
	try
	{
		//        LMFTracker->GoHomePosition();
	}
	catch (LMF::Tracker::ErrorHandling::LmfException^ e)
	{
		cout << (decode)(e->Description) << "\n";;
		cout << "Hit an exception trying to perform a GoHomePosition call \n";
	}



	DateTime wakeuptime;
	//  wakeuptime = wakeuptime->Now;
	//  wakeuptime = wakeuptime->AddMinutes(1.0);

	// The running simulator claims this exits, the actual Type Library / dll code says that it does not *sigh*
	// probably related to checking if something is still in place periodically?
	//LMFTracker->GotoStandBy(wakeuptime); 


	  // so setting to some absolute position

	Boolean searchtarget = false;
	Boolean isrelative = false;
	Double pos1 = 0.0, pos2 = 0.0, pos3 = 0.0;

	//   cout << "Setting Positionto pos1: " << pos1 << " pos2: " << pos2 << " pos3: " << pos3 << "\n";
	try
	{
		//       LMFTracker->PositionToAsync(searchtarget, isrelative, pos1, pos2, pos3);
	}
	catch (LMF::Tracker::ErrorHandling::LmfException^ e)
	{
		cout << (decode)(e->Description) << "\n";
		cout << "Hit an exception trying to perform a PositionToAsyn call \n";
	}


	pos1 = 0.0, pos2 = 0.0, pos3 = 0.0;
	//  cout << "Setting Positionto pos1: " << pos1 << " pos2: " << pos2 << " pos3: " << pos3 << "\n";
	try
	{
		//      LMFTracker->PositionTo(searchtarget, isrelative, pos1, pos2, pos3);
	}
	catch (LMF::Tracker::ErrorHandling::LmfException^ e)
	{
		cout << (decode)(e->Description) << "\n";;
		cout << "Hit an exception trying to perform a PositionTo call \n";
	}


	// actually then moving to that psoition

	Int32 percentspeedH = 5;
	Int32 percentspeedV = 5;
	//  
	//  cout << "Trying to Start Move . . . \n";

	try
	{
		//       LMFTracker->Move(percentspeedH, percentspeedV);
	}
	catch (LMF::Tracker::ErrorHandling::LmfException^ e)
	{
		cout << (decode)(e->Description) << "\n";;
		cout << "Hit an exception trying to perform a Move call \n";
	}


	//   LMFTracker->OpenTrackerScope();// not supported


   // functions
   //    LMFTracker->Disconnect();
   //    LMFTracker->Dispose();





   /*
   // Needs actual targets to do anything . . .
	   LockOnToken^ lockontoken;


	   LMFTracker->PositionToTarget(lockontoken, isrelative, pos1, pos2, pos3); // not supported
	   LMFTracker->PositionToTargetAsync(lockontoken, isrelative, pos1, pos2, pos3);// not supported
	   LMFTracker->ShutDown();
	   LMFTracker->StopMove();// not supported




   // Compensations
   // Face
   // InclinationSensor
   // Laser
   // Measurement
   //   LMFTracker->Measurement->MeasurementArrived += gcnew LMF::Tracker::Measurements::MeasurementSettings::MeasurementArrivedHandler(&OnMeasurementArrived);
   //   LMFTracker->Measurement->StartMeasurement();

	//  LMF::Tracker::Measurements::MeasurementSettings^ thing;
	//  thing->MeasureStationary();

   */

	cout << "Perform a Measure Stationary . . .  \n";

	CheckForErrors(LMFTracker);
	CheckForMeasurementErrors(LMFTracker);

	LMF::Tracker::MeasurementResults::Measurement^ data = LMFTracker->Measurement->MeasureStationary();

	cout << "Measurment Humidity: " << data->Humidity->Value << " Pressure: " << data->Pressure->Value << " Temperature: " << data->Temperature->Value << "\n";

	StationaryMeasurement3D^ stationaryMeas3D = dynamic_cast<StationaryMeasurement3D^>(data);
	cout << " X = " << stationaryMeas3D->Position->Coordinate1->Value;
	cout << " Y = " << stationaryMeas3D->Position->Coordinate2->Value;
	cout << " Z = " << stationaryMeas3D->Position->Coordinate3->Value << "\n";

	Sleep(2000);

	CheckForErrors(LMFTracker);
	CheckForMeasurementErrors(LMFTracker);

	cout << "Doing a StartMeasurement . . . status: " << LMFTracker->Measurement->MeasurementInProgress->Value << "\n";
	LMFTracker->Measurement->StartMeasurement();

	Sleep(20000);
	LMFTracker->Measurement->StopMeasurement();
	cout << "Doing a StopMeasurement . . . \n";






	// MeteoStation
	// OverviewCamera
	// PowerLock
	// PowerSource
	// QuickRelease
	// Settings
	// Targets
	// TargetSearch
	// TrackerAlignment
	// Triggers

	cout << "Shutdown . . . \n";
	LMFTracker->ShutDown();

	cout << "Disconnect . . . \n";
	LMFTracker->Disconnect();

}



void OnDisconnected(LMF::Tracker::Tracker^ sender, LMF::Tracker::ErrorHandling::LmfException^ ex)
{
	//   throw gcnew System::NotImplementedException();
	cout << (decode)(ex->Description) << "\n";;
	cout << "callback Disconnected finished . . . \n";
}

void OnErrorArrived(LMF::Tracker::Tracker^ sender, LMF::Tracker::ErrorHandling::LmfError^ error)
{
	//   throw gcnew System::NotImplementedException();
	cout << (decode)(error->Description) << "\n";;
	cout << "callback Got some sort of error message . . . \n";
}

void OnGetDirectionFinished(LMF::Tracker::Tracker^ sender, LMF::Tracker::Direction^ bm, LMF::Tracker::ErrorHandling::LmfException^ ex)
{
	//   throw gcnew System::NotImplementedException();
	cout << (decode)(ex->Description) << "\n";;
	cout << "callback Got some sort of Get Direction finished message . . . \n";

	cout << "Direction H Angle: " << bm->HorizontalAngle->Value << " V Angle: " << bm->VerticalAngle->Value << "\n";


}

void OnGetPrismPositionFinished(LMF::Tracker::Tracker^ sender, LMF::Tracker::MeasurementResults::Measurement^ paramMeasurement, LMF::Tracker::ErrorHandling::LmfException^ ex)
{
	//   throw gcnew System::NotImplementedException();
	cout << (decode)(ex->Description) << "\n";;
	cout << "callback OnGetPosition Finished . . . \n";

}

void OnGoHomePositionFinished(LMF::Tracker::Tracker^ sender, LMF::Tracker::ErrorHandling::LmfException^ ex)
{
	//   throw gcnew System::NotImplementedException();
	cout << (decode)(ex->Description) << "\n";;
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
	cout << (decode)(ex->Description) << "\n";;
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
	cout << "callback Got some sort of Warning message . . . \n";
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

					cout << " X = " << stationaryMeas3D->Position->Coordinate1->Value;
					cout << " Y = " << stationaryMeas3D->Position->Coordinate2->Value;
					cout << " Z = " << stationaryMeas3D->Position->Coordinate3->Value << "\n";


				}
				else if (StationaryMeasurement6D^ stationaryMeas6D = dynamic_cast<StationaryMeasurement6D^>(LastMeasurement))
				{
					cout << "I am a stationary6d measurement \n";
					cout << " X = " << stationaryMeas6D->Position->Coordinate1->Value;
					cout << " Y = " << stationaryMeas6D->Position->Coordinate2->Value;
					cout << " Z = " << stationaryMeas6D->Position->Coordinate3->Value << "\n";


				}
				else if (SingleShotMeasurement3D^ singleshot3dD = dynamic_cast<SingleShotMeasurement3D^>(LastMeasurement))
				{
					cout << "I am a singleshot 3d measurement \n";

					cout << " X = " << singleshot3dD->Position->Coordinate1->Value;
					cout << " Y = " << singleshot3dD->Position->Coordinate2->Value;
					cout << " Z = " << singleshot3dD->Position->Coordinate3->Value << "\n";



				}
				else if (SingleShotMeasurement6D^ singleshot6dD = dynamic_cast<SingleShotMeasurement6D^>(LastMeasurement))
				{
					cout << "I am a singleshot 6d measurement \n";
					cout << " X = " << singleshot6dD->Position->Coordinate1->Value;
					cout << " Y = " << singleshot6dD->Position->Coordinate2->Value;
					cout << " Z = " << singleshot6dD->Position->Coordinate3->Value << "\n";


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
