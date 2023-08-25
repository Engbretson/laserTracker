//
// LaserTracker.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "QueryTrackerState.h"


int mainLT(void);

#ifdef DO_MAIN
int main()
{
	mainLT();
}
#endif


int mainLT()
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

	std::cout << "Searching for trackers . . . ";

	// The TrackerFinder holds a list of found Trackers, but doesn't seem to find any when the hardware is on a private address
	// hard encoding a static address  . . . doesn't seem to work, but dhcp on the last device *did* work, I think

	TrackerFinder^ trackerFinder = gcnew TrackerFinder();
	TrackerInfoCollection^ foundTrackers = trackerFinder->Trackers;

	std::cout << "Found : " << foundTrackers->Count << std::endl;

	String^ UseLastTracker;

	for (int i = 0; i < foundTrackers->Count; i++)
	{
		TrackerInfo^ tracker = foundTrackers[i];

		std::cout << " Tracker Name: " << (decode)(tracker->Name);
		std::cout << " Serial Number: " << (decode)(tracker->SerialNumber);
		std::cout << " IP Address: " << (decode)(tracker->IPAddress);
		UseLastTracker = tracker->IPAddress;
		std::cout << " Type: " << (decode)(tracker->Type) << std::endl;
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

	//	std::cout << "Connecting to At403Simulator \n";
	//	std::cout << "Connecting to At930Simulator \n";
	//	std::cout << "Connecting to 164.54.116.53 \n";
	//	std::cout << "Connecting to " << (decode)(UseLastTracker) < std::endl;

	//	LMFTracker = con->Connect("At403Simulator");
	//	LMFTracker = con->Connect("At930Simulator");



// right now, try the real hardware or do the simulator, no other behavior is reasonable

	try {
		std::cout << "Alwats try Connecting to 164.54.116.53 \n";
			LMFTracker = con->Connect("164.54.116.53");
//		LMFTracker = con->Connect(UseLastTracker);
	}
	catch (LMF::Tracker::ErrorHandling::LmfException^ e)
	{
		std::cout << "Error code: " << e->Number << " " << (decode)(e->Description) << std::endl;;
		//		std::cout << "Hit an exception trying to perform a Connect call, Exiting. \n";

		//		exit(-1);
		std::cout << "No actual Hardware seen . . . . using Simulator \n";
		LMFTracker = con->Connect("At930Simulator");
	}

	////
	//// I do not want the Simulator Control screen up at all, even when talking to Simulated hardware
	//// This does not apply at all to real hardware , but you *can* turn it on
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

	std::cout << "Is the hardware ready ?\n";

	CheckForErrors(LMFTracker);

	// not valid here, yet
	//	CheckForMeasurementErrors(LMFTracker);


	// This actually gets tricky , since none of this code actually works correctly if the device is warming up 
	// You can initialze, so can't move anything, or do anything target related
	// do ideally, you need to just sit here and wait and retry until it is . . .  happy?
	// the docs say that this could be as long as 2 *hours*, but more typically 5-7 minutes
	//

	std::cout << "Initialize . . . \n";

	// depricated warning that actually make the compiler and/pr Intellicade very unhappy . . . 

#pragma warning(disable : 4996)

	time_t rawtime;
	struct tm* timeinfo;
	time(&rawtime);

	timeinfo = localtime(&rawtime);
	std::cout << "Starting Day, Date and Time is = " << asctime(timeinfo);

	int check = 1;
	while (check) {
		try {
			check = 0;
			LMFTracker->Initialize();

		}
		catch (LMF::Tracker::ErrorHandling::LmfException^ e)
		{
			check = 1;
			if (LMFTracker->Laser->IsOn->Value == 0) {
				std::cout << "laser is off . . . Attemping Laser On . . . " << std::endl;
				LMFTracker->Laser->IsOn->Value = 1;
			}
			std::cout << "Initialization Error Code: " << e->Number << " " << (decode)(e->Description) << std::endl;
			//		std::cout << "Hit an exception trying to perform an Initialize  call \n";
			//		std::cout << "waiting for no Initialze exceptions . . . .\n";
			Sleep(15000); // 15 seconds between test

		}

	}
	time(&rawtime);
	timeinfo = localtime(&rawtime);
	std::cout << "Current Day, Date and Time is = " << asctime(timeinfo);

	// do 1 or the other - trying both queues up the asyn to try happening *after* the current program ends and various code faults out with 
	//	"Initialization not done yet" errors
	/*
		std::cout << "Initialize Async . . . \n";
		try {
			LMFTracker->InitializeAsync();
		}
		catch (LMF::Tracker::ErrorHandling::LmfException^ e)
		{
			std::cout << "Error code: " << e->Number << " " << (decode)(e->Description) << std::endl;
			std::cout << "Hit an exception trying to perform an Initilize Async call \n";
		}
	*/

	std::cout << "After Initialization . . . Check for Errors? \n\n";

	CheckForErrors(LMFTracker);

	// not valid code to check, yet maybe? nope

	//CheckForMeasurementErrors(LMFTracker);

	// 5

	std::cout << "Top Level  Parameters . . . \n";

	String^ Comment = LMFTracker->Comment;
	std::cout << "Comment: " << (decode)(Comment) << std::endl;

	String^ Firmware = LMFTracker->ExpectedFirmware;
	std::cout << "ExpectedFirmware: " << (decode)(Firmware) << std::endl;

	String^ InstalledFirmware = LMFTracker->InstalledFirmware;
	std::cout << "InstalledFirmware: " << (decode)(InstalledFirmware) << std::endl;

	String^ IP = LMFTracker->IPAddress;
	std::cout << "IPAddress: " << (decode)(IP) << std::endl;

	Boolean CompatFirmware = LMFTracker->IsCompatibleWithInstalledFirmware;
	std::cout << "IsCompatibleWithInstalledFirmware: " << TFS[CompatFirmware] << std::endl;

	String^ Name = LMFTracker->Name;
	std::cout << "Name: " << (decode)(Name) << std::endl;

	String^ ProductName = LMFTracker->ProductName;
	std::cout << "ProductName: " << (decode)(ProductName) << std::endl;

	String^ Serial = LMFTracker->SerialNumber;
	std::cout << "SerialNumber: " << (decode)(Serial) << "\n\n";

	// Top Level additional Info

	// Get Direction
	// Get Direction Async
	// Get Error Description (probably typically used betwen/after every command)
	// Get Prism Position - Nope, this actually requires a locked on target and measureming
	/*
		std::cout << "Attempting a GetPrismPosition call \n";

		try
		{
					Measurement^ measure = LMFTracker->GetPrismPosition(); //says not supported by this tracker
					std::cout << "Prism Position Humidity: " << measure->Humidity->Value << " Pressure: " << measure->Pressure->Value << " Temperature: " << measure->Temperature->Value << std::endl;
		}
		catch (LMF::Tracker::ErrorHandling::LmfException^ e)
		{
			std::cout << "Error code: " << e->Number << " " << (decode)(e->Description) << std::endl;
			std::cout << "Hit an exception trying to perform a Get Prism Position  call \n";
		}


	// Get Prism Position Async

		std::cout << "Attempting a GetPrismPositionAsync call \n";

		try
		{
			LMFTracker->GetPrismPositionAsync();
		}
		catch (LMF::Tracker::ErrorHandling::LmfException^ e)
		{
			std::cout << "Error code: " << e->Number << " " << (decode)(e->Description) << std::endl;
			std::cout << "Hit an exception trying to perform a Get Prism Position Async call \n";
		}

		Sleep(1000);

	*/

	// Try to locate a known to be existing reflector, since a lot of this code doesn't actually work unless it is looking at something real

	std::cout << "Trying to pre-position to known position . . . " << std::endl;

	CALL_AND_HANDLE(LMFTracker->PositionTo(true, false, 546, 3059, 690));
//	LMFTracker->GetPrismPosition();
    Sleep(1000);
	CALL_AND_HANDLE(LMFTracker->GetPrismPosition());

//	LMFTracker->PositionTo(true, false, 546, 3059, 690);

	// These sorts of tests/checks should be valid now

	CheckForMeasurementErrors(LMFTracker);


	Do_Compensations(LMFTracker);
	Do_Face(LMFTracker);
	Do_InclinationSensor(LMFTracker);
	Do_Laser(LMFTracker);
	Do_Measurement(LMFTracker);
	Do_MeteoStation(LMFTracker);
	Do_OverviewCamera(LMFTracker);
	Do_PowerLock(LMFTracker);
	Do_PowerSource(LMFTracker);
	Do_QuickRelease(LMFTracker);
	Do_Settings(LMFTracker);
	Do_Targets(LMFTracker);
	Do_TargetSearch(LMFTracker);
	Do_TrackerAlignment(LMFTracker);
	Do_Triggers(LMFTracker);
	Do_WrtlBoxes(LMFTracker);


	system("pause");

	// This actually power downs the Unit!!!!!!!!	
	//	std::cout << "Shutdown . . . \n";
	//	LMFTracker->ShutDown();
	//
	// but certainly could turn off the laser if you plan on leaving it on, but still have a warm-up time

//	LMFTracker->Laser->IsOn->Value = 0;


	std::cout << "Disconnect . . . \n";
	LMFTracker->Disconnect();
	return 0;
}

