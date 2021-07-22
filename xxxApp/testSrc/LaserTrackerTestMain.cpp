//
// LaserTracker.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>

//laser tracker realted headers


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



int main()
{
    LMF::Tracker::Tracker^ LMFTracker;
   
    Connection^ con = gcnew Connection();
  
    LMFTracker = con->Connect("At403Simulator");

// Callbacks and I do not see any advantage to using any of the async ones at the instant, most are not supportd on this hardware

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
  

    LMFTracker->Initialize();
//    LMFTracker->InitializeAsync();
// in namespace LMF::tracker
// the simple values 

    String^ Comment = LMFTracker->Comment;
    cout << "Comment: " << msclr::interop::marshal_as<std::string>(Comment) << "\n";   
     
    String^ Firmware = LMFTracker->ExpectedFirmware;
    cout << "Firmware: " << msclr::interop::marshal_as<std::string>(Firmware) << "\n";

    String^ InstalledFirmware = LMFTracker->InstalledFirmware;
    cout << "InstalledFirmware: " << msclr::interop::marshal_as<std::string>(InstalledFirmware) << "\n";

    String^ IP = LMFTracker->IPAddress;
    cout << "IP: " << msclr::interop::marshal_as<std::string>(IP) << "\n";

    Boolean CompatFirmware = LMFTracker->IsCompatibleWithInstalledFirmware;
    cout << "Is Compatible With Installed Firmware: " << CompatFirmware << "\n";

    String^ Name = LMFTracker->Name;
    cout << "Name: " << msclr::interop::marshal_as<std::string>(Name) << "\n";
    
    String^ ProductName = LMFTracker->ProductName;
    cout << "ProductName: " << msclr::interop::marshal_as<std::string>(ProductName) << "\n";

    String^ Serial = LMFTracker->SerialNumber;
    cout << "Serial: " << msclr::interop::marshal_as<std::string>(Serial) << "\n";

// generates some binary debug file that has limited use
//  String^ GenerateLFile = LMFTracker->GenerateLFile();
//  cout << "GenerateLFile: " << msclr::interop::marshal_as<std::string>(GenerateLFile) << "\n";    /*

    LMFTracker->GetDirectionAsync();    
    Direction^ dir1 = LMFTracker->GetDirection(); 
    cout << "Direction H Angle: " << dir1->HorizontalAngle->Value << " V Angle: " << dir1->VerticalAngle->Value << "\n";

// If and when something throws and error, this is how to decode it 
//    Int32 ErrorNumber = 0;
//    LmfError^ err = LMFTracker->GetErrorDescription(ErrorNumber);

//    LMFTracker->GetPrismPositionAsync(); 
//  Measurement^ measure = LMFTracker->GetPrismPosition(); //says not supported by this tracker
//  cout << "Prism Position Humidity: " << measure->Humidity->Value << " Pressure: " << measure->Pressure->Value << " Temperature: " << measure->Temperature->Value << "\n";

//    cout << "Starting  GoHomePosition Async. . . \n";
    try
    {
 //       LMFTracker->GoHomePositionAsync();
    }
    catch (LMF::Tracker::ErrorHandling::LmfException^ e)
    {
        cout << "Hit an exception trying to perform a GoHomePositionAsync call \n";
    }

 
//    cout << "Starting GoHomePosition . . . \n";
    try
    {
//        LMFTracker->GoHomePosition();
    }
    catch (LMF::Tracker::ErrorHandling::LmfException^ e)
    {
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
   catch  (LMF::Tracker::ErrorHandling::LmfException^ e)
   {
       cout << "Hit an exception trying to perform a Move call \n";
   }


 //   LMFTracker->OpenTrackerScope();// not supported


// functions
//    LMFTracker->Disconnect();
//    LMFTracker->Dispose();

/*




 //    LockOnToken^ lockontoken;
//    LMFTracker->PositionToTarget(lockontoken, isrelative, pos1, pos2, pos3);// not supported
//    LMFTracker->PositionToTargetAsync(lockontoken, isrelative, pos1, pos2, pos3);// not supported
//   LMFTracker->ShutDown();
//    LMFTracker->StopMove();// not supported




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

LMF::Tracker::MeasurementResults::Measurement^ data = LMFTracker->Measurement->MeasureStationary();

 cout << "Measurment Humidity: " << data->Humidity->Value << " Pressure: " << data->Pressure->Value << " Temperature: " << data->Temperature->Value << "\n";
 
 StationaryMeasurement3D^ stationaryMeas3D = dynamic_cast<StationaryMeasurement3D^>(data);
 cout << " X = " << stationaryMeas3D->Position->Coordinate1->Value << "\n";
 cout << " Y = " << stationaryMeas3D->Position->Coordinate2->Value << "\n";
 cout << " Z = " << stationaryMeas3D->Position->Coordinate3->Value << "\n";

 
 cout << "Doing a StartMeasurement . . . status: " << LMFTracker->Measurement->MeasurementInProgress->Value << "\n";
 LMFTracker->Measurement->StartMeasurement();

 Sleep(10000);
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






    LMFTracker->ShutDown();


    LMFTracker->Disconnect();

}



void OnDisconnected(LMF::Tracker::Tracker^ sender, LMF::Tracker::ErrorHandling::LmfException^ ex)
{
 //   throw gcnew System::NotImplementedException();
    cout << "callback Disconnected finished . . . \n";
}

void OnErrorArrived(LMF::Tracker::Tracker^ sender, LMF::Tracker::ErrorHandling::LmfError^ error)
{
 //   throw gcnew System::NotImplementedException();
    cout << "callback Got some sort of error message . . . \n";
}

void OnGetDirectionFinished(LMF::Tracker::Tracker^ sender, LMF::Tracker::Direction^ bm, LMF::Tracker::ErrorHandling::LmfException^ ex)
{
 //   throw gcnew System::NotImplementedException();
    cout << "callback Got some sort of Get Direction finished message . . . ";
    
    cout << "Direction H Angle: " << bm->HorizontalAngle->Value << " V Angle: " << bm->VerticalAngle->Value << "\n";

}

void OnGetPrismPositionFinished(LMF::Tracker::Tracker^ sender, LMF::Tracker::MeasurementResults::Measurement^ paramMeasurement, LMF::Tracker::ErrorHandling::LmfException^ ex)
{
    throw gcnew System::NotImplementedException();

}

void OnGoHomePositionFinished(LMF::Tracker::Tracker^ sender, LMF::Tracker::ErrorHandling::LmfException^ ex)
{
 //   throw gcnew System::NotImplementedException();
    cout << "callback Asyn GoHomePosition finished . . . \n";
}

void OnInformationArrived(LMF::Tracker::Tracker^ sender, LMF::Tracker::ErrorHandling::LmfInformation^ paramInfo)
{
 //   throw gcnew System::NotImplementedException();
    cout << "callback Got some sort of Information message . . . \n";
}

void OnInitializeFinished(LMF::Tracker::Tracker^ sender, LMF::Tracker::ErrorHandling::LmfException^ ex)
{
 //   throw gcnew System::NotImplementedException();
    cout << "callback Initialization finished . . . \n";
    
}

void OnPositionToFinished(LMF::Tracker::Tracker^ sender, LMF::Tracker::Targets::Target^ foundTarget, LMF::Tracker::ErrorHandling::LmfException^ ex)
{
  //  throw gcnew System::NotImplementedException();

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
   // throw gcnew System::NotImplementedException();
    cout << "callback Got a Measurement Value . . . \n";

    cout << "count :" << paramMeasurements->Count << "\n";
    
    cout << msclr::interop::marshal_as<std::string>(paramMeasurements->GetType()->FullName) << "\n";

    
// Does not work as 6d, but simulator says that it should be this

    if (StationaryMeasurement3D^ stationaryMeas3D = dynamic_cast<StationaryMeasurement3D^>(paramMeasurements))
    {
        cout << "I am a stationary3d measurement \n ";

    }
    else cout << " not a 1 ";

    if (StationaryMeasurement6D^ stationaryMeas6D = dynamic_cast<StationaryMeasurement6D^>(paramMeasurements))
    {
        cout << "I am a stationary6d measurement \n ";

    }cout << " not a 2 ";

    if (SingleShotMeasurement3D^ singleshot3dD = dynamic_cast<SingleShotMeasurement3D^>(paramMeasurements))
    {
        cout << "I am a singleshot 3d measurement \n ";

    }cout << " not a 3 ";

    if (SingleShotMeasurement6D^ singleshot6dD = dynamic_cast<SingleShotMeasurement6D^>(paramMeasurements))
    {
        cout << "I am a singleshot 6d measurement \n ";

    }cout << " not a 4 ";




//    cout << stationaryMeas6D->Temperature->Value;   

    //does not work as 3d
    // 
//    StationaryMeasurement3D^ stationaryMeas3D = dynamic_cast<StationaryMeasurement3D^>(paramMeasurements);
//    cout << stationaryMeas3D->Temperature->Value;

//    SingleShotMeasurement6D^ singleShotMeas6D = dynamic_cast<SingleShotMeasurement6D^>(fromMeasurement)
//    SingleShotMeasurement6D ^ singleShotMeas6D = dynamic_cast<SingleShotMeasurement6D^>(fromMeasurement)

 //   cout << "Measurment Humidity: " << stationaryMeas6D->Humidity->Value << " Pressure: " << stationaryMeas6D->Pressure->Value << " Temperature: " << stationaryMeas6D->Temperature->Value << "\n";

 //   cout << " X = " << stationaryMeas6D->Position->Coordinate1->Value << "\n";
 //   cout << " Y = " << stationaryMeas6D->Position->Coordinate2->Value << "\n";
 //   cout << " Z = " << stationaryMeas6D->Position->Coordinate3->Value << "\n";

    
}

void OnChanged(LMF::Tracker::MeasurementStatus::MeasurementStatusValue^ sender, LMF::Tracker::Enums::EMeasurementStatus paramNewValue)
{
//    throw gcnew System::NotImplementedException();
    cout << "Measurement Status Value changed: "  << "\n";
        

}
