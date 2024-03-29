/* leica.cpp
 *
 * This is a driver for a simulated area detector.
 *
 * Author: Mark Rivers
 *         University of Chicago
 *
 * Created:  March 20, 2008
 *
 */

#include <stddef.h>
#include <stdlib.h>
#include <stdarg.h>
#include <math.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>

#include <epicsTime.h>
#include <epicsThread.h>
#include <epicsEvent.h>
#include <epicsMutex.h>
#include <epicsString.h>
#include <epicsStdio.h>
#include <epicsMutex.h>
#include <cantProceed.h>
#include <iocsh.h>

#include "ADDriver.h"
#include <epicsExport.h>
#include "leica.h"


 //
 // Some of the callback just WON"T allow me to stick the function defination within the class header file
 // But work fine here. Why waste time argueing with the compiler or TypeLibrary
 //

 //void OnWarnChanged(LMF::Tracker::BasicTypes::BoolValue::ReadOnlyBoolValue^ sender, bool paramNewValue);
 //void OOnChanged(LMF::Tracker::BasicTypes::BoolValue::ReadOnlyBoolValue^ sender, bool paramNewValue);

static const char* driverName = "leica";

//
// Laser Tracker Related helper functions
//

std::string decode(System::String^ something)
{
	if (System::String::IsNullOrEmpty(something))
		return "";
	else
		return (msclr::interop::marshal_as<std::string>(something));
}

// common printable entity templates ?? Turned off the creation of callbacks - I just want the settings, now. Callbacks can be created as required . . . later

void Do_BoolValue(String^ Title, LMF::Tracker::BasicTypes::BoolValue::ReadOnlyBoolValue^ thing)
{
	//	thing->Changed += gcnew LMF::Tracker::BasicTypes::BoolValue::ReadOnlyBoolValue::ChangedEventHandler(&OnChanged);
	std::cout << (decode)(Title) << " Label: " << (decode)(thing->Label) <<
		" Value: " << TFS[(int)thing->Value] <<
		std::endl;

}

void Do_ReadOnlyDoubleValue(String^ Title, LMF::Tracker::BasicTypes::DoubleValue::ReadOnlyDoubleValue^ thing)
{
	//	thing->Changed += gcnew LMF::Tracker::BasicTypes::DoubleValue::ReadOnlyDoubleValue::ChangedEventHandler(&OnChanged);

	std::cout << (decode)(Title) << ": Label: " << (decode)(thing->Label) <<
		" UnitString: " << (decode)(thing->UnitString) <<
		" UnitType: " << EUnitTypeStrings[(int)thing->UnitType] <<
		" Value: " << thing->Value <<
		" ValueInBaseUnits: " << thing->ValueInBaseUnits <<
		std::endl;

}

void Do_SimpleDoubleValue(String^ Title, LMF::Tracker::BasicTypes::DoubleValue::SimpleDoubleValue^ thing)
{
	//	thing->Changed += gcnew LMF::Tracker::BasicTypes::DoubleValue::ReadOnlyDoubleValue::ChangedEventHandler(&OnChanged);



	std::cout << (decode)(Title) << ": Label: " << (decode)(thing->Label) <<
		" UnitString: " << (decode)(thing->UnitString) <<
		" UnitType: " << EUnitTypeStrings[(int)thing->UnitType] <<
		" Value: " << thing->Value <<
		" ValueInBaseUnits: " << thing->ValueInBaseUnits <<
		std::endl;

	// something is printing out -0 sporatically, but no format string ever actually explains it
	//FWIIW, since this happens on code that just called attempting to do a 'getorientation', the speculation is that the internal math
	// does a divide by negative infinity, or some other nonsence, so it hits the negative representation of zero and not the positive one
	// https://en.wikipedia.org/wiki/Signed_zero

//	printf("+++ %e +++", thing->ValueInBaseUnits); // or g
}

void Do_EnvironmentalSensor(String^ Title, LMF::Tracker::Meteo::EnvironmentalSensor^ thing)
{
	std::cout << (decode)(Title) << std::endl <<
		" Label: " << (decode)(thing->Label) <<
		" SerialNumber: " << (decode)(thing->SerialNumber) <<
		" UnitString: " << (decode)(thing->UnitString) <<
		" UnitType: " << EUnitTypeStrings[(int)thing->UnitType] <<
		" Value: " << thing->Value <<
		" ValueInBaseUnits: " << thing->ValueInBaseUnits <<
		" Available: " <<
		" Label: " << (decode)(thing->Available->Label) <<
		" Value: " << TFS[(int)thing->Available->Value] <<
		//		" Value: " << thing->Available->Value <<
		std::endl;
}

void Do_ManualEnvironmentalSensor(String^ Title, LMF::Tracker::Meteo::ManualEnvironmentalSensor^ thing)
{
	std::cout << (decode)(Title) << std::endl <<
		" Label: " << (decode)(thing->Label) <<
		" MaxValue: " << thing->MaxValue <<
		" MaxValueInBaseUnits: " << thing->MaxValueInBaseUnits <<
		" MinValue: " << thing->MinValue <<
		" MinValueInBaseUnits: " << thing->MinValueInBaseUnits <<
		" SerialNumber: " << (decode)(thing->SerialNumber) <<
		" UnitString: " << (decode)(thing->UnitString) <<
		" UnitType: " << EUnitTypeStrings[(int)thing->UnitType] <<
		" Value: " << thing->Value <<
		" ValueInBaseUnits: " << thing->ValueInBaseUnits <<
		" Available: " <<
		" Label: " << (decode)(thing->Available->Label) <<
		" Value: " << TFS[(int)thing->Available->Value] <<
		//		" Value: " << thing->Available->Value <<
		std::endl;
}

void Do_SimpleDoubleValueWithAccuracy(String^ Title, LMF::Tracker::BasicTypes::DoubleValue::SimpleDoubleValueWithAccuracy^ thing)
{
	std::cout << (decode)(Title) << std::endl <<
		" Accuracy: " << thing->Accuracy <<
		" AccuracyInBaseUnits: " << thing->AccuracyInBaseUnits <<
		" Label: " << (decode)(thing->Label) <<
		" UnitString: " << (decode)(thing->UnitString) <<
		" UnitType: " << EUnitTypeStrings[(int)thing->UnitType] <<
		" Value: " << thing->Value <<
		" ValueInBaseUnits: " << thing->ValueInBaseUnits <<
		std::endl;
}

void Do_DoubleValueWithRange(String^ Title, LMF::Tracker::BasicTypes::DoubleValue::DoubleValueWithRange^ thing)
{
	//	thing->Changed += gcnew LMF::Tracker::BasicTypes::DoubleValue::ReadOnlyDoubleValue::ChangedEventHandler(&OnChanged);

	std::cout << (decode)(Title) << ": Label: " << (decode)(thing->Label) <<
		" MaxValue: " << thing->MaxValue <<
		" MaxValueInBaseUnits: " << thing->MaxValueInBaseUnits <<
		" MinValue: " << thing->MinValue <<
		" MinValueInBaseUnits: " << thing->MinValueInBaseUnits <<
		" UnitString: " << (decode)(thing->UnitString) <<
		" UnitType: " << EUnitTypeStrings[(int)thing->UnitType] <<
		" Value: " << thing->Value <<
		" ValueInBaseUnits: " << thing->ValueInBaseUnits <<
		std::endl;
}

void Do_SimpleDoubleValueWithRange(String^ Title, LMF::Tracker::BasicTypes::DoubleValue::SimpleDoubleValueWithRange^ thing)
{
	//	thing->Changed += gcnew LMF::Tracker::BasicTypes::DoubleValue::ReadOnlyDoubleValue::ChangedEventHandler(&OnChanged);

	std::cout << (decode)(Title) << ": Label: " << (decode)(thing->Label) <<
		" MaxValue: " << thing->MaxValue <<
		" MaxValueInBaseUnits: " << thing->MaxValueInBaseUnits <<
		" MinValue: " << thing->MinValue <<
		" MinValueInBaseUnits: " << thing->MinValueInBaseUnits <<
		" UnitString: " << (decode)(thing->UnitString) <<
		" UnitType: " << EUnitTypeStrings[(int)thing->UnitType] <<
		" Value: " << thing->Value <<
		" ValueInBaseUnits: " << thing->ValueInBaseUnits <<
		std::endl;
}

void Do_IntValueWithRange(String^ Title, LMF::Tracker::BasicTypes::IntValue::IntValueWithRange^ thing)
{
	//	thing->Changed += gcnew LMF::Tracker::BasicTypes::IntValue::ReadOnlyIntValue::ChangedEventHandler(&OnChanged);

	std::cout << (decode)(Title) << ": Label: " << (decode)(thing->Label) <<
		" MaxValue: " << thing->MaxValue <<
		" MaxValueInBaseUnits: " << thing->MaxValueInBaseUnits <<
		" MinValue: " << thing->MinValue <<
		" MinValueInBaseUnits: " << thing->MinValueInBaseUnits <<
		" UnitString: " << (decode)(thing->UnitString) <<
		" UnitType: " << EUnitTypeStrings[(int)thing->UnitType] <<
		" Value: " << thing->Value <<
		" ValueInBaseUnits: " << thing->ValueInBaseUnits <<
		std::endl;
}

void Do_MeasurmentInfo(String^ Title, LMF::Tracker::MeasurementResults::MeasurementInfo^ thing)
{

	std::cout << (decode)(Title) << ": Label: " << std::endl;

	std::cout << " FirstMeasAfterStartSignal: " << TFS[(int)thing->FirstMeasAfterStartSignal] << std::endl;
	// note: Flags are a *long* list whose bits are added to get the final thing, not really epicy
	std::cout << " Flags: " << (int)thing->Flags << std::endl;
	std::cout << " FlagsWithWarnings: " << (int)thing->FlagsWithWarnings << std::endl;
	std::cout << " GUID: " << (decode)(thing->GUID) << std::endl;
	std::cout << " GUIDInfo: " << (decode)(thing->GUIDInfo) << std::endl;
	std::cout << " LineIndex: " << thing->LineIndex << std::endl;
	std::cout << " Mode: " << (int)thing->Mode << std::endl;
	std::cout << " MountNr: " << thing->MountNr << std::endl;
	std::cout << " NumberOfLEDSUsed: " << thing->NumberOfLEDsUsed << std::endl;
	std::cout << " NumberofLEDsVisible: " << thing->NumberOfLEDsVisible << std::endl;
	std::cout << " PointIndex: " << thing->PointIndex << std::endl;
	std::cout << " ProbeFace: " << thing->ProbeFace << std::endl;
	std::cout << " ProbeSerialNumber: " << (decode)(thing->ProbeSerialNumber) << std::endl;
	std::cout << " RegionIndex: " << thing->RegionIndex << std::endl;
	std::cout << " TrackerStatusWord: " << (int)thing->TrackerStatusWord << std::endl;
	std::cout << " Type: " << (int)thing->Type << std::endl;

	Do_SimpleDoubleValueWithRange("DistanceBroadening", thing->DistanceBroadening);
	Do_SimpleDoubleValueWithRange("Intensity", thing->Intensity);
	std::cout << (decode)(thing->ToStringWithWarnings()) << std::endl;
}

static void exitHandler(void* drvPvt) {

	//	  printf("\nThis is an exit handler being hit . . . which invoked heavy magic to actually make the destructor get processed.\n"); 

	leica* pleica = (leica*)drvPvt;
	delete pleica;

}
asynStatus leica::writeInt32(asynUser* pasynUser, epicsInt32 value)
{
	int function = pasynUser->reason;
	asynStatus status = asynSuccess;
	const char* paramName;
	const char* functionName = "writeInt32";
	char* whoami;



	/* Set the parameter in the parameter library. */
	status = (asynStatus)setIntegerParam(function, value);
	getParamName(function, (const char**)&whoami);

	/* Fetch the parameter string name for possible use in debugging */
	getParamName(function, &paramName);

//	printf("in writeInt32 . . . function %d  %s value %d\n", function, whoami, value);

	if ((function == L_GotoXY) || (function == L_YY)) {
		try
		{
			int tempx, tempy, temptargets;
			getIntegerParam(L_XX, &tempx);
			getIntegerParam(L_YY, &tempy);
			///		std::cout << "goto x " << tempx << " y " << tempy << std::endl;
			GlobalObjects::LMFTracker->OverviewCamera->MoveToPixel(tempx, tempy, 640, 480);
			getIntegerParam(L_Count_3, &temptargets);
			if (temptargets != 0) {
				GlobalObjects::LMFTracker->TargetSearch->Start();
				// crashes . . . 
				//				GlobalObjects::LMFTracker->Measurement->StartMeasurement();
			}
		}
		catch (...) {
			// this happens if your moving the trager via the mouse, and there is a race condition between if it sees a tracker
			// that used to exist, and no longer exists, so attempts to find something that no longer exists

			;
		}
	}
	if (function == L_GotoHere_1) {
		try
		{
			int tempx, tempy, temptargets;
			getIntegerParam(L_x1, &tempx);
			getIntegerParam(L_y1, &tempy);
			///		std::cout << "goto x " << tempx << " y " << tempy << std::endl;
			GlobalObjects::LMFTracker->OverviewCamera->MoveToPixel(tempx, tempy, 640, 480);
			getIntegerParam(L_Count_3, &temptargets);
			if (temptargets != 0)
				GlobalObjects::LMFTracker->TargetSearch->Start();
		}
		catch (...) {
			// this happens if your moving the trager via the mouse, and there is a race condition between if it sees a tracker
			// that used to exist, and no longer exists, so attempts to find something that no longer exists

			;
		}
	}
	if (function == L_GotoHere_2) {
		try
		{
			int tempx, tempy, temptargets;
			getIntegerParam(L_x2, &tempx);
			getIntegerParam(L_y2, &tempy);
			///		std::cout << "goto x " << tempx << " y " << tempy << std::endl;
			GlobalObjects::LMFTracker->OverviewCamera->MoveToPixel(tempx, tempy, 640, 480);
			getIntegerParam(L_Count_3, &temptargets);
			if (temptargets != 0)
				GlobalObjects::LMFTracker->TargetSearch->Start();
		}
		catch (...) {
			// this happens if your moving the trager via the mouse, and there is a race condition between if it sees a tracker
			// that used to exist, and no longer exists, so attempts to find something that no longer exists

			;
		}
	}
	if (function == L_GotoHere_3) {
		try
		{
			int tempx, tempy, temptargets;
			getIntegerParam(L_x3, &tempx);
			getIntegerParam(L_y3, &tempy);
			///		std::cout << "goto x " << tempx << " y " << tempy << std::endl;
			GlobalObjects::LMFTracker->OverviewCamera->MoveToPixel(tempx, tempy, 640, 480);
			getIntegerParam(L_Count_3, &temptargets);
			if (temptargets != 0)
				GlobalObjects::LMFTracker->TargetSearch->Start();
		}
		catch (...) {
			// this happens if your moving the trager via the mouse, and there is a race condition between if it sees a tracker
			// that used to exist, and no longer exists, so attempts to find something that no longer exists

			;
		}
	}
	if (function == L_GotoHere_4) {
		try
		{
			int tempx, tempy, temptargets;
			getIntegerParam(L_x4, &tempx);
			getIntegerParam(L_y4, &tempy);
			///		std::cout << "goto x " << tempx << " y " << tempy << std::endl;
			GlobalObjects::LMFTracker->OverviewCamera->MoveToPixel(tempx, tempy, 640, 480);
			getIntegerParam(L_Count_3, &temptargets);
			if (temptargets != 0)
				GlobalObjects::LMFTracker->TargetSearch->Start();
		}
		catch (...) {
			// this happens if your moving the trager via the mouse, and there is a race condition between if it sees a tracker
			// that used to exist, and no longer exists, so attempts to find something that no longer exists

			;
		}
	}
	if (function == L_GotoHere_5) {
		try
		{
			int tempx, tempy, temptargets;
			getIntegerParam(L_x5, &tempx);
			getIntegerParam(L_y5, &tempy);
			///		std::cout << "goto x " << tempx << " y " << tempy << std::endl;
			GlobalObjects::LMFTracker->OverviewCamera->MoveToPixel(tempx, tempy, 640, 480);
			getIntegerParam(L_Count_3, &temptargets);
			if (temptargets != 0)
				GlobalObjects::LMFTracker->TargetSearch->Start();
		}
		catch (...) {
			// this happens if your moving the trager via the mouse, and there is a race condition between if it sees a tracker
			// that used to exist, and no longer exists, so attempts to find something that no longer exists

			;
		}
	}


	else if (function == L_GotoXYZ) {
		try
		{
			double tempx, tempy, tempz;
			int temptargets;
			getDoubleParam(L_x00, &tempx);
			getDoubleParam(L_y00, &tempy);
			getDoubleParam(L_z00, &tempz);
			///		std::cout << "goto x " << tempx << " y " << tempy << std::endl;
			GlobalObjects::LMFTracker->PositionTo(true, false, tempx, tempy, tempz);
			getIntegerParam(L_Count_3, &temptargets);
			if (temptargets != 0)
				GlobalObjects::LMFTracker->TargetSearch->Start();
		}
		catch (...) {
			// this happens if your moving the trager via the mouse, and there is a race condition between if it sees a tracker
			// that used to exist, and no longer exists, so attempts to find something that no longer exists

			;
		}
	}
	else if (function == L_IsOnValue) {
		GlobalObjects::LMFTracker->Laser->IsOn->Value = value;
	}
	//	else if (function == L_StartMeasurement) {
	//		if (value == 1) {
	//			GlobalObjects::LMFTracker->Measurement->StartMeasurement();
	//		}
	//		if (value == 0) {
	//			GlobalObjects::LMFTracker->Measurement->StopMeasurement();
	//		}
	//	}
	else if (function == L_StartMeasurement) {
		GlobalObjects::LMFTracker->Measurement->StartMeasurement();
	}
	else if (function == L_StopMeasurement) {
		GlobalObjects::LMFTracker->Measurement->StopMeasurement();
	}
	else if (function == leica_->L_MeasureStationary) {
		GlobalObjects::LMFTracker->Measurement->MeasureStationary();
	}

	else if ((function == L_Change) || (function == L_isFace1)) {
		GlobalObjects::LMFTracker->Face->Change();
	}
	// Top Level commands
	else if (function == L_GoHomePosition) {
		GlobalObjects::LMFTracker->GoHomePosition();
	}
	else if (function == L_OpenTrackerScope) {
		GlobalObjects::LMFTracker->OpenTrackerScope();
	}
	else if (function == L_ShutDown) {
		GlobalObjects::LMFTracker->ShutDown();
	}
	// no longer exist in latest sdk ???
	//	else if (function == L_Restart) {
	//		GlobalObjects::LMFTracker->Restart();
	//	}
	else if (function == L_StopMove) {
		GlobalObjects::LMFTracker->StopMove();
	}
	else if (function == L_Initialize) {
		GlobalObjects::LMFTracker->Initialize();
	}
	else if (function == L_PositionTo) {
		double tempx, tempy, tempz;
		int tempi, tempj;
		getDoubleParam(L_Pos1_2, &tempx);
		getDoubleParam(L_Pos2_2, &tempy);
		getDoubleParam(L_Pos3_2, &tempz);
		getIntegerParam(L_searchTarget, &tempi);
		getIntegerParam(L_isRelative_2, &tempj);
		GlobalObjects::LMFTracker->PositionTo(tempi, tempj, tempx, tempy, tempz);

	}
	// OverviewCamera related stuff
	else if (function == L_StartAsync) {
		GlobalObjects::LMFTracker->OverviewCamera->StartAsync();
	}
	else if (function == L_Stop) {
		GlobalObjects::LMFTracker->OverviewCamera->Stop();
	}

	// This info comes from a callback, and could be many targets seen . . . so which one?
	// Use the existing top level screen image thing, for now
	//	GlobalObjects::LMFTracker->OverviewCamera->MoveToPixel(x,y,width,height)

	else if (function == L_Close) {
		GlobalObjects::LMFTracker->OverviewCamera->Dialog->Close();
	}
	else if (function == L_Show) {
		GlobalObjects::LMFTracker->OverviewCamera->Dialog->Show();
	}
	else if (function == L_ShowDialog) {
		GlobalObjects::LMFTracker->OverviewCamera->Dialog->ShowDialog();
	}
	// needs a process id?
	//	else if (function == L_ShowOnProcess) {
	//	GlobalObjects::LMFTracker->OverviewCamera->Dialog->ShowOnProcess(id);
	//	}
	else if (function == L_ShowTopmost) {
		GlobalObjects::LMFTracker->OverviewCamera->Dialog->ShowTopmost();
	}
	// InclinationSensor commands

	else if (function == L_StartBubbleReadoutStream) {
		GlobalObjects::LMFTracker->InclinationSensor->BubbleReadout->StartBubbleReadoutStream();
	}
	else if (function == L_StopBubbleReadoutStream) {
		GlobalObjects::LMFTracker->InclinationSensor->BubbleReadout->StopBubbleReadoutStream();
	}
	else if (function == L_GetInclinationToGravity) {
		GlobalObjects::LMFTracker->InclinationSensor->GetInclinationToGravity();
	}
	else if (function == L_GetInclinationToGravityAsync) {
		GlobalObjects::LMFTracker->InclinationSensor->GetInclinationToGravityAsync();
	}
	else if (function == L_Value_1) {
		GlobalObjects::LMFTracker->InclinationSensor->InclinedToGravity->Value = value;
	}
	else if (function == L_Value_2) {
		GlobalObjects::LMFTracker->InclinationSensor->Monitoring->Active->Value = value;
	}
	// Change Measurement profiles
	else if (function == L_Select_6) { //stationary, high res
		GlobalObjects::LMFTracker->Measurement->StopMeasurement();
		LMF::Tracker::Measurements::MeasurementProfile^ profile = GlobalObjects::LMFTracker->Measurement->Profiles[0];
		profile->Select();
	}
	else if (function == L_Select_7) { //continious time profile
		GlobalObjects::LMFTracker->Measurement->StopMeasurement();
		LMF::Tracker::Measurements::MeasurementProfile^ profile = GlobalObjects::LMFTracker->Measurement->Profiles[1];
		profile->Select();
	}
	else if (function == L_Select_8) { //continious distance profile
		GlobalObjects::LMFTracker->Measurement->StopMeasurement();
		LMF::Tracker::Measurements::MeasurementProfile^ profile = GlobalObjects::LMFTracker->Measurement->Profiles[2];
		profile->Select();
	}
	else if (function == L_Select_9) { //custom trigger profile
		GlobalObjects::LMFTracker->Measurement->StopMeasurement();
		LMF::Tracker::Measurements::MeasurementProfile^ profile = GlobalObjects::LMFTracker->Measurement->Profiles[3];
		profile->Select();
	}

	else {
		/* All other parameters just get set in parameter list, no need to
		 * act on them here */
		status = asynPortDriver::writeInt32(pasynUser, value);
	}

	/* Do callbacks so higher layers see any changes */
	status = (asynStatus)callParamCallbacks();

	if (status)
		epicsSnprintf(pasynUser->errorMessage, pasynUser->errorMessageSize,
			"%s:%s: status=%d, function=%d, name=%s, value=%d",
			driverName, functionName, status, function, paramName, value);
	else
		asynPrint(pasynUser, ASYN_TRACEIO_DRIVER,
			"%s:%s: function=%d, name=%s, value=%d\n",
			driverName, functionName, function, paramName, value);

	return status;
}

asynStatus leica::readInt32(asynUser* pasynUser, epicsInt32* value)
{
	int addr;
	int function = pasynUser->reason;
	int status = 0;
	//	epicsInt32 temp;
	char* whoami;
	static const char* functionName = "readInt32";



	this->getAddress(pasynUser, &addr);
	getParamName(function, (const char**)&whoami);
	epicsInt32 temp;

	// what here is likely to change, that isn't updated via callbacks or other meams?
	/*
	The actual stuff down in the startup code could be here, but then I would have to deal with doing callbacks for things that do not as of yet exist
	*/

	//  if (function == L_iscompatibleFirmware) {
	//	getIntegerParam(L_iscompatibleFirmware, &temp); 
	//	printf("ReadInt32 values is  %d\n", temp);
	//    temp = temp +1;
	asynPortDriver::readInt32(pasynUser, &temp);
	*value = temp;
	//		asynPortDriver::writeInt32(pasynUser, temp);
	//	callParamCallbacks(addr);
	//   }

	  // Other functions we call the base class method
	//  else {

	status = asynPortDriver::readInt32(pasynUser, value);
	//	printf("in readInt32 . . . function %d %s value %d\n", function, whoami, *value);

		// }
	if (function > FIRST_LEICA_PARAM)
		printf("in readInt32 . . . function %d %s value %d\n", function, whoami, *value);

	callParamCallbacks(addr);


	return (status == 0) ? asynSuccess : asynError;
}



/** Report status of the driver.
  * Prints details about the driver if details>0.
  * It then calls the ADDriver::report() method.
  * \param[in] fp File pointed passed by caller where the output is written to.
  * \param[in] details If >0 then driver details are printed.
  */
void leica::report(FILE* fp, int details)
{

	fprintf(fp, "Simulation detector %s\n", this->portName);
	if (details > 0) {
		int nx, ny, dataType;
		getIntegerParam(ADSizeX, &nx);
		getIntegerParam(ADSizeY, &ny);
		getIntegerParam(NDDataType, &dataType);
		fprintf(fp, "  NX, NY:            %d  %d\n", nx, ny);
		fprintf(fp, "  Data type:         %d\n", dataType);
	}
	/* Invoke the base class method */
	ADDriver::report(fp, details);
}

asynStatus leica::writeOctet(asynUser* pasynUser, const char* value, size_t nChars, size_t* nActual)
{
	int addr = 0;
	int function = pasynUser->reason;
	asynStatus status = asynSuccess;
	const char* functionName = "writeOctet";

	char* whoami;
	getParamName(function, (const char**)&whoami);

	printf("in write. . . function %d %s value %s\n", function, whoami, *value);

	status = getAddress(pasynUser, &addr); if (status != asynSuccess) return(status);


	// Set the parameter in the parameter library.
	status = (asynStatus)setStringParam(addr, function, (char*)value);
	if (status != asynSuccess) return(status);

	// Do callbacks so higher layers see any changes
	callParamCallbacks(addr, addr);

	if (status) {
		epicsSnprintf(pasynUser->errorMessage, pasynUser->errorMessageSize,
			"%s:%s: status=%d, function=%d, value=%s",
			driverName, functionName, status, function, value);
	}
	else {
		asynPrint(pasynUser, ASYN_TRACEIO_DRIVER,
			"%s:%s: function=%d, value=%s\n",
			driverName, functionName, function, value);
	}
	*nActual = nChars;
	return status;
}

void leica::Do_Face(void)
{
	std::cout << std::endl;
	std::cout << "Face \n";

	GlobalObjects::LMFTracker->Face->Changed += gcnew LMF::Tracker::Face::ChangedHandler(&OnFaceChanged);

	GlobalObjects::LMFTracker->Face->ChangeFinished += gcnew LMF::Tracker::Face::ChangeFinishedHandler(&OnFaceChangeFinished);


	std::cout << " Face: isface1: " << TFS[GlobalObjects::LMFTracker->Face->IsFace1] << std::endl;
	std::cout << " Face: Value: " << EFaceStrings[(int)GlobalObjects::LMFTracker->Face->Value] << std::endl;

	// Just doing the follow as a value check here just to see if it works

		/*
		std::cout << "Face: Flip \n";
		LMFTracker->Face->Change();
		std::cout << "Face: isface1: " << LMFTracker->Face->IsFace1 << std::endl;
		std::cout << "Face: Value: " << (int)LMFTracker->Face->Value << std::endl;
		std::cout << "Face: UnFlip \n\n";
		LMFTracker->Face->Change();
	*/
	std::cout << std::endl;
}

void leica::Do_Laser(void)
{
	std::cout << std::endl;
	std::cout << "Laser\n";

	DateTime^ dt;

	GlobalObjects::LMFTracker->Laser->IsLaserWarmedUp->Changed += gcnew LMF::Tracker::BasicTypes::BoolValue::ReadOnlyBoolValue::ChangedEventHandler(&OnLaserChanged);


	GlobalObjects::LMFTracker->Laser->IsOn->Changed += gcnew LMF::Tracker::BasicTypes::BoolValue::ReadOnlyBoolValue::ChangedEventHandler(&OnLaserChanged);


	dt = GlobalObjects::LMFTracker->Laser->WakeUpTime;

	std::cout << " Wakeup Time: " << (decode)(dt->ToString("dddd, dd. MMMM yyyy HH:mm:ss.fff")) << std::endl;
	leica_->setStringParam(leica_->L_WakeUpTime, (decode)(dt->ToString("dddd, dd. MMMM yyyy HH:mm:ss.fff")));

	std::cout << " Is Laser Warmed Up: Label: " << (decode)(GlobalObjects::LMFTracker->Laser->IsLaserWarmedUp->Label) << " Value: " << TFS[GlobalObjects::LMFTracker->Laser->IsLaserWarmedUp->Value] << std::endl;
	leica_->setStringParam(leica_->L_Label_8, (decode)(GlobalObjects::LMFTracker->Laser->IsLaserWarmedUp->Label));
	leica_->setIntegerParam(leica_->L_IsWarmValue, GlobalObjects::LMFTracker->Laser->IsLaserWarmedUp->Value);

	std::cout << " Is on: " << (decode)(GlobalObjects::LMFTracker->Laser->IsOn->Label) << " Value : " << TFS[GlobalObjects::LMFTracker->Laser->IsOn->Value] << std::endl;
	leica_->setStringParam(leica_->L_Label_9, (decode)(GlobalObjects::LMFTracker->Laser->IsOn->Label));
	leica_->setIntegerParam(leica_->L_IsOnValue, GlobalObjects::LMFTracker->Laser->IsOn->Value);

	leica_->callParamCallbacks();

	//This code leaves the unit *on*, but turns off the actual laser until a certain time, right now 24 hours from now

/*
	DateTime newt;
	newt = DateTime::Now;
	newt = newt.AddDays(1);


	LMFTracker->Laser->GoToSleep(newt);
	std::cout << "Wakeup From Sleep Time: " << (decode)(newt.ToString("dddd, dd. MMMM yyyy HH:mm:ss.fff")) << std::endl;
*/

	std::cout << std::endl;
}


void leica::Do_MeteoStation(void)
{
	std::cout << std::endl;
	std::cout << "MeteoStation\n";

	GlobalObjects::LMFTracker->MeteoStation->EnvironmentalValuesChanged += gcnew LMF::Tracker::Meteo::MeteoStation::EnvironmentalValuesChangedEventHandler(&OnEnvironmentalValuesChanged);

	GlobalObjects::LMFTracker->MeteoStation->HardwareHumidity->Changed += gcnew LMF::Tracker::BasicTypes::DoubleValue::ReadOnlyDoubleValue::ChangedEventHandler(&OnMeteoChanged);

	GlobalObjects::LMFTracker->MeteoStation->HardwarePressure->Changed += gcnew LMF::Tracker::BasicTypes::DoubleValue::ReadOnlyDoubleValue::ChangedEventHandler(&OnMeteoChanged);
	GlobalObjects::LMFTracker->MeteoStation->HardwareTemperature->Changed += gcnew LMF::Tracker::BasicTypes::DoubleValue::ReadOnlyDoubleValue::ChangedEventHandler(&OnMeteoChanged);
	GlobalObjects::LMFTracker->MeteoStation->IsAirSensorConnected->Changed += gcnew LMF::Tracker::BasicTypes::BoolValue::ReadOnlyBoolValue::ChangedEventHandler(&OnMeteoChanged);

	GlobalObjects::LMFTracker->MeteoStation->ManualHumidity->Changed += gcnew LMF::Tracker::BasicTypes::DoubleValue::ReadOnlyDoubleValue::ChangedEventHandler(&OnMeteoChanged);
	GlobalObjects::LMFTracker->MeteoStation->ManualPressure->Changed += gcnew LMF::Tracker::BasicTypes::DoubleValue::ReadOnlyDoubleValue::ChangedEventHandler(&OnMeteoChanged);
	GlobalObjects::LMFTracker->MeteoStation->ManualTemperature->Changed += gcnew LMF::Tracker::BasicTypes::DoubleValue::ReadOnlyDoubleValue::ChangedEventHandler(&OnMeteoChanged);
	GlobalObjects::LMFTracker->MeteoStation->ObjectTemperature->Changed += gcnew LMF::Tracker::BasicTypes::DoubleValue::ReadOnlyDoubleValue::ChangedEventHandler(&OnMeteoChanged);
	GlobalObjects::LMFTracker->MeteoStation->Source->Changed += gcnew LMF::Tracker::Meteo::MeteoSource::ChangedEventHandler(&OnMeteoChanged);

	/*	std::cout << "HardwareHumidity: Label: " << (decode)(GlobalObjects::LMFTracker->MeteoStation->HardwareHumidity->Label) <<
			" SerialNumber: " << (decode)(GlobalObjects::LMFTracker->MeteoStation->HardwareHumidity->SerialNumber) <<
			" UnitString: " << (decode)(GlobalObjects::LMFTracker->MeteoStation->HardwareHumidity->UnitString) <<
			" UnitType: " << EUnitTypeStrings[(int)GlobalObjects::LMFTracker->MeteoStation->HardwareHumidity->UnitType] <<
			" Value: " << GlobalObjects::LMFTracker->MeteoStation->HardwareHumidity->Value <<
			" ValueInBaseUnits: " << GlobalObjects::LMFTracker->MeteoStation->HardwareHumidity->ValueInBaseUnits <<
			std::endl;
	*/

	//Can not do these in the macro since not all serialNumbers have a _##
	// value is actually a named variable, since this code was done before I generalized how names would be generated
	// changing these now would cause all #'s to shift, and not doing that 2 'sections' from being done
	// so do the ones that I can, and just manually do the 2 lines afterwards as required

	//leica_->setStringParam(leica_->L_SerialNumber_##a, (decode)(##thing->Label)); 
	//leica_->setDoubleParam(leica_->L_Value_##d, ##thing->Value); 

#define PV_IT_4(a,b,c,d,thing) \
leica_->setStringParam(leica_->L_Label_##a, (decode)(##thing->Label)); \
leica_->setStringParam(leica_->L_UnitString_##b, (decode)(##thing->UnitString)); \
leica_->setStringParam(leica_->L_UnitType_##b, EUnitTypeStrings[(int)##thing->UnitType]); \
leica_->setDoubleParam(leica_->L_ValueInBaseUnits_##b, ##thing->ValueInBaseUnits); \
leica_->setStringParam(leica_->L_Label_##c, (decode)(##thing->Available->Label)); \
leica_->setIntegerParam(leica_->L_Value_##d, ##thing->Available->Value); 


	Do_EnvironmentalSensor(" HardwareHumidity", GlobalObjects::LMFTracker->MeteoStation->HardwareHumidity);

	PV_IT_4(23, 9, 24, 22, GlobalObjects::LMFTracker->MeteoStation->HardwareHumidity)

		leica_->setStringParam(leica_->L_SerialNumber, (decode)(GlobalObjects::LMFTracker->MeteoStation->HardwareHumidity->SerialNumber));
	leica_->setDoubleParam(leica_->L_HumidityValue, GlobalObjects::LMFTracker->MeteoStation->HardwareHumidity->Value);


	GlobalObjects::LMFTracker->MeteoStation->HardwareHumidity->Available->Changed += gcnew LMF::Tracker::BasicTypes::BoolValue::ReadOnlyBoolValue::ChangedEventHandler(&OnMeteoChanged);

	/*	std::cout << "Available: " << (decode)(GlobalObjects::LMFTracker->MeteoStation->HardwareHumidity->Available->Label) <<
			" Value: " << TFS[(int)GlobalObjects::LMFTracker->MeteoStation->HardwareHumidity->Available->Value] <<
			std::endl;
	*/

	/*	std::cout << "HardwarePressure: Label: " << (decode)(GlobalObjects::LMFTracker->MeteoStation->HardwarePressure->Label) <<
			" SerialNumber: " << (decode)(GlobalObjects::LMFTracker->MeteoStation->HardwarePressure->SerialNumber) <<
			" UnitString: " << (decode)(GlobalObjects::LMFTracker->MeteoStation->HardwarePressure->UnitString) <<
			" UnitType: " << EUnitTypeStrings[(int)GlobalObjects::LMFTracker->MeteoStation->HardwarePressure->UnitType] <<
			" Value: " << GlobalObjects::LMFTracker->MeteoStation->HardwarePressure->Value <<
			" ValueInBaseUnits: " << GlobalObjects::LMFTracker->MeteoStation->HardwarePressure->ValueInBaseUnits <<
			std::endl;
	*/

	Do_EnvironmentalSensor(" HardwarePressure", GlobalObjects::LMFTracker->MeteoStation->HardwarePressure);

	PV_IT_4(25, 10, 26, 23, GlobalObjects::LMFTracker->MeteoStation->HardwarePressure)

		leica_->setStringParam(leica_->L_SerialNumber_1, (decode)(GlobalObjects::LMFTracker->MeteoStation->HardwarePressure->SerialNumber));
	leica_->setDoubleParam(leica_->L_PressureValue, GlobalObjects::LMFTracker->MeteoStation->HardwarePressure->Value);


	GlobalObjects::LMFTracker->MeteoStation->HardwarePressure->Available->Changed += gcnew LMF::Tracker::BasicTypes::BoolValue::ReadOnlyBoolValue::ChangedEventHandler(&OnMeteoChanged);
	/*
		std::cout << "Available: " << (decode)(GlobalObjects::LMFTracker->MeteoStation->HardwarePressure->Available->Label) <<
			" Value: " << TFS[(int)GlobalObjects::LMFTracker->MeteoStation->HardwarePressure->Available->Value] <<
			std::endl;
	*/

	/*
	std::cout << "HardwareTemperature: Label: " << (decode)(GlobalObjects::LMFTracker->MeteoStation->HardwareTemperature->Label) <<
		" SerialNumber: " << (decode)(GlobalObjects::LMFTracker->MeteoStation->HardwareTemperature->SerialNumber) <<
		// The degree symbol from this device is something wierd, so pop it off and replace it with something correct.
		" UnitString: " << '\370' << (decode)(GlobalObjects::LMFTracker->MeteoStation->HardwareTemperature->UnitString->Substring(1)) <<
		" UnitType: " << EUnitTypeStrings[(int)GlobalObjects::LMFTracker->MeteoStation->HardwareTemperature->UnitType] <<
		" Value: " << GlobalObjects::LMFTracker->MeteoStation->HardwareTemperature->Value <<
		" ValueInBaseUnits: " << GlobalObjects::LMFTracker->MeteoStation->HardwareTemperature->ValueInBaseUnits <<
		std::endl;
*/
	Do_EnvironmentalSensor(" HardwareTemperature", GlobalObjects::LMFTracker->MeteoStation->HardwareTemperature);

	PV_IT_4(27, 11, 28, 24, GlobalObjects::LMFTracker->MeteoStation->HardwareTemperature)

		leica_->setStringParam(leica_->L_SerialNumber_2, (decode)(GlobalObjects::LMFTracker->MeteoStation->HardwareTemperature->SerialNumber));
	leica_->setDoubleParam(leica_->L_TemperatureValue, GlobalObjects::LMFTracker->MeteoStation->HardwareTemperature->Value);
	// note the degree symbol is incorrect, manually patch it, caqtdm and phoebus 'see' different symbols

	leica_->setStringParam(leica_->L_UnitString_11, (decode)(GlobalObjects::LMFTracker->MeteoStation->HardwareTemperature->UnitString->Substring(1)));

	GlobalObjects::LMFTracker->MeteoStation->HardwareTemperature->Available->Changed += gcnew LMF::Tracker::BasicTypes::BoolValue::ReadOnlyBoolValue::ChangedEventHandler(&OnMeteoChanged);

	/*
	std::cout << "Available: " << (decode)(GlobalObjects::LMFTracker->MeteoStation->HardwareTemperature->Available->Label) <<
		" Value: " << TFS[(int)GlobalObjects::LMFTracker->MeteoStation->HardwareTemperature->Available->Value] <<
		std::endl;
*/

/*
	GlobalObjects::LMFTracker->MeteoStation->IsAirSensorConnected->Changed += gcnew LMF::Tracker::BasicTypes::BoolValue::ReadOnlyBoolValue::ChangedEventHandler(&OnMeteoChanged);
	std::cout << "IsAirSensorConnected: Label: " << (decode)(GlobalObjects::LMFTracker->MeteoStation->IsAirSensorConnected->Label) <<
		" Value: " << TFS[(int)GlobalObjects::LMFTracker->MeteoStation->IsAirSensorConnected->Value] <<
		std::endl;
*/

	Do_BoolValue(" IsAirSensorConnected", GlobalObjects::LMFTracker->MeteoStation->IsAirSensorConnected);
	leica_->setStringParam(leica_->L_Label_29, (decode)(GlobalObjects::LMFTracker->MeteoStation->IsAirSensorConnected->Label));
	leica_->setIntegerParam(leica_->L_Value_25, (int)GlobalObjects::LMFTracker->MeteoStation->IsAirSensorConnected->Value);

	//
/*
	std::cout << "ManualHumidity: Label: " << (decode)(GlobalObjects::LMFTracker->MeteoStation->ManualHumidity->Label) <<

		// extra fields on manual readouts

		" MaxValue: " << GlobalObjects::LMFTracker->MeteoStation->ManualHumidity->MaxValue <<
		" MaxValueInBaseUnits: " << GlobalObjects::LMFTracker->MeteoStation->ManualHumidity->MaxValueInBaseUnits <<
		" MinValue: " << GlobalObjects::LMFTracker->MeteoStation->ManualHumidity->MinValue <<
		" MinValueInBaseUnits: " << GlobalObjects::LMFTracker->MeteoStation->ManualHumidity->MinValueInBaseUnits <<


		" SerialNumber: " << (decode)(GlobalObjects::LMFTracker->MeteoStation->ManualHumidity->SerialNumber) <<
		" UnitString: " << (decode)(GlobalObjects::LMFTracker->MeteoStation->ManualHumidity->UnitString) <<
		" UnitType: " << EUnitTypeStrings[(int)GlobalObjects::LMFTracker->MeteoStation->ManualHumidity->UnitType] <<
		" Value: " << GlobalObjects::LMFTracker->MeteoStation->ManualHumidity->Value <<
		" ValueInBaseUnits: " << GlobalObjects::LMFTracker->MeteoStation->ManualHumidity->ValueInBaseUnits <<
		std::endl;
*/
/*
#define PV_IT_5(a,b,c,d,thing) \
leica_->setStringParam(leica_->L_Label_##a, (decode)(##thing->Label)); \
leica_->setDoubleParam(leica_->L_MaxValue_##b, ##thing->MaxValue); \
leica_->setDoubleParam(leica_->L_MaxValueInBaseUnits_##b, ##thing->MaxValueInBaseUnits); \
leica_->setDoubleParam(leica_->L_MinValue_##b, ##thing->MinValue); \
leica_->setDoubleParam(leica_->L_MinValueInBaseUnits_##b, ##thing->MinValueInBaseUnits); \
leica_->setStringParam(leica_->L_SerialNumber_##c, (decode)(##thing->SerialNumber)); \
leica_->setStringParam(leica_->L_UnitString_##d, (decode)(##thing->UnitString)); \
leica_->setStringParam(leica_->L_UnitType_##d, EUnitTypeStrings[(int)##thing->UnitType]); \
leica_->setDoubleParam(leica_->L_ValueInBaseUnits_##d, ##thing->ValueInBaseUnits); \

#define PV_IT_5a(a,b,c,d,thing) \
leica_->setStringParam(leica_->L_Label_##a, (decode)(##thing->Label)); \
leica_->setDoubleParam(leica_->L_MaxValue_##b, ##thing->MaxValue); \
leica_->setDoubleParam(leica_->L_MaxValueInBaseUnits_##b, ##thing->MaxValueInBaseUnits); \
leica_->setDoubleParam(leica_->L_MinValue_##b, ##thing->MinValue); \
leica_->setDoubleParam(leica_->L_MinValueInBaseUnits_##b, ##thing->MinValueInBaseUnits); \
leica_->setStringParam(leica_->L_UnitString_##c, (decode)(##thing->UnitString)); \
leica_->setStringParam(leica_->L_UnitType_##c, EUnitTypeStrings[(int)##thing->UnitType]); \
leica_->setDoubleParam(leica_->L_Value_##d, ##thing->Value); \
leica_->setDoubleParam(leica_->L_ValueInBaseUnits_##c, ##thing->ValueInBaseUnits); \
*/


	Do_ManualEnvironmentalSensor(" ManualHumidity", GlobalObjects::LMFTracker->MeteoStation->ManualHumidity);

	PV_IT_5(30, 5, 3, 12, GlobalObjects::LMFTracker->MeteoStation->ManualHumidity)
		leica_->setDoubleParam(leica_->L_MHumidityValue, GlobalObjects::LMFTracker->MeteoStation->ManualHumidity->Value);

	GlobalObjects::LMFTracker->MeteoStation->ManualHumidity->Available->Changed += gcnew LMF::Tracker::BasicTypes::BoolValue::ReadOnlyBoolValue::ChangedEventHandler(&OnMeteoChanged);

	/*
	std::cout << "Available: " << (decode)(GlobalObjects::LMFTracker->MeteoStation->ManualHumidity->Available->Label) <<
			" Value: " << TFS[(int)GlobalObjects::LMFTracker->MeteoStation->ManualHumidity->Available->Value] <<
			std::endl;
	*/

	/*
		std::cout << "ManualPressure: Label: " << (decode)(GlobalObjects::LMFTracker->MeteoStation->ManualPressure->Label) <<

			// extra fields on manual readouts

			" MaxValue: " << GlobalObjects::LMFTracker->MeteoStation->ManualPressure->MaxValue <<
			" MaxValueInBaseUnits: " << GlobalObjects::LMFTracker->MeteoStation->ManualPressure->MaxValueInBaseUnits <<
			" MinValue: " << GlobalObjects::LMFTracker->MeteoStation->ManualPressure->MinValue <<
			" MinValueInBaseUnits: " << GlobalObjects::LMFTracker->MeteoStation->ManualPressure->MinValueInBaseUnits <<

			" SerialNumber: " << (decode)(GlobalObjects::LMFTracker->MeteoStation->ManualPressure->SerialNumber) <<
			" UnitString: " << (decode)(GlobalObjects::LMFTracker->MeteoStation->ManualPressure->UnitString) <<
			" UnitType: " << EUnitTypeStrings[(int)GlobalObjects::LMFTracker->MeteoStation->ManualPressure->UnitType] <<
			" Value: " << GlobalObjects::LMFTracker->MeteoStation->ManualPressure->Value <<
			" ValueInBaseUnits: " << GlobalObjects::LMFTracker->MeteoStation->ManualPressure->ValueInBaseUnits <<
			std::endl;
	*/

	Do_ManualEnvironmentalSensor(" ManualPressure", GlobalObjects::LMFTracker->MeteoStation->ManualPressure);

	PV_IT_5(31, 6, 4, 13, GlobalObjects::LMFTracker->MeteoStation->ManualPressure)

		leica_->setDoubleParam(leica_->L_MPressureValue, GlobalObjects::LMFTracker->MeteoStation->ManualPressure->Value);

	/*
	GlobalObjects::LMFTracker->MeteoStation->ManualPressure->Available->Changed += gcnew LMF::Tracker::BasicTypes::BoolValue::ReadOnlyBoolValue::ChangedEventHandler(&OnMeteoChanged);

		std::cout << "Available: " << (decode)(GlobalObjects::LMFTracker->MeteoStation->ManualPressure->Available->Label) <<
			" Value: " << TFS[(int)GlobalObjects::LMFTracker->MeteoStation->ManualPressure->Available->Value] <<
			std::endl;
	*/
	/*
		std::cout << "ManualTemperature: Label: " << (decode)(GlobalObjects::LMFTracker->MeteoStation->ManualTemperature->Label) <<

			// extra fields on manual readouts

			" MaxValue: " << GlobalObjects::LMFTracker->MeteoStation->ManualTemperature->MaxValue <<
			" MaxValueInBaseUnits: " << GlobalObjects::LMFTracker->MeteoStation->ManualTemperature->MaxValueInBaseUnits <<
			" MinValue: " << GlobalObjects::LMFTracker->MeteoStation->ManualTemperature->MinValue <<
			" MinValueInBaseUnits: " << GlobalObjects::LMFTracker->MeteoStation->ManualTemperature->MinValueInBaseUnits <<


			" SerialNumber: " << (decode)(GlobalObjects::LMFTracker->MeteoStation->ManualTemperature->SerialNumber) <<
			// The degree symbol from this device is something wierd, so pop it off and replace it with something correct.
			" UnitString: " << '\370' << (decode)(GlobalObjects::LMFTracker->MeteoStation->ManualTemperature->UnitString->Substring(1)) <<
			//		" UnitString: " << (decode)(LMFTracker->MeteoStation->ManualTemperature->UnitString) <<
			" UnitType: " << EUnitTypeStrings[(int)GlobalObjects::LMFTracker->MeteoStation->ManualTemperature->UnitType] <<
			" Value: " << GlobalObjects::LMFTracker->MeteoStation->ManualTemperature->Value <<
			" ValueInBaseUnits: " << GlobalObjects::LMFTracker->MeteoStation->ManualTemperature->ValueInBaseUnits <<
			std::endl;
	*/

	Do_ManualEnvironmentalSensor(" ManualTemperature", GlobalObjects::LMFTracker->MeteoStation->ManualTemperature);

	PV_IT_5(32, 7, 5, 14, GlobalObjects::LMFTracker->MeteoStation->ManualTemperature)
		leica_->setStringParam(leica_->L_UnitString_14, (decode)(GlobalObjects::LMFTracker->MeteoStation->ManualTemperature->UnitString->Substring(1)));
	leica_->setDoubleParam(leica_->L_MTemperatureValue, GlobalObjects::LMFTracker->MeteoStation->ManualTemperature->Value);

	GlobalObjects::LMFTracker->MeteoStation->ManualTemperature->Available->Changed += gcnew LMF::Tracker::BasicTypes::BoolValue::ReadOnlyBoolValue::ChangedEventHandler(&OnMeteoChanged);

	/*
		std::cout << "Available: " << (decode)(GlobalObjects::LMFTracker->MeteoStation->ManualTemperature->Available->Label) <<
			" Value: " << TFS[(int)GlobalObjects::LMFTracker->MeteoStation->ManualTemperature->Available->Value] <<
			std::endl;
	*/

	/*
		std::cout << "ObjectTemperature: Label: " << (decode)(GlobalObjects::LMFTracker->MeteoStation->ObjectTemperature->Label) <<
			" SerialNumber: " << (decode)(GlobalObjects::LMFTracker->MeteoStation->ObjectTemperature->SerialNumber) <<
			// The degree symbol from this device is something wierd, so pop it off and replace it with something correct.
			" UnitString: " << '\370' << (decode)(GlobalObjects::LMFTracker->MeteoStation->ObjectTemperature->UnitString->Substring(1)) <<
			//		" UnitString: " << (decode)(LMFTracker->MeteoStation->ObjectTemperature->UnitString) <<
			" UnitType: " << EUnitTypeStrings[(int)GlobalObjects::LMFTracker->MeteoStation->ObjectTemperature->UnitType] <<
			" Value: " << GlobalObjects::LMFTracker->MeteoStation->ObjectTemperature->Value <<
			" ValueInBaseUnits: " << GlobalObjects::LMFTracker->MeteoStation->ObjectTemperature->ValueInBaseUnits <<
			std::endl;
	*/

	Do_EnvironmentalSensor(" ObjectTemperature", GlobalObjects::LMFTracker->MeteoStation->ObjectTemperature);

	PV_IT_4(33, 15, 34, 26, GlobalObjects::LMFTracker->MeteoStation->ObjectTemperature)

		leica_->setStringParam(leica_->L_SerialNumber_6, (decode)(GlobalObjects::LMFTracker->MeteoStation->ObjectTemperature->SerialNumber));
	leica_->setDoubleParam(leica_->L_OTemperatureValue, GlobalObjects::LMFTracker->MeteoStation->ObjectTemperature->Value);
	leica_->setStringParam(leica_->L_UnitString_15, (decode)(GlobalObjects::LMFTracker->MeteoStation->ObjectTemperature->UnitString->Substring(1)));



	GlobalObjects::LMFTracker->MeteoStation->ObjectTemperature->Available->Changed += gcnew LMF::Tracker::BasicTypes::BoolValue::ReadOnlyBoolValue::ChangedEventHandler(&OnMeteoChanged);

	/*
	std::cout << "Available: " << (decode)(GlobalObjects::LMFTracker->MeteoStation->ObjectTemperature->Available->Label) <<
			" Value: " << TFS[(int)GlobalObjects::LMFTracker->MeteoStation->ObjectTemperature->Available->Value] <<
			std::endl;
	*/

	GlobalObjects::LMFTracker->MeteoStation->Source->Changed += gcnew LMF::Tracker::Meteo::MeteoSource::ChangedEventHandler(&OnMeteoChanged);

	std::cout << " Source: Label: " << (decode)(GlobalObjects::LMFTracker->MeteoStation->Source->Label) <<
		" Value: " << EMeteoSourceStrings[(int)GlobalObjects::LMFTracker->MeteoStation->Source->Value] <<
		std::endl;


	leica_->setStringParam(leica_->L_Label_35, (decode)(GlobalObjects::LMFTracker->MeteoStation->Source->Label));
	leica_->setIntegerParam(leica_->L_Value_27, (int)GlobalObjects::LMFTracker->MeteoStation->Source->Value);
	// This is actually a 2 state sting and not just true/false or 0/1

	std::cout << std::endl;

	leica_->callParamCallbacks();

}

void leica::Do_Measurement(void)
{
	std::cout << std::endl;
	std::cout << "Measurement\n";

	GlobalObjects::LMFTracker->Measurement->MeasurementArrived += gcnew LMF::Tracker::Measurements::MeasurementSettings::MeasurementArrivedHandler(&OnMeasurementArrived);

	// Methods
	/*
		LMFTracker->Measurement->GoAndMeasureStationary();
		LMFTracker->Measurement->GoAndMeasureStationaryAsync();
		LMFTracker->Measurement->MeasureStationary()
		LMFTracker->Measurement->StartMeasurement();
		LMFTracker->Measurement->StopMeasurement();
	*/
	GlobalObjects::LMFTracker->Measurement->MeasurementInProgress->Changed += gcnew LMF::Tracker::BasicTypes::BoolValue::ReadOnlyBoolValue::ChangedEventHandler(&OnMeasChanged);
	//		+= gcnew LMF::Tracker::BasicTypes::BoolValue::ReadOnlyBoolValue::ChangedEventHandler(&OnChanged);

	Do_BoolValue("MeasurementInProgress", GlobalObjects::LMFTracker->Measurement->MeasurementInProgress);
	std::cout << "Profiles: Count: " << GlobalObjects::LMFTracker->Measurement->Profiles->Count << std::endl;

	leica_->setStringParam(leica_->L_Label_10, (decode)(GlobalObjects::LMFTracker->Measurement->MeasurementInProgress->Label));
	leica_->setIntegerParam(leica_->L_Value_9, (int)GlobalObjects::LMFTracker->Measurement->MeasurementInProgress->Value);
	leica_->setIntegerParam(leica_->L_Count_1, GlobalObjects::LMFTracker->Measurement->Profiles->Count);

	//	std::cout << "Try Quick Measurement . . . " << std::endl;

	//	LMFTracker->Measurement->StartMeasurement();
	//	Sleep(20);
	//	LMFTracker->Measurement->StopMeasurement();

		/* This is actually important,

			The first option is basically a one-shot
			The second is a periodic one, so potentially a scan forevery.
		*/


	for (int i = 0; i < GlobalObjects::LMFTracker->Measurement->Profiles->Count; i++)
	{
		std::cout << std::endl;
		LMF::Tracker::Measurements::MeasurementProfile^ profile = GlobalObjects::LMFTracker->Measurement->Profiles[i];

		if (LMF::Tracker::Measurements::Profiles::StationaryMeasurementProfile^ thisProfile = dynamic_cast<LMF::Tracker::Measurements::Profiles::StationaryMeasurementProfile^>(profile))
		{
			thisProfile->Accuracy->Changed += gcnew LMF::Tracker::BasicTypes::EnumTypes::AccuracyValue::ChangedEventHandler(&OnMeasChanged);


			std::cout << "StationaryMeasurementProfile" << std::endl;
			std::cout << "GUID: " << (decode)(profile->GUID) << std::endl;
			std::cout << "Name: " << (decode)(profile->Name) << std::endl;

			leica_->setStringParam(leica_->L_GUID_6, (decode)(profile->GUID));
			leica_->setStringParam(leica_->L_Name_6, (decode)(profile->Name));

			const char* EAccuracyStrings[] = { "Precise", "Standard", "Fast" }; //ok

			thisProfile->Accuracy->Value = LMF::Tracker::Enums::EAccuracy::Precise;

			std::cout << "Accuracy: Label: " << (decode)(thisProfile->Accuracy->Label) <<
				" Value: " << EAccuracyStrings[(int)thisProfile->Accuracy->Value] <<
				std::endl;

			leica_->setStringParam(leica_->L_Label_11, (decode)(thisProfile->Accuracy->Label));
			// need to fix what these units actually are
			leica_->setIntegerParam(leica_->L_Value_10, (int)thisProfile->Accuracy->Value);

			Do_BoolValue("TwoFace", thisProfile->TwoFace);

			leica_->setStringParam(leica_->L_Label_12, (decode)(thisProfile->TwoFace->Label));
			leica_->setIntegerParam(leica_->L_Value_11, (int)thisProfile->TwoFace->Value);

			//methods
			//	thisProfile->Select();
			//thisProfile->Select();
		}
		else if (LMF::Tracker::Measurements::Profiles::ContinuousTimeProfile^ thisProfile = dynamic_cast<LMF::Tracker::Measurements::Profiles::ContinuousTimeProfile^>(profile))
		{
			std::cout << "ContinuousTimeProfile" << std::endl;
			std::cout << "GUID: " << (decode)(profile->GUID) << std::endl;
			std::cout << "Name: " << (decode)(profile->Name) << std::endl;

			leica_->setStringParam(leica_->L_GUID_7, (decode)(profile->GUID));
			leica_->setStringParam(leica_->L_Name_7, (decode)(profile->Name));


			// I want this as the default startup value

			thisProfile->TimeSeparation->Value = 1000;
			thisProfile->TimeSeparation->ValueInBaseUnits = 1000;

			//methods and startup readoutmode
			thisProfile->Select();

			Do_IntValueWithRange("PacketRate", thisProfile->PacketRate);



#define PV_IT_6a(a,b,c,thing) \
leica_->setStringParam(leica_->L_Label_##a, (decode)(##thing->Label)); \
leica_->setIntegerParam(leica_->L_MaxValue, ##thing->MaxValue); \
leica_->setIntegerParam(leica_->L_MaxValueInBaseUnits, ##thing->MaxValueInBaseUnits); \
leica_->setIntegerParam(leica_->L_MinValue, ##thing->MinValue); \
leica_->setIntegerParam(leica_->L_MinValueInBaseUnits, ##thing->MinValueInBaseUnits); \
leica_->setStringParam(leica_->L_UnitString_##b, (decode)(##thing->UnitString)); \
leica_->setStringParam(leica_->L_UnitType_##b, EUnitTypeStrings[(int)##thing->UnitType]); \
leica_->setIntegerParam(leica_->L_Value_##c, ##thing->Value); \
leica_->setIntegerParam(leica_->L_ValueInBaseUnits_##b, ##thing->ValueInBaseUnits); \

			PV_IT_6a(13, 4, 12, thisProfile->PacketRate)


				Do_DoubleValueWithRange("TimeSeparation", thisProfile->TimeSeparation);

			PV_IT_2(14, 1, 5, 13, thisProfile->TimeSeparation)

				//methods
						//	thisProfile->Select();
		}
		else if (LMF::Tracker::Measurements::Profiles::ContinuousDistanceProfile^ thisProfile = dynamic_cast<LMF::Tracker::Measurements::Profiles::ContinuousDistanceProfile^>(profile))
		{
			std::cout << "ContinuousDistanceProfile" << std::endl;
			std::cout << "GUID: " << (decode)(profile->GUID) << std::endl;
			std::cout << "Name: " << (decode)(profile->Name) << std::endl;

			leica_->setStringParam(leica_->L_GUID_8, (decode)(profile->GUID));
			leica_->setStringParam(leica_->L_Name_8, (decode)(profile->Name));



			Do_DoubleValueWithRange("DistanceSeparation", thisProfile->DistanceSeparation);

			PV_IT_2(15, 2, 6, 14, thisProfile->DistanceSeparation)

		}
		else if (LMF::Tracker::Measurements::Profiles::CustomTriggerProfile^ thisProfile = dynamic_cast<LMF::Tracker::Measurements::Profiles::CustomTriggerProfile^>(profile))
		{
			thisProfile->TriggeredMeasurementsArrived += gcnew LMF::Tracker::Measurements::Profiles::CustomTriggerProfile::TriggeredMeasurementsArrivedHandler(&OnTriggeredMeasurementsArrived);
			//				+= gcnew LMF::Tracker::Measurements::Profiles::CustomTriggerProfile::TriggeredMeasurementsArrivedHandler(&OnTriggeredMeasurementsArrived);

			thisProfile->ClockSource->Changed += gcnew LMF::Tracker::BasicTypes::EnumTypes::ClockSourceValue::ChangedEventHandler(&OnMeasChanged);
			//+= gcnew LMF::Tracker::BasicTypes::EnumTypes::ClockSourceValue::ChangedEventHandler(&OnChanged);
			thisProfile->ClockTransmission->Changed += gcnew LMF::Tracker::BasicTypes::EnumTypes::ClockTransmissionValue::ChangedEventHandler(&OnMeasChanged);
			//+= gcnew LMF::Tracker::BasicTypes::EnumTypes::ClockTransmissionValue::ChangedEventHandler(&OnMeasChanged);
			thisProfile->StartStopActiveLevel->Changed += gcnew LMF::Tracker::BasicTypes::EnumTypes::StartStopActiveLevelValue::ChangedEventHandler(&OnMeasChanged);
			//+= gcnew LMF::Tracker::BasicTypes::EnumTypes::StartStopActiveLevelValue::ChangedEventHandler(&OnMeasChanged);
			thisProfile->StartStopSource->Changed += gcnew LMF::Tracker::BasicTypes::EnumTypes::StartStopSourceValue::ChangedEventHandler(&OnMeasChanged);
			//+= gcnew LMF::Tracker::BasicTypes::EnumTypes::StartStopSourceValue::ChangedEventHandler(&OnMeasChanged);

			std::cout << "CustomTriggerProfile" << std::endl;
			std::cout << "GUID: " << (decode)(profile->GUID) << std::endl;
			std::cout << "Name: " << (decode)(profile->Name) << std::endl;

			leica_->setStringParam(leica_->L_GUID_9, (decode)(profile->GUID));
			leica_->setStringParam(leica_->L_Name_9, (decode)(profile->Name));


			const char* EClockSourceStrings[] = { "Internal", "External" }; //ok

			std::cout << "ClockSource: Label: " << (decode)(thisProfile->ClockSource->Label) <<
				" Value: " << EClockSourceStrings[(int)thisProfile->ClockSource->Value] <<
				std::endl;

			leica_->setStringParam(leica_->L_Label_16, (decode)(thisProfile->ClockSource->Label));
			leica_->setIntegerParam(leica_->L_Value_15, (int)thisProfile->ClockSource->Value);


			const char* EClockTransmissionStrings[] = { "Negative", "Positve" }; //ok
			std::cout << "ClockTransmission: Label: " << (decode)(thisProfile->ClockTransmission->Label) <<
				" Value: " << EClockTransmissionStrings[(int)thisProfile->ClockTransmission->Value] <<
				std::endl;

			leica_->setStringParam(leica_->L_Label_17, (decode)(thisProfile->ClockTransmission->Label));
			leica_->setIntegerParam(leica_->L_Value_16, (int)thisProfile->ClockTransmission->Value);

			Do_DoubleValueWithRange("MinimalTimeDelay", thisProfile->MinimalTimeDelay);
			Do_IntValueWithRange("PacketRate", thisProfile->PacketRate);

			PV_IT_2(18, 3, 7, 17, thisProfile->MinimalTimeDelay)


#define PV_IT_6(a,b,c,d,thing) \
leica_->setStringParam(leica_->L_Label_##a, (decode)(##thing->Label)); \
leica_->setIntegerParam(leica_->L_MaxValue_##b, ##thing->MaxValue); \
leica_->setIntegerParam(leica_->L_MaxValueInBaseUnits_##b, ##thing->MaxValueInBaseUnits); \
leica_->setIntegerParam(leica_->L_MinValue_##b, ##thing->MinValue); \
leica_->setIntegerParam(leica_->L_MinValueInBaseUnits_##b, ##thing->MinValueInBaseUnits); \
leica_->setStringParam(leica_->L_UnitString_##c, (decode)(##thing->UnitString)); \
leica_->setStringParam(leica_->L_UnitType_##c, EUnitTypeStrings[(int)##thing->UnitType]); \
leica_->setIntegerParam(leica_->L_Value_##d, ##thing->Value); \
leica_->setIntegerParam(leica_->L_ValueInBaseUnits_##c, ##thing->ValueInBaseUnits); 


				PV_IT_6(19, 4, 8, 18, thisProfile->PacketRate)

				const char* EStartStopActiveLevelStrings[] = { "Low", "High" }; //ok
			std::cout << "StartStopActiveLevel: Label: " << (decode)(thisProfile->StartStopActiveLevel->Label) <<
				" Value: " << EStartStopActiveLevelStrings[(int)thisProfile->StartStopActiveLevel->Value] <<
				std::endl;

			leica_->setStringParam(leica_->L_Label_20, (decode)(thisProfile->StartStopActiveLevel->Label));
			leica_->setIntegerParam(leica_->L_Value_19, (int)thisProfile->StartStopActiveLevel->Value);

			const char* EStartStopSourceStrings[] = { "Ignored", "Active" }; //ok
			std::cout << "StartStopSource: Label: " << (decode)(thisProfile->StartStopSource->Label) <<
				" Value: " << EStartStopSourceStrings[(int)thisProfile->StartStopSource->Value] <<
				std::endl;

			leica_->setStringParam(leica_->L_Label_21, (decode)(thisProfile->StartStopSource->Label));
			leica_->setIntegerParam(leica_->L_Value_20, (int)thisProfile->StartStopSource->Value);

			//methods
					 //	thisProfile->Select();
//			leica_->setIntegerParam(leica_->L_Select_9, 1);
		}
	}
	// and the same thing for whatever the selected thing actually is


	std::cout << std::endl << "Selected" << std::endl;
	std::cout << "GUID: " << (decode)(GlobalObjects::LMFTracker->Measurement->Profiles->Selected->GUID) << std::endl;
	std::cout << "Name: " << (decode)(GlobalObjects::LMFTracker->Measurement->Profiles->Selected->Name) << std::endl;

	leica_->setStringParam(leica_->L_GUID_10, (decode)(GlobalObjects::LMFTracker->Measurement->Profiles->Selected->GUID));
	leica_->setStringParam(leica_->L_Name_10, (decode)(GlobalObjects::LMFTracker->Measurement->Profiles->Selected->Name));

	// and again, I have a listed callback that I can not actually reach via normal methods
			// TODO
			// Research how this might work in the current context
			// afx_msg void OnProfileSelectedChanged(IMeasurementProfileCollectionCOM * sender, IMeasurementProfileCOM * profile);

	//		LMF::Tracker::Measurements::MeasurementProfileCollection^ here = dynamic_cast<LMF::Tracker::Measurements::MeasurementProfileCollection^>(LMFTracker->Measurement->Profiles);
	//		here->

//////////////////////// This repeats whatever you actually selected . . .  but really could just be stipped totally

#ifdef VERBOSE

	if (LMF::Tracker::Measurements::Profiles::StationaryMeasurementProfile^ thisProfile = dynamic_cast<LMF::Tracker::Measurements::Profiles::StationaryMeasurementProfile^>(GlobalObjects::LMFTracker->Measurement->Profiles->Selected))
	{
		thisProfile->Accuracy->Changed += gcnew LMF::Tracker::BasicTypes::EnumTypes::AccuracyValue::ChangedEventHandler(&OnMeasChanged);
		//+= gcnew LMF::Tracker::BasicTypes::EnumTypes::AccuracyValue::ChangedEventHandler(&OnChanged);

		std::cout << "StationaryMeasurementProfile" << std::endl;
		const char* EAccuracyStrings[] = { "Precise", "Standard", "Fast" }; //ok

		std::cout << "Accuracy: Label: " << (decode)(thisProfile->Accuracy->Label) <<
			" Value: " << EAccuracyStrings[(int)thisProfile->Accuracy->Value] <<
			std::endl;

		Do_BoolValue("TwoFace", thisProfile->TwoFace);

		//methods
		//	thisProfile->Select();
	}
	else if (LMF::Tracker::Measurements::Profiles::ContinuousTimeProfile^ thisProfile = dynamic_cast<LMF::Tracker::Measurements::Profiles::ContinuousTimeProfile^>(GlobalObjects::LMFTracker->Measurement->Profiles->Selected))
	{
		std::cout << "ContinuousTimeProfile" << std::endl;
		Do_IntValueWithRange("PacketRate", thisProfile->PacketRate);
		Do_DoubleValueWithRange("TimeSeparation", thisProfile->TimeSeparation);

		//methods
				//	thisProfile->Select();
	}
	else if (LMF::Tracker::Measurements::Profiles::ContinuousDistanceProfile^ thisProfile = dynamic_cast<LMF::Tracker::Measurements::Profiles::ContinuousDistanceProfile^>(GlobalObjects::LMFTracker->Measurement->Profiles->Selected))
	{
		std::cout << "ContinuousDistanceProfile" << std::endl;
		Do_DoubleValueWithRange("DistanceSeparation", thisProfile->DistanceSeparation);
	}
	else if (LMF::Tracker::Measurements::Profiles::CustomTriggerProfile^ thisProfile = dynamic_cast<LMF::Tracker::Measurements::Profiles::CustomTriggerProfile^>(GlobalObjects::LMFTracker->Measurement->Profiles->Selected))
	{
		thisProfile->TriggeredMeasurementsArrived += gcnew LMF::Tracker::Measurements::Profiles::CustomTriggerProfile::TriggeredMeasurementsArrivedHandler(&OnTriggeredMeasurementsArrived);

		thisProfile->ClockSource->Changed += gcnew LMF::Tracker::BasicTypes::EnumTypes::ClockSourceValue::ChangedEventHandler(&OnMeasChanged);
		//+= gcnew LMF::Tracker::BasicTypes::EnumTypes::ClockSourceValue::ChangedEventHandler(&OnChanged);
		thisProfile->ClockTransmission->Changed += gcnew LMF::Tracker::BasicTypes::EnumTypes::ClockTransmissionValue::ChangedEventHandler(&OnMeasChanged);
		//+= gcnew LMF::Tracker::BasicTypes::EnumTypes::ClockTransmissionValue::ChangedEventHandler(&OnChanged);
		thisProfile->StartStopActiveLevel->Changed += gcnew LMF::Tracker::BasicTypes::EnumTypes::StartStopActiveLevelValue::ChangedEventHandler(&OnMeasChanged);
		//+= gcnew LMF::Tracker::BasicTypes::EnumTypes::StartStopActiveLevelValue::ChangedEventHandler(&OnChanged);
		thisProfile->StartStopSource->Changed += gcnew LMF::Tracker::BasicTypes::EnumTypes::StartStopSourceValue::ChangedEventHandler(&OnMeasChanged);
		//+= gcnew LMF::Tracker::BasicTypes::EnumTypes::StartStopSourceValue::ChangedEventHandler(&OnMeasChanged);

		const char* EClockSourceStrings[] = { "Internal", "External" }; //ok
		std::cout << "CustomTriggerProfile" << std::endl;
		std::cout << "ClockSource: Label: " << (decode)(thisProfile->ClockSource->Label) <<
			" Value: " << EClockSourceStrings[(int)thisProfile->ClockSource->Value] <<
			std::endl;

		const char* EClockTransmissionStrings[] = { "Negative", "Positve" }; //ok
		std::cout << "ClockTransmission: Label: " << (decode)(thisProfile->ClockTransmission->Label) <<
			" Value: " << EClockTransmissionStrings[(int)thisProfile->ClockTransmission->Value] <<
			std::endl;

		Do_DoubleValueWithRange("MinimalTimeDelay", thisProfile->MinimalTimeDelay);
		Do_IntValueWithRange("PacketRate", thisProfile->PacketRate);

		const char* EStartStopActiveLevelStrings[] = { "Low", "High" }; //ok
		std::cout << "StartStopActiveLevel: Label: " << (decode)(thisProfile->StartStopActiveLevel->Label) <<
			" Value: " << EStartStopActiveLevelStrings[(int)thisProfile->StartStopActiveLevel->Value] <<
			std::endl;
		const char* EStartStopSourceStrings[] = { "Ignored", "Active" }; //ok
		std::cout << "StartStopSource: Label: " << (decode)(thisProfile->StartStopSource->Label) <<
			" Value: " << EStartStopSourceStrings[(int)thisProfile->StartStopSource->Value] <<
			std::endl;


		//methods
				 //	thisProfile->Select();
	}

#endif

	std::cout << std::endl;
	std::cout << "Status" << std::endl;
	GlobalObjects::LMFTracker->Measurement->Status->Changed += gcnew LMF::Tracker::MeasurementStatus::MeasurementStatusValue::ChangedEventHandler(&OnChanged);

	std::cout << "Label: " << (decode)(GlobalObjects::LMFTracker->Measurement->Status->Label) << std::endl;
	const char* EMeasurementStatusString[] = { "ReadyToMeasure", "MeasurementInProgress","NotReady","Invalid" };
	std::cout << "Value: " << EMeasurementStatusString[(int)(GlobalObjects::LMFTracker->Measurement->Status->Value)] << std::endl;

	leica_->setStringParam(leica_->L_Label_22, (decode)(GlobalObjects::LMFTracker->Measurement->Status->Label));
	leica_->setIntegerParam(leica_->L_Value_21, (int)(GlobalObjects::LMFTracker->Measurement->Status->Value));


	std::cout << "Preconditions: Count: " << GlobalObjects::LMFTracker->Measurement->Status->Preconditions->Count << std::endl;

	leica_->setIntegerParam(leica_->L_Count_2, GlobalObjects::LMFTracker->Measurement->Status->Preconditions->Count);

	for (int i = 0; i < GlobalObjects::LMFTracker->Measurement->Status->Preconditions->Count; i++)
	{
		LMF::Tracker::MeasurementStatus::MeasurementPrecondition^ preconditions = GlobalObjects::LMFTracker->Measurement->Status->Preconditions[i];

		std::cout << "Descriptions: " << (decode)(preconditions->Description) << std::endl;
		std::cout << "Number: " << preconditions->Number << std::endl;
		std::cout << "Solution: " << (decode)(preconditions->Solution) << std::endl;
		std::cout << "Title: " << (decode)(preconditions->Title) << std::endl;

		if (i = 0)
		{
			leica_->setStringParam(leica_->L_Description, (decode)(preconditions->Description));
			leica_->setIntegerParam(leica_->L_Number, preconditions->Number);
			leica_->setStringParam(leica_->L_Solution, (decode)(preconditions->Solution));
			leica_->setStringParam(leica_->L_Title, (decode)(preconditions->Title));

		}
		if (i = 1)
		{
			leica_->setStringParam(leica_->L_Description_1, (decode)(preconditions->Description));
			leica_->setIntegerParam(leica_->L_Number_1, preconditions->Number);
			leica_->setStringParam(leica_->L_Solution_1, (decode)(preconditions->Solution));
			leica_->setStringParam(leica_->L_Title_1, (decode)(preconditions->Title));

		}
		if (i = 2)
		{
			leica_->setStringParam(leica_->L_Description_2, (decode)(preconditions->Description));
			leica_->setIntegerParam(leica_->L_Number_2, preconditions->Number);
			leica_->setStringParam(leica_->L_Solution_2, (decode)(preconditions->Solution));
			leica_->setStringParam(leica_->L_Title_2, (decode)(preconditions->Title));

		}
		if (i = 3)
		{
			leica_->setStringParam(leica_->L_Description_3, (decode)(preconditions->Description));
			leica_->setIntegerParam(leica_->L_Number_3, preconditions->Number);
			leica_->setStringParam(leica_->L_Solution_3, (decode)(preconditions->Solution));
			leica_->setStringParam(leica_->L_Title_3, (decode)(preconditions->Title));

		}
		if (i = 4)
		{
			leica_->setStringParam(leica_->L_Description_4, (decode)(preconditions->Description));
			leica_->setIntegerParam(leica_->L_Number_4, preconditions->Number);
			leica_->setStringParam(leica_->L_Solution_4, (decode)(preconditions->Solution));
			leica_->setStringParam(leica_->L_Title_4, (decode)(preconditions->Title));

		}

	}



	std::cout << std::endl;

	leica_->callParamCallbacks();
}

void leica::Do_Settings(void)
{
	std::cout << std::endl;

	std::cout << "Settings\n";

	LMF::Units::ECoordinateType coordtype = GlobalObjects::LMFTracker->Settings->CoordinateType;
	const char* coordtypeNames[] = { "Spherical", "Cartesian", "Cylindrical" }; //ok
	std::cout << " CoordinateType : " << coordtypeNames[(int)coordtype] << std::endl;
	leica_->setStringParam(leica_->L_CoordinateType, coordtypeNames[(int)coordtype]);

	LMF::Units::ERotationType rottype = GlobalObjects::LMFTracker->Settings->RotationType;
	const char* rottypeNames[] = { "RotationAngles", "RollPitchYaw", "Quarternion" }; //ok
	std::cout << " RotationType : " << rottypeNames[(int)rottype] << std::endl;
	leica_->setStringParam(leica_->L_RotationType, rottypeNames[(int)rottype]);

	LMF::Units::EAngleUnit angunit = GlobalObjects::LMFTracker->Settings->Units->AngleUnit;
	const char* angunitNames[] = { "Radian", "Millirad", "Degree", "Gon", "CC" }; //ok
	std::cout << " AngleUnits : " << angunitNames[(int)angunit] << std::endl;
	leica_->setStringParam(leica_->L_AngleUnit, angunitNames[(int)angunit]);

	LMF::Units::EHumidityUnit humunit = GlobalObjects::LMFTracker->Settings->Units->HumidityUnit;
	const char* humunitNames[] = { "RelativeHumidity" }; //ok
	std::cout << " HumidityUnits : " << humunitNames[(int)humunit] << std::endl;
	leica_->setStringParam(leica_->L_HumidityUnit, humunitNames[(int)humunit]);

	LMF::Units::ELengthUnit lenunit = GlobalObjects::LMFTracker->Settings->Units->LengthUnit;
	const char* lenunitNames[] = { "Meter", "Millimeter", "Micrometer","Foot","Yard", "Inch" }; //ok
	std::cout << " LengthUnits : " << lenunitNames[(int)lenunit] << std::endl;
	leica_->setStringParam(leica_->L_LengthUnit, lenunitNames[(int)lenunit]);

	LMF::Units::EPercentUnit perunit = GlobalObjects::LMFTracker->Settings->Units->PercentUnit;
	const char* perunitNames[] = { "Percent", "None" }; //ok
	std::cout << " PercentUnits : " << perunitNames[(int)perunit] << std::endl;
	leica_->setStringParam(leica_->L_PercentUnit, perunitNames[(int)perunit]);

	LMF::Units::EPressureUnit presunit = GlobalObjects::LMFTracker->Settings->Units->PressureUnit;
	const char* presunitNames[] = { "mBar", "HPascal","KPascal","MmHg", "Psi", "InH2O","InHg" }; //ok
	std::cout << " PressureUnits : " << presunitNames[(int)presunit] << std::endl;
	leica_->setStringParam(leica_->L_PressureUnit, presunitNames[(int)presunit]);

	LMF::Units::ETemperatureUnit tempunit = GlobalObjects::LMFTracker->Settings->Units->TemperatureUnit;
	const char* tempunitNames[] = { "Celsius", "Fahrenheit" }; //ok
	std::cout << " TemperatureUnits : " << tempunitNames[(int)tempunit] << std::endl;
	leica_->setStringParam(leica_->L_TemperatureUnit, tempunitNames[(int)tempunit]);

	LMF::Units::ETimeUnit timeunit = GlobalObjects::LMFTracker->Settings->Units->TimeUnit;
	const char* timeunitNames[] = { "Millisecond", "Second", "Minute", "Hour" }; //ok
	std::cout << " TimeUnits : " << timeunitNames[(int)timeunit] << "\n\n";
	leica_->setStringParam(leica_->L_TimeUnit, timeunitNames[(int)timeunit]);

	std::cout << " Get Orientation \n";
	LMF::Tracker::Alignment^ orient = GlobalObjects::LMFTracker->Settings->GetOrientation();

	std::cout << "  CoordinateType : " << coordtypeNames[(int)orient->CoordinateType] << std::endl;
	leica_->setStringParam(leica_->L_CoordinateType, coordtypeNames[(int)orient->CoordinateType]);

	std::cout << "  RotationType : " << rottypeNames[(int)orient->RotationType] << std::endl;
	leica_->setStringParam(leica_->L_RotationType, rottypeNames[(int)orient->RotationType]);

	//
	//     The issue at this point is that as-is, none of these actually contain any actually values, and while it is trivial to print out the values
	// 	   that I get back from the hardware, slightly messier to do the epics aspects, so using a macro
	//

#define PV_IT(a,b,c,thing) \
leica_->setStringParam(leica_->L_Label_##a, (decode)(##thing->Label)); \
leica_->setStringParam(leica_->L_UnitString_##b, (decode)(##thing->UnitString)); \
leica_->setStringParam(leica_->L_UnitType_##b, EUnitTypeStrings[(int)##thing->UnitType]); \
leica_->setDoubleParam(leica_->L_Value_##c, ##thing->Value); \
leica_->setDoubleParam(leica_->L_ValueInBaseUnits_##b, ##thing->ValueInBaseUnits); 

	Do_SimpleDoubleValue("  Rotation0", orient->Rotation0);
	Do_SimpleDoubleValue("  Rotation1", orient->Rotation1);
	Do_SimpleDoubleValue("  Rotation2", orient->Rotation2);
	Do_SimpleDoubleValue("  Rotation3", orient->Rotation3);
	Do_SimpleDoubleValue("  Translation1", orient->Translation1);
	Do_SimpleDoubleValue("  Translation2", orient->Translation2);
	Do_SimpleDoubleValue("  Translation3", orient->Translation3);

	PV_IT(42, 19, 34, orient->Rotation0);
	PV_IT(43, 20, 35, orient->Rotation1);
	PV_IT(44, 21, 36, orient->Rotation2);
	PV_IT(45, 22, 37, orient->Rotation3);
	PV_IT(46, 23, 38, orient->Translation1);
	PV_IT(47, 24, 39, orient->Translation2);
	PV_IT(48, 25, 40, orient->Translation3);


	std::cout << " Get Transformation \n";
	LMF::Tracker::AlignmentWithScale^ transf = GlobalObjects::LMFTracker->Settings->GetTransformation();

	std::cout << "  CoordinateType : " << coordtypeNames[(int)transf->CoordinateType] << std::endl;
	std::cout << "  RotationType : " << rottypeNames[(int)transf->RotationType] << std::endl;

	Do_SimpleDoubleValue("  Rotation0", transf->Rotation0);
	Do_SimpleDoubleValue("  Rotation1", transf->Rotation1);
	Do_SimpleDoubleValue("  Rotation2", transf->Rotation2);
	Do_SimpleDoubleValue("  Rotation3", transf->Rotation3);
	Do_SimpleDoubleValue("  Scale", transf->Scale);
	Do_SimpleDoubleValue("  Translation1", transf->Translation1);
	Do_SimpleDoubleValue("  Translation2", transf->Translation2);
	Do_SimpleDoubleValue("  Translation3", transf->Translation3);

	PV_IT(49, 26, 41, transf->Rotation0);
	PV_IT(50, 27, 42, transf->Rotation1);
	PV_IT(51, 28, 43, transf->Rotation2);
	PV_IT(52, 29, 44, transf->Rotation3);
	PV_IT(53, 30, 45, transf->Scale);
	PV_IT(54, 31, 46, transf->Translation1);
	PV_IT(55, 32, 47, transf->Translation2);
	PV_IT(56, 33, 48, transf->Translation3);


	std::cout << std::endl;

	leica_->callParamCallbacks();
}


void leica::Do_OverviewCamera(void)
{
	std::cout << std::endl;
	std::cout << "OverviewCamera\n";

	// this comes back as a poubter to a type that I can not reasonable access from within epics 
//	GlobalObjects::LMFTracker->OverviewCamera->ImageArrived += gcnew LMF::Tracker::OVC::OverviewCamera::ImageArrivedHandler(&OnImageArrived);

	GlobalObjects::LMFTracker->OverviewCamera->WPFBitmapImageArrived += gcnew LMF::Tracker::OVC::OverviewCamera::WPFBitmapImageArrivedHandler(&OnWPFBitmapImageArrived);

	// METHODS

	//	GlobalObjects::LMFTracker->OverviewCamera->GetStillImage(LMF::Tracker::Enums::EStillImageMode::High);
	GlobalObjects::LMFTracker->OverviewCamera->GetStillImage(LMF::Tracker::Enums::EStillImageMode::Medium);
	//	GlobalObjects::LMFTracker->OverviewCamera->GetStillImage(LMF::Tracker::Enums::EStillImageMode::Low);

// code here if I actually wanted to do something like test some looking for images and then saving them instead of NDArraying them
	//	LMFTracker->OverviewCamera->MoveToPixel
	//	LMFTracker->OverviewCamera->StartAsync();
//	GlobalObjects::LMFTracker->OverviewCamera->Stop();

	GlobalObjects::LMFTracker->OverviewCamera->Brightness->Changed += gcnew LMF::Tracker::BasicTypes::DoubleValue::ReadOnlyDoubleValue::ChangedEventHandler(&OnBrightnessChanged);


	Do_DoubleValueWithRange(" Brightness", GlobalObjects::LMFTracker->OverviewCamera->Brightness);
	Do_DoubleValueWithRange(" Constrast", GlobalObjects::LMFTracker->OverviewCamera->Contrast);
	/*
	#define PV_IT_2(a,b,c,d,thing) \
	leica_->setStringParam(leica_->L_Label_##a, (decode)(##thing->Label)); \
	leica_->setDoubleParam(leica_->L_MaxValue_##b, ##thing->MaxValue); \
	leica_->setDoubleParam(leica_->L_MaxValueInBaseUnits_##b, ##thing->MaxValueInBaseUnits); \
	leica_->setDoubleParam(leica_->L_MinValue_##b, ##thing->MinValue); \
	leica_->setDoubleParam(leica_->L_MinValueInBaseUnits_##b, ##thing->MinValueInBaseUnits); \
	leica_->setStringParam(leica_->L_UnitString_##c, (decode)(##thing->UnitString)); \
	leica_->setStringParam(leica_->L_UnitType_##c, EUnitTypeStrings[(int)##thing->UnitType]); \
	leica_->setDoubleParam(leica_->L_Value_##d, ##thing->Value); \
	leica_->setDoubleParam(leica_->L_ValueInBaseUnits_##c, ##thing->ValueInBaseUnits);
	*/

	PV_IT_2(36, 8, 16, 28, GlobalObjects::LMFTracker->OverviewCamera->Brightness);
	PV_IT_2(37, 9, 17, 29, GlobalObjects::LMFTracker->OverviewCamera->Contrast);

	// more Methods

	GlobalObjects::LMFTracker->OverviewCamera->Dialog->Closed += gcnew LMF::Tracker::OVC::Dialog::ClosedHandler(&OnDialogClosed);

	//	LMFTracker->OverviewCamera->Dialog->Close();
	//	LMFTracker->OverviewCamera->Dialog->Show();
	//	LMFTracker->OverviewCamera->Dialog->ShowDialog();
	//	LMFTracker->OverviewCamera->Dialog->ShowOnProcess();
	//	LMFTracker->OverviewCamera->Dialog->ShowTopmost();

	std::cout << std::endl;
	leica_->callParamCallbacks();
}

void leica::Do_QuickRelease(void)
{
	std::cout << std::endl;
	std::cout << "QuickRelease\n";

	GlobalObjects::LMFTracker->QuickRelease->Changed += gcnew LMF::Tracker::BasicTypes::BoolValue::ReadOnlyBoolValue::ChangedEventHandler(&OnQuickChanged);

	std::cout << " QuickRelease: Label: " << (decode)(GlobalObjects::LMFTracker->QuickRelease->Label) << " Value: " << TFS[GlobalObjects::LMFTracker->QuickRelease->Value] << std::endl;
	leica_->setStringParam(leica_->L_Label_41, (decode)(GlobalObjects::LMFTracker->QuickRelease->Label));
	leica_->setIntegerParam(leica_->L_Value_33, GlobalObjects::LMFTracker->QuickRelease->Value);

	std::cout << std::endl;
	leica_->callParamCallbacks();
}

void leica::Do_PowerSource(void)
{
	std::cout << std::endl;
	std::cout << "PowerSource\n";

	// does not seem to exist (at run time!) and a try block doesn't correct trap it Maybe only valid on Battery operated units

	/*
		try {
			LMFTracker->PowerSource->SensorPowerStatus->Level->Changed += gcnew LMF::Tracker::BasicTypes::DoubleValue::ReadOnlyDoubleValue::ChangedEventHandler(&OnChanged);
			std::cout << "Sensor Power Status: Label: " << (decode)(LMFTracker->PowerSource->SensorPowerStatus->Level->Label) << " Value: " << LMFTracker->PowerSource->SensorPowerStatus->Level->Value << std::endl;
		}
		catch (LMF::Tracker::ErrorHandling::LmfException^ e)
		{
			std::cout << "Sensor Power Status Error Code: " << e->Number << " " << (decode)(e->Description) << std::endl;
		}
	*/

	GlobalObjects::LMFTracker->PowerSource->ControllerPowerStatus->RunsOn->Changed += gcnew LMF::Tracker::BasicTypes::EnumTypes::ReadOnlyPowerSourceValue::ChangedEventHandler(&OnPowerChanged);

	Do_ReadOnlyDoubleValue(" SensorPowerStatus", GlobalObjects::LMFTracker->PowerSource->ControllerPowerStatus->Level);

	//	This has no useful information so not going to even trouble to populate PVs


	std::cout << " RunsOn: Label: " << (decode)(GlobalObjects::LMFTracker->PowerSource->ControllerPowerStatus->RunsOn->Label) <<
		" Value: " << EPowerSourceStrings[(int)GlobalObjects::LMFTracker->PowerSource->ControllerPowerStatus->RunsOn->Value] <<
		//		" Value: " << (int)LMFTracker->PowerSource->ControllerPowerStatus->RunsOn->Value <<
		std::endl;
	leica_->setStringParam(leica_->L_Label_40, (decode)(GlobalObjects::LMFTracker->PowerSource->ControllerPowerStatus->RunsOn->Label));
	leica_->setStringParam(leica_->L_Value_32, EPowerSourceStrings[(int)GlobalObjects::LMFTracker->PowerSource->ControllerPowerStatus->RunsOn->Value]);

	std::cout << std::endl;
	leica_->callParamCallbacks();

}



void leica::Do_PowerLock(void)
{
	std::cout << std::endl;
	std::cout << "PowerLock\n";
	GlobalObjects::LMFTracker->PowerLock->UsePowerLock->Changed += gcnew LMF::Tracker::BasicTypes::BoolValue::ReadOnlyBoolValue::ChangedEventHandler(&OnPowerLockChanged);

	std::cout << " Use Power Lock: Label: " << (decode)(GlobalObjects::LMFTracker->PowerLock->UsePowerLock->Label) << " Value: " << TFS[GlobalObjects::LMFTracker->PowerLock->UsePowerLock->Value] << std::endl;

	leica_->setStringParam(leica_->L_Label_38, (decode)(GlobalObjects::LMFTracker->PowerLock->UsePowerLock->Label));
	leica_->setIntegerParam(leica_->L_Value_30, (int)GlobalObjects::LMFTracker->PowerLock->UsePowerLock->Value);

	LMF::Tracker::OVC::ATRCoordinateCollection^ gettargetdirections = GlobalObjects::LMFTracker->PowerLock->GetTargetDirections();

	std::cout << " Get Target Directions Count: " << gettargetdirections->Count << std::endl;

	// This is all handled elsewhere via callback
	// I have limited use to what you can do with just 2 angles and pixel locations without knowing the image size 
		// And it is a heck of a lot easier to get the coordinates of the trackers in the camera callbacks
	// NOTHING uses this as parameters. The number are *twice* the values  as returned by the image callbacks
	// so printing them here, and not wiring *anything* else related unless and until someone has an actual use case
	for (int i = 0; i < gettargetdirections->Count; i++)
	{
		std::cout << std::endl;

		LMF::Tracker::OVC::ATRCoordinate^ thing = gettargetdirections[i];

		Do_SimpleDoubleValue("  AngleHz", thing->AngleHz);
		Do_SimpleDoubleValue("  AngleVt", thing->AngleVt);
		Do_SimpleDoubleValue("  PixelX", thing->PixelX);
		Do_SimpleDoubleValue("  PixelY", thing->PixelY);
		//		std::cout << std::endl;

// I did not actually make elements to decode anything here - again, this info typically back in a timely manner via a callback
	}

	std::cout << std::endl;
	leica_->callParamCallbacks();

}

void leica::Do_InclinationSensor(void)
{
	std::cout << std::endl;
	std::cout << "InclinationSensor\n";

	std::cout << " Checking Inclination to Gravity . . . " << std::endl;


	GlobalObjects::LMFTracker->InclinationSensor->GetInclinationToGravityFinished += gcnew LMF::Tracker::Inclination::InclinationSensor::GetInclinationToGravityFinishedHandler(&OnGetInclinationToGravityFinished);
	GlobalObjects::LMFTracker->InclinationSensor->BubbleReadout->BubbleReadoutArrived += gcnew LMF::Tracker::Inclination::InclinationBubbleReadout::BubbleReadoutArrivedHandler(&OnBubbleReadoutArrived);
	GlobalObjects::LMFTracker->InclinationSensor->InclinedToGravity->Changed += gcnew LMF::Tracker::BasicTypes::BoolValue::ReadOnlyBoolValue::ChangedEventHandler(&OnIncChanged);
	GlobalObjects::LMFTracker->InclinationSensor->Monitoring->InclinationChanged += gcnew LMF::Tracker::Inclination::InclinationMonitoring::InclinationChangedHandler(&OnInclinationChanged);

	//	if (GlobalObjects::LMFTracker->InclinationSensor->InclinedToGravity->Value == false)
	// always do it, seems to be a heck of a lot less noisy with the callbacks 	
	if (1)
	{
		std::cout << " Performing InclinationToGravity . . . Please Wait . . .  " << std::endl;

		// If waiting, you can do everything linearly, if async, you might have to restart everything in the on finished callback 
		try {
			GlobalObjects::LMFTracker->InclinationSensor->GetInclinationToGravity();
			//			GlobalObjects::LMFTracker->InclinationSensor->GetInclinationToGravityAsync();
			GlobalObjects::LMFTracker->InclinationSensor->InclinedToGravity->Value = true;
			GlobalObjects::LMFTracker->InclinationSensor->Monitoring->Active->Value = true;
			leica_->setIntegerParam(leica_->L_Value_1, 1);
			leica_->setIntegerParam(leica_->L_Value_2, 1);

			GlobalObjects::LMFTracker->InclinationSensor->BubbleReadout->StartBubbleReadoutStream();
		}
		catch (...)
		{
			std::cout << " Failed!" << std::endl;
		}

	}
	else
		std::cout << " Already Inclined . . . Using last values . . . " << std::endl;

	// methods
	/*
		LMFTracker->InclinationSensor->GetInclinationToGravity();
		LMFTracker->InclinationSensor->GetInclinationToGravityAsync();
		LMFTracker->InclinationSensor->Measure();
	*/


	GlobalObjects::LMFTracker->InclinationSensor->Monitoring->Active->Value = true;

	std::cout << " Attemping StartBubbleReadoutStream . . . " << std::endl;

	// This might be about 1/2 event, just doing this to try to capture the event.

//	GlobalObjects::LMFTracker->InclinationSensor->BubbleReadout->StartBubbleReadoutStream();
//	Sleep(20);
//	Sleep(11000);
//	GlobalObjects::LMFTracker->InclinationSensor->BubbleReadout->StopBubbleReadoutStream();

	// and may as well turn on, if we can, so that we don't have to try block it at the instant

	try {
		GlobalObjects::LMFTracker->InclinationSensor->Measure();
	}
	catch (LMF::Tracker::ErrorHandling::LmfException^ e)
	{
		std::cout << " Error exception " << e->Number << " " << (decode)(e->Description) << std::endl;;
		//		std::cout << "Hit an exception trying to decode Check For Errors  \n";
	}

	DateTime^ dt;
	dt = GlobalObjects::LMFTracker->InclinationSensor->CurrentInclinationToGravity->TimeStamp;
	std::cout << " CurrentInclinationToGravity: TimeStamp: " << (decode)(dt->ToString("dddd, dd. MMMM yyyy HH:mm:ss.fff")) << std::endl;

	Do_SimpleDoubleValue(" InclinationRotX", GlobalObjects::LMFTracker->InclinationSensor->CurrentInclinationToGravity->InclinationRotX);
	Do_SimpleDoubleValue(" InclinationRotY", GlobalObjects::LMFTracker->InclinationSensor->CurrentInclinationToGravity->InclinationRotY);
	Do_BoolValue(" ", GlobalObjects::LMFTracker->InclinationSensor->InclinedToGravity);

	//	GlobalObjects::LMFTracker->InclinationSensor->Monitoring->InclinationChanged += gcnew LMF::Tracker::Inclination::InclinationMonitoring::InclinationChangedHandler(&OnInclinationChanged);

	dt = GlobalObjects::LMFTracker->InclinationSensor->Monitoring->ThresholdExceededTime;
	std::cout << " ThresholdExceededTime: " << (decode)(dt->ToString("dddd, dd. MMMM yyyy HH:mm:ss.fff")) << std::endl;

	dt = GlobalObjects::LMFTracker->InclinationSensor->Monitoring->WorkingRangeExceededTime;
	std::cout << " WorkingRangeExceededTime: " << (decode)(dt->ToString("dddd, dd. MMMM yyyy HH:mm:ss.fff")) << std::endl;

	Do_BoolValue("", GlobalObjects::LMFTracker->InclinationSensor->Monitoring->Active);

	dt = GlobalObjects::LMFTracker->InclinationSensor->Monitoring->Current->TimeStamp;
	std::cout << " Current: TimeStamp: " << (decode)(dt->ToString("dddd, dd. MMMM yyyy HH:mm:ss.fff")) << std::endl;

	Do_SimpleDoubleValue(" X", GlobalObjects::LMFTracker->InclinationSensor->Monitoring->Current->X);
	Do_SimpleDoubleValue(" Y", GlobalObjects::LMFTracker->InclinationSensor->Monitoring->Current->Y);

	Do_ReadOnlyDoubleValue(" Interval", GlobalObjects::LMFTracker->InclinationSensor->Monitoring->Interval);
	Do_ReadOnlyDoubleValue(" Theshold", GlobalObjects::LMFTracker->InclinationSensor->Monitoring->Threshold);
	Do_BoolValue(" ", GlobalObjects::LMFTracker->InclinationSensor->Monitoring->ThresholdExceeded);
	Do_BoolValue(" ", GlobalObjects::LMFTracker->InclinationSensor->Monitoring->WorkingRangeExceeded);

	//	LMFTracker->InclinationSensor->Monitoring->Reset();

		// This might be about 1/2 event, just doing this to try to capture the event, again

//	GlobalObjects::LMFTracker->InclinationSensor->BubbleReadout->StartBubbleReadoutStream();
//	Sleep(20);
//	Sleep(11000);
//	GlobalObjects::LMFTracker->InclinationSensor->BubbleReadout->StopBubbleReadoutStream();



	std::cout << std::endl;

}


/** Constructor for leica; most parameters are simply passed to ADDriver::ADDriver.
  * After calling the base class constructor this method creates a thread to compute the simulated detector data,
  * and sets reasonable default values for parameters defined in this class, asynNDArrayDriver and ADDriver.
  * \param[in] portName The name of the asyn port driver to be created.
  * \param[in] maxSizeX The maximum X dimension of the images that this driver can create.
  * \param[in] maxSizeY The maximum Y dimension of the images that this driver can create.
  * \param[in] dataType The initial data type (NDDataType_t) of the images that this driver will create.
  * \param[in] maxBuffers The maximum number of NDArray buffers that the NDArrayPool for this driver is
  *            allowed to allocate. Set this to -1 to allow an unlimited number of buffers.
  * \param[in] maxMemory The maximum amount of memory that the NDArrayPool for this driver is
  *            allowed to allocate. Set this to -1 to allow an unlimited amount of memory.
  * \param[in] priority The thread priority for the asyn port driver thread if ASYN_CANBLOCK is set in asynFlags.
  * \param[in] stackSize The stack size for the asyn port driver thread if ASYN_CANBLOCK is set in asynFlags.
  */
leica::leica(const char* portName, int maxSizeX, int maxSizeY, NDDataType_t dataType,
	int maxBuffers, size_t maxMemory, int priority, int stackSize)

	: ADDriver(portName, 1, 0, maxBuffers, maxMemory,
		0, 0, /* No interfaces beyond those set in ADDriver.cpp */
		0, 1, /* ASYN_CANBLOCK=0, ASYN_MULTIDEVICE=0, autoConnect=1 */
		priority, stackSize)

{
	int status = asynSuccess;
	char versionString[20];
	const char* functionName = "leica";

	leica_ = this;

	createParam(L_noopString, asynParamInt32, &L_noop);

#include "leica.cpp.inc"

	createParam(L_noop2String, asynParamInt32, &L_noop2);


	initializeHardware(portName);


	printf("\n***********************************\n");
	printf("\nConnected to Laser Tracker, checking default parameters . . . \n");

	Do_Face();
	Do_Laser();
	Do_Settings();
	Do_OverviewCamera();
	Do_PowerLock();
	Do_PowerSource();
	Do_QuickRelease();
	Do_InclinationSensor();
	Do_MeteoStation();
	Do_Measurement();

	// Global top level parameters	

	epicsSnprintf(versionString, sizeof(versionString), "%d.%d.%d",
		DRIVER_VERSION, DRIVER_REVISION, DRIVER_MODIFICATION);
	setStringParam(NDDriverVersion, versionString);
	setStringParam(ADSDKVersion, "1.10.0.372");

	setIntegerParam(ADMaxSizeX, 2560);
	setIntegerParam(ADMaxSizeY, 1920);

	setStringParam(ADManufacturer, "Leica");

	std::cout << "Top Level Information" << std::endl;

	String^ Comment = GlobalObjects::LMFTracker->Comment;
	cout << "Comment: " << (decode)(Comment) << "\n";
	setStringParam(L_Topcomment, (decode)(Comment));

	String^ ExpectedFirmware = GlobalObjects::LMFTracker->ExpectedFirmware;
	cout << "Firmware: " << (decode)(ExpectedFirmware) << "\n";
	setStringParam(L_expectedFirmware, (decode)(ExpectedFirmware));

	String^ InstalledFirmware = GlobalObjects::LMFTracker->InstalledFirmware;
	cout << "InstalledFirmware: " << (decode)(InstalledFirmware) << "\n";
	setStringParam(L_installedFirmware, (decode)(InstalledFirmware));
	setStringParam(ADFirmwareVersion, (decode)(InstalledFirmware));

	String^ IP = GlobalObjects::LMFTracker->IPAddress;
	cout << "IP: " << (decode)(IP) << "\n";
	setStringParam(L_ipAddress, (decode)(IP));

	Boolean CompatFirmware = GlobalObjects::LMFTracker->IsCompatibleWithInstalledFirmware;
	cout << "Is Compatible With Installed Firmware: " << TFS[CompatFirmware] << "\n";
	setIntegerParam(L_IsCompatible, CompatFirmware);

	String^ Name = GlobalObjects::LMFTracker->Name;
	cout << "Name: " << (decode)(Name) << "\n";
	setStringParam(L_Name_15, (decode)(Name));

	String^ ProductName = GlobalObjects::LMFTracker->ProductName;
	cout << "ProductName: " << (decode)(ProductName) << "\n";
	setStringParam(L_ProductName_3, (decode)(ProductName));
	setStringParam(ADModel, (decode)(ProductName));

	String^ SerialNumber = GlobalObjects::LMFTracker->SerialNumber;
	cout << "Serial: " << (decode)(SerialNumber) << "\n";
	setStringParam(L_SerialNumber_7, (decode)(SerialNumber));
	setStringParam(ADSerialNumber, (decode)(SerialNumber));


	GlobalObjects::LMFTracker->GetDirectionAsync();
	Direction^ dir1 = GlobalObjects::LMFTracker->GetDirection();


	cout << "Direction H Angle: " << dir1->HorizontalAngle->Value << " " << (decode)(dir1->HorizontalAngle->UnitString)
		<< " V Angle: " << dir1->VerticalAngle->Value << " " << (decode)(dir1->VerticalAngle->UnitString) << "\n";
	setDoubleParam(L_horizontalAngle, dir1->HorizontalAngle->Value);
	setDoubleParam(L_verticalAngle, dir1->VerticalAngle->Value);
	setStringParam(L_angleUnits, (decode)(dir1->VerticalAngle->UnitString));

	std::cout << std::endl;

	// This is now all down in the Do_inclination call . . .  
	/*
		// quick and dirty trun on orient to gravity

		std::cout << "Performing Inclination to Gravity . . . Please wait . . ." << std::endl;

		GlobalObjects::LMFTracker->InclinationSensor->GetInclinationToGravityFinished += gcnew LMF::Tracker::Inclination::InclinationSensor::GetInclinationToGravityFinishedHandler(&OnGetInclinationToGravityFinished);
		GlobalObjects::LMFTracker->InclinationSensor->BubbleReadout->BubbleReadoutArrived += gcnew LMF::Tracker::Inclination::InclinationBubbleReadout::BubbleReadoutArrivedHandler(&OnBubbleReadoutArrived);
		GlobalObjects::LMFTracker->InclinationSensor->InclinedToGravity->Changed += gcnew LMF::Tracker::BasicTypes::BoolValue::ReadOnlyBoolValue::ChangedEventHandler(&OnIncChanged);
		GlobalObjects::LMFTracker->InclinationSensor->Monitoring->InclinationChanged += gcnew LMF::Tracker::Inclination::InclinationMonitoring::InclinationChangedHandler(&OnInclinationChanged);


		try {
			GlobalObjects::LMFTracker->InclinationSensor->GetInclinationToGravity();
			GlobalObjects::LMFTracker->InclinationSensor->InclinedToGravity->Value = true;
			GlobalObjects::LMFTracker->InclinationSensor->BubbleReadout->StartBubbleReadoutStream();
		}
		catch (...)
		{
			std::cout << "Failed!" << std::endl;
		}


	*/


	// this code you can't actually do unless a tracker actually exists

	// if you don't pick mediuam, you get a mix of mostlu medium res images and maybe 2 fps of the larger or smaller ones.

// now done in the overviewCamera call, if done at all

//	GlobalObjects::LMFTracker->OverviewCamera->WPFBitmapImageArrived += gcnew LMF::Tracker::OVC::OverviewCamera::WPFBitmapImageArrivedHandler(&OnWPFBitmapImageArrived);
//	GlobalObjects::LMFTracker->OverviewCamera->WPFBitmapImageArrived += gcnew LMF::Tracker::OVC::OverviewCamera::WPFBitmapImageArrivedHandler(&OnWPFBitmapImageArrived);

/*

//	GlobalObjects::LMFTracker->OverviewCamera->ImageArrived += gcnew LMF::Tracker::OVC::OverviewCamera::ImageArrivedHandler(&OnImageArrived);

	GlobalObjects::LMFTracker->OverviewCamera->WPFBitmapImageArrived += gcnew LMF::Tracker::OVC::OverviewCamera::WPFBitmapImageArrivedHandler(&OnWPFBitmapImageArrived);

	GlobalObjects::LMFTracker->OverviewCamera->Stop();

	//	GlobalObjects::LMFTracker->OverviewCamera->GetStillImage(LMF::Tracker::Enums::EStillImageMode::High);

	GlobalObjects::LMFTracker->OverviewCamera->GetStillImage(LMF::Tracker::Enums::EStillImageMode::Medium);
	//	GlobalObjects::LMFTracker->OverviewCamera->GetStillImage(LMF::Tracker::Enums::EStillImageMode::Low);

	GlobalObjects::LMFTracker->OverviewCamera->StartAsync();
	Sleep(1000);
	GlobalObjects::LMFTracker->OverviewCamera->Stop();
*/

// These are handled now in do measurement
/*
	GlobalObjects::LMFTracker->Measurement->MeasurementArrived += gcnew LMF::Tracker::Measurements::MeasurementSettings::MeasurementArrivedHandler(&OnMeasurementArrived);
	// also not very helpful
//	GlobalObjects::LMFTracker->Measurement->Status->Changed += gcnew LMF::Tracker::MeasurementStatus::MeasurementStatusValue::ChangedEventHandler(&OnChanged);
	GlobalObjects::LMFTracker->Measurement->Status->Changed += gcnew LMF::Tracker::MeasurementStatus::MeasurementStatusValue::ChangedEventHandler(&OnChanged);
	//almost too generic to be of much use, I.e., not sure that putting this into a PV would be helpful
	GlobalObjects::LMFTracker->Measurement->MeasurementInProgress->Changed += gcnew LMF::Tracker::BasicTypes::BoolValue::ReadOnlyBoolValue::ChangedEventHandler(&OnMeasChanged);
*/
// set a profile before actually start counting


// This is now handled in Do_measurement

/*
	std::cout << "Measurement Profiles Count: " << GlobalObjects::LMFTracker->Measurement->Profiles->Count << std::endl;
	for (int i = 0; i < GlobalObjects::LMFTracker->Measurement->Profiles->Count; i++)
	{
		std::cout << std::endl;
		LMF::Tracker::Measurements::MeasurementProfile^ profile = GlobalObjects::LMFTracker->Measurement->Profiles[i];

		if (LMF::Tracker::Measurements::Profiles::StationaryMeasurementProfile^ thisProfile = dynamic_cast<LMF::Tracker::Measurements::Profiles::StationaryMeasurementProfile^>(profile))
		{
			//			thisProfile->Accuracy->Changed += gcnew LMF::Tracker::BasicTypes::EnumTypes::AccuracyValue::ChangedEventHandler(&OnChanged);

			std::cout << "StationaryMeasurementProfile" << std::endl;
			std::cout << "GUID: " << (decode)(profile->GUID) << std::endl;
			std::cout << "Name: " << (decode)(profile->Name) << std::endl;

			const char* EAccuracyStrings[] = { "Precise", "Standard", "Fast" }; //ok

			std::cout << "Accuracy: Label: " << (decode)(thisProfile->Accuracy->Label) <<
				" Value: " << EAccuracyStrings[(int)thisProfile->Accuracy->Value] <<
				std::endl;

			Do_BoolValue("TwoFace", thisProfile->TwoFace);

			//methods
			//	thisProfile->Select();
		}
		else if (LMF::Tracker::Measurements::Profiles::ContinuousTimeProfile^ thisProfile = dynamic_cast<LMF::Tracker::Measurements::Profiles::ContinuousTimeProfile^>(profile))
		{
			std::cout << "ContinuousTimeProfile" << std::endl;
			std::cout << "GUID: " << (decode)(profile->GUID) << std::endl;
			std::cout << "Name: " << (decode)(profile->Name) << std::endl;


			thisProfile->TimeSeparation->Value = 1000;
			thisProfile->TimeSeparation->ValueInBaseUnits = 1000;

			Do_IntValueWithRange("PacketRate", thisProfile->PacketRate);
			Do_DoubleValueWithRange("TimeSeparation", thisProfile->TimeSeparation);

			//methods
			thisProfile->Select();
		}
		else if (LMF::Tracker::Measurements::Profiles::ContinuousDistanceProfile^ thisProfile = dynamic_cast<LMF::Tracker::Measurements::Profiles::ContinuousDistanceProfile^>(profile))
		{
			std::cout << "ContinuousDistanceProfile" << std::endl;
			std::cout << "GUID: " << (decode)(profile->GUID) << std::endl;
			std::cout << "Name: " << (decode)(profile->Name) << std::endl;

			Do_DoubleValueWithRange("DistanceSeparation", thisProfile->DistanceSeparation);
		}
		else if (LMF::Tracker::Measurements::Profiles::CustomTriggerProfile^ thisProfile = dynamic_cast<LMF::Tracker::Measurements::Profiles::CustomTriggerProfile^>(profile))
		{
			//			thisProfile->TriggeredMeasurementsArrived += gcnew LMF::Tracker::Measurements::Profiles::CustomTriggerProfile::TriggeredMeasurementsArrivedHandler(&OnTriggeredMeasurementsArrived);

			//			thisProfile->ClockSource->Changed += gcnew LMF::Tracker::BasicTypes::EnumTypes::ClockSourceValue::ChangedEventHandler(&OnChanged);
			//			thisProfile->ClockTransmission->Changed += gcnew LMF::Tracker::BasicTypes::EnumTypes::ClockTransmissionValue::ChangedEventHandler(&OnChanged);
			//			thisProfile->StartStopActiveLevel->Changed += gcnew LMF::Tracker::BasicTypes::EnumTypes::StartStopActiveLevelValue::ChangedEventHandler(&OnChanged);
			//			thisProfile->StartStopSource->Changed += gcnew LMF::Tracker::BasicTypes::EnumTypes::StartStopSourceValue::ChangedEventHandler(&OnChanged);

			std::cout << "CustomTriggerProfile" << std::endl;
			std::cout << "GUID: " << (decode)(profile->GUID) << std::endl;
			std::cout << "Name: " << (decode)(profile->Name) << std::endl;

			const char* EClockSourceStrings[] = { "Internal", "External" }; //ok

			std::cout << "ClockSource: Label: " << (decode)(thisProfile->ClockSource->Label) <<
				" Value: " << EClockSourceStrings[(int)thisProfile->ClockSource->Value] <<
				std::endl;

			const char* EClockTransmissionStrings[] = { "Negative", "Positve" }; //ok
			std::cout << "ClockTransmission: Label: " << (decode)(thisProfile->ClockTransmission->Label) <<
				" Value: " << EClockTransmissionStrings[(int)thisProfile->ClockTransmission->Value] <<
				std::endl;

			Do_DoubleValueWithRange("MinimalTimeDelay", thisProfile->MinimalTimeDelay);
			Do_IntValueWithRange("PacketRate", thisProfile->PacketRate);

			const char* EStartStopActiveLevelStrings[] = { "Low", "High" }; //ok
			std::cout << "StartStopActiveLevel: Label: " << (decode)(thisProfile->StartStopActiveLevel->Label) <<
				" Value: " << EStartStopActiveLevelStrings[(int)thisProfile->StartStopActiveLevel->Value] <<
				std::endl;
			const char* EStartStopSourceStrings[] = { "Ignored", "Active" }; //ok
			std::cout << "StartStopSource: Label: " << (decode)(thisProfile->StartStopSource->Label) <<
				" Value: " << EStartStopSourceStrings[(int)thisProfile->StartStopSource->Value] <<
				std::endl;


			//methods
					 //	thisProfile->Select();
		}
	}
	// and the same thing for whatever the selected thing actually is


	std::cout << std::endl << "Selected" << std::endl;
	std::cout << "GUID: " << (decode)(GlobalObjects::LMFTracker->Measurement->Profiles->Selected->GUID) << std::endl;
	std::cout << "Name: " << (decode)(GlobalObjects::LMFTracker->Measurement->Profiles->Selected->Name) << std::endl;
	std::cout << std::endl;
*/

//	printf("%s\n",__LINE__);

#ifdef TARGETATSTARTUP

	LMF::Tracker::MeasurementResults::Measurement^ data = GlobalObjects::LMFTracker->Measurement->MeasureStationary();

	// May not have a visible target at startup . . . 
	// 
	GlobalObjects::LMFTracker->Measurement->StartMeasurement();
	////	CALL_AND_HANDLE(GlobalObjects::LMFTracker->Measurement->StartMeasurement());
	setIntegerParam(L_measonoff_command, 1);
	printf("%s\n", __LINE__);
	// This/these may not actually be required since values also appear from various other sources.

//	GlobalObjects::LMFTracker->MeteoStation->EnvironmentalValuesChanged += gcnew LMF::Tracker::Meteo::MeteoStation::EnvironmentalValuesChangedEventHandler(&OnEnvironmentalValuesChanged);

	// I do not know if this/these are required since  should be handled by the OnEnvironmentalValuesChanged callback but also values come back with each data pointer
//	GlobalObjects::LMFTracker->MeteoStation->HardwareHumidity->Changed += gcnew LMF::Tracker::BasicTypes::DoubleValue::ReadOnlyDoubleValue::ChangedEventHandler(&OnChanged);
//	GlobalObjects::LMFTracker->MeteoStation->HardwarePressure->Changed += gcnew LMF::Tracker::BasicTypes::DoubleValue::ReadOnlyDoubleValue::ChangedEventHandler(&OnChanged);
//	GlobalObjects::LMFTracker->MeteoStation->HardwareTemperature->Changed += gcnew LMF::Tracker::BasicTypes::DoubleValue::ReadOnlyDoubleValue::ChangedEventHandler(&OnChanged);

	printf("%s\n", __LINE__);

	cout << "Measurment Humidity: " << data->Humidity->Value << " " << (decode)(data->Humidity->UnitString)
		<< " Pressure: " << data->Pressure->Value << " " << (decode)(data->Pressure->UnitString)
		<< " Temperature: " << data->Temperature->Value << " " << '\370' << (decode)(data->Temperature->UnitString->Substring(1)) << "\n";

	setDoubleParam(L_humidity, data->Humidity->Value);
	setDoubleParam(L_pressure, data->Pressure->Value);
	setDoubleParam(L_ltemperature, data->Temperature->Value);
	setStringParam(L_humidityUnits, (decode)(data->Humidity->UnitString));
	setStringParam(L_pressureUnits, (decode)(data->Pressure->UnitString));
	// The degree symbol from this device is something wierd, so pop it off and replace it with something correct.
//	String^ temptempunits = (decode)(data->Temperature->UnitString);
//	string aa = (decode)(data->Temperature->UnitString);
//	aa[0] = '\370';
//std::cout << aa;
// prints correctly but is not displayed correctly in medm/caqtdm, so just pop off the degree symbol
	printf("%s\n", __LINE__);
	setStringParam(L_ltemperatureUnits, (decode)(data->Temperature->UnitString->Substring(1)));

	StationaryMeasurement3D^ stationaryMeas3D = dynamic_cast<StationaryMeasurement3D^>(data);
	cout << " X = " << stationaryMeas3D->Position->Coordinate1->Value << " " << (decode)(stationaryMeas3D->Position->Coordinate1->UnitString);
	cout << " Y = " << stationaryMeas3D->Position->Coordinate2->Value << " " << (decode)(stationaryMeas3D->Position->Coordinate2->UnitString);
	cout << " Z = " << stationaryMeas3D->Position->Coordinate3->Value << " " << (decode)(stationaryMeas3D->Position->Coordinate3->UnitString) << "\n";

	setDoubleParam(L_x, stationaryMeas3D->Position->Coordinate1->Value);
	setDoubleParam(L_y, stationaryMeas3D->Position->Coordinate2->Value);
	setDoubleParam(L_z, stationaryMeas3D->Position->Coordinate3->Value);
	setStringParam(L_xUnits, (decode)(stationaryMeas3D->Position->Coordinate1->UnitString));
	setStringParam(L_yUnits, (decode)(stationaryMeas3D->Position->Coordinate2->UnitString));
	setStringParam(L_zUnits, (decode)(stationaryMeas3D->Position->Coordinate3->UnitString));

	// end of test commands 



	callParamCallbacks();
#endif


	epicsAtExit(exitHandler, this);

	// which works . . . . assuming that the system sees a target

	GlobalObjects::LMFTracker->OverviewCamera->Stop();
	GlobalObjects::LMFTracker->OverviewCamera->GetStillImage(LMF::Tracker::Enums::EStillImageMode::Medium);
	GlobalObjects::LMFTracker->OverviewCamera->StartAsync();


	//	GlobalObjects::LMFTracker->OpenTrackerScope();
}


void leica::initializeHardware(const char* portName)
{

	GlobalObjects::con = gcnew Connection();
	try {
		GlobalObjects::LMFTracker = GlobalObjects::con->Connect(marshal_as<String^>(portName));
	}
	catch (LMF::Tracker::ErrorHandling::LmfException^ e)
	{
		std::cout << "Error code: " << e->Number << " " << (decode)(e->Description) << std::endl;;
		//		std::cout << "Hit an exception trying to perform a Connect call, Exiting. \n";

		//		exit(-1);
		std::cout << "No actual Hardware seen . . . . using Simulator \n";
		GlobalObjects::LMFTracker = GlobalObjects::con->Connect("At930Simulator");
	}
	//    closeWindowByTitle("AT930 Simulator 1.8.0.2250"); // up to 1.9.1.11 now
	//	closeWindowByTitle("AT930 Simulator 1.8.0.2250"); // up to 1.9.1.11 now
	// Not sure what a normal laser tracker might show , maybe key on the SDK value?

	//	closeWindowByPartialTitle("Simulator"); //not working, but the above ones did.

	//	closeWindowByPartialTitle("Simulator"); //but works here, so is the windows not always finsihed generating when this happens?

//setup some high level callbacks

	GlobalObjects::LMFTracker->ErrorArrived += gcnew LMF::Tracker::Tracker::ErrorArrivedHandler(&OnErrorArrived);
	GlobalObjects::LMFTracker->InformationArrived += gcnew LMF::Tracker::Tracker::InformationArrivedHandler(&OnInformationArrived);
	GlobalObjects::LMFTracker->WarningArrived += gcnew LMF::Tracker::Tracker::WarningArrivedHandler(&OnWarningArrived);


	//	Now Handled by Do_laser()
	/*
		// laser status

		GlobalObjects::LMFTracker->Laser->IsLaserWarmedUp->Changed += gcnew LMF::Tracker::BasicTypes::BoolValue::ReadOnlyBoolValue::ChangedEventHandler(&OnLaserChanged);
		GlobalObjects::LMFTracker->Laser->IsOn->Changed += gcnew LMF::Tracker::BasicTypes::BoolValue::ReadOnlyBoolValue::ChangedEventHandler(&OnLaserChanged);

		std::cout << "Is Laser Warmed Up: Label: " << (decode)(GlobalObjects::LMFTracker->Laser->IsLaserWarmedUp->Label) << " Value: " << TFS[GlobalObjects::LMFTracker->Laser->IsLaserWarmedUp->Value] << std::endl;
		std::cout << "Is Laser on: " << (decode)(GlobalObjects::LMFTracker->Laser->IsOn->Label) << " Value : " << TFS[GlobalObjects::LMFTracker->Laser->IsOn->Value] << std::endl;

		setIntegerParam(L_IsOnValue, (int)GlobalObjects::LMFTracker->Laser->IsOn->Value);
	//	setIntegerParam(L_laseronoff_command, (int)GlobalObjects::LMFTracker->Laser->IsOn->Value);
		setIntegerParam(L_IsWarmValue, (int)GlobalObjects::LMFTracker->Laser->IsLaserWarmedUp->Value);

		callParamCallbacks();
	*/
	int check = 1;
	while (check) {
		try {
			check = 0;
			GlobalObjects::LMFTracker->Initialize();

		}
		catch (LMF::Tracker::ErrorHandling::LmfException^ e)
		{
			check = 1;
			if (GlobalObjects::LMFTracker->Laser->IsOn->Value == 0) {
				std::cout << "laser is off . . . Attemping Laser On . . . " << std::endl;
				GlobalObjects::LMFTracker->Laser->IsOn->Value = 1;
			}
			std::cout << "Initialization Error Code: " << e->Number << " " << (decode)(e->Description) << std::endl;
			//		std::cout << "Hit an exception trying to perform an Initialize  call \n";
			//		std::cout << "waiting for no Initialze exceptions . . . .\n";
			Sleep(15000); // 15 seconds between test

		}

	}

	/*
	Various things actualyy go easier if there is actually a target that exosts and we are pointing at it. Otherwise, can't create callbacks for various things since
	they trap out, so them might have to check on every call that is made *if* the callbacks have a non NULL value and if they don't attempt to create them again.

	But since I also do not know what commands that I will end up implementing, also don't know how big of a deal this is likley to be.
	*/

	//	GlobalObjects::LMFTracker->PositionTo(true, false, 546, 3059, 690);
	// trying this without an actual visibale target . . . 


//	GlobalObjects::LMFTracker->GoHomePosition();

GlobalObjects::LMFTracker->OverviewCamera->StartAsync();

}


leica::~leica(void)
{

	GlobalObjects::LMFTracker->OverviewCamera->Stop();
	
	GlobalObjects::LMFTracker->Disconnect();

	iocshCmd("stopPVAServer() > nul");
	

}
/** Configuration command, called directly or from iocsh */
extern "C" int leicaConfig(const char* portName, int maxSizeX, int maxSizeY, int dataType,
	int maxBuffers, int maxMemory, int priority, int stackSize)
{
	new leica(portName, maxSizeX, maxSizeY, (NDDataType_t)dataType,
		(maxBuffers < 0) ? 0 : maxBuffers,
		(maxMemory < 0) ? 0 : maxMemory,
		priority, stackSize);
	return(asynSuccess);
}

/** Code for iocsh registration */
static const iocshArg leicaConfigArg0 = { "Port name", iocshArgString };
static const iocshArg leicaConfigArg1 = { "Max X size", iocshArgInt };
static const iocshArg leicaConfigArg2 = { "Max Y size", iocshArgInt };
static const iocshArg leicaConfigArg3 = { "Data type", iocshArgInt };
static const iocshArg leicaConfigArg4 = { "maxBuffers", iocshArgInt };
static const iocshArg leicaConfigArg5 = { "maxMemory", iocshArgInt };
static const iocshArg leicaConfigArg6 = { "priority", iocshArgInt };
static const iocshArg leicaConfigArg7 = { "stackSize", iocshArgInt };
static const iocshArg* const leicaConfigArgs[] = { &leicaConfigArg0,
														  &leicaConfigArg1,
														  &leicaConfigArg2,
														  &leicaConfigArg3,
														  &leicaConfigArg4,
														  &leicaConfigArg5,
														  &leicaConfigArg6,
														  &leicaConfigArg7 };
static const iocshFuncDef configleica = { "leicaConfig", 8, leicaConfigArgs };
static void configleicaCallFunc(const iocshArgBuf* args)
{
	leicaConfig(args[0].sval, args[1].ival, args[2].ival, args[3].ival,
		args[4].ival, args[5].ival, args[6].ival, args[7].ival);
}


static void leicaRegister(void)
{

	iocshRegister(&configleica, configleicaCallFunc);
}

extern "C" {
	epicsExportRegistrar(leicaRegister);
}

//
// Laser Tracker Related Callbacks 
//

void leica::OnLaserChanged(LMF::Tracker::BasicTypes::BoolValue::ReadOnlyBoolValue^ sender, bool paramNewValue)
{
	//	throw gcnew System::NotImplementedException();
//	std::cout << "Callback OnLaserChanged Bool Value changed: " << paramNewValue;
//	std::cout << " Label: " << (decode)(sender->Label) <<
//		" Value: " << TFS[(int)sender->Value] <<
//		std::endl;
	// Now these stupid callbacks get  . . . messy . . . since the "Label", what this value actually means could be *dozens* of values if we used a generic callback
	// as is, we have to choices "Is laser on" and "Is laser warmed up"		

	string str1 = "Is laser on";
	string str2 = "Is laser warmed up";

	if (str1 == (decode)(sender->Label)) {
		leica_->setIntegerParam(leica_->L_IsOnValue, (int)sender->Value);
		leica_->callParamCallbacks();
		if (sender->Value == 0)
		{
			leica_->setIntegerParam(leica_->L_Value_9, 2);
			leica_->callParamCallbacks();

		}
	}
	else if (str2 == (decode)(sender->Label)) {
		leica_->setIntegerParam(leica_->L_IsWarmValue, (int)sender->Value);
		leica_->callParamCallbacks();
		if (sender->Value == 0)
		{
			leica_->setIntegerParam(leica_->L_Value_9, 2);
			leica_->callParamCallbacks();

		}

	}

}

void leica::OnErrorArrived(LMF::Tracker::Tracker^ sender, LMF::Tracker::ErrorHandling::LmfError^ error)
{
	//	throw gcnew System::NotImplementedException();
	std::cout << "Callback OnErrorArrived:" <<
		" Number: " << (int)(error->Number) <<
		" Title : " << (decode)(error->Title) <<
		" Description: " << (decode)(error->Description) <<
		" Solution: " << (decode)(error->Solution) <<
		std::endl;
}


void leica::OnInformationArrived(LMF::Tracker::Tracker^ sender, LMF::Tracker::ErrorHandling::LmfInformation^ paramInfo)
{
	//	throw gcnew System::NotImplementedException();
	std::cout << "Callback OnInformationArrived: " <<
		" Number: " << (int)(paramInfo->Number) <<
		" Title : " << (decode)(paramInfo->Title) <<
		" Description: " << (decode)(paramInfo->Description) <<
		" Solution: " << (decode)(paramInfo->Solution) <<
		std::endl;
}


void leica::OnWarningArrived(LMF::Tracker::Tracker^ sender, LMF::Tracker::ErrorHandling::LmfWarning^ warning)
{
	//	throw gcnew System::NotImplementedException();
	std::cout << "Callback OnWarningArrived:" <<
		" Number: " << (int)(warning->Number) <<
		" Title : " << (decode)(warning->Title) <<
		" Description: " << (decode)(warning->Description) <<
		" Solution: " << (decode)(warning->Solution) <<
		std::endl;
}

void leica::OnChanged(LMF::Tracker::MeasurementStatus::MeasurementStatusValue^ sender, LMF::Tracker::Enums::EMeasurementStatus paramNewValue)
{
	//	throw gcnew System::NotImplementedException();

	const char* EMeasurementStatusStrings[] = { "ReadyToMeasure","MeasurementInProgress","NotReady","Invalid" }; //ok

//	std::cout << "Callback OnMeasurementStatusChanged: ";

	//	if (paramNewValue == EMeasurementStatus::ReadyToMeasure) { std::cout << "Ready To Measure . . . \n"; }
	//	if (paramNewValue == EMeasurementStatus::MeasurementInProgress) { std::cout << "Measurement in Progress . . . \n"; }
	//	if (paramNewValue == EMeasurementStatus::NotReady) { std::cout << "Not Ready . . . \n"; }
	//	if (paramNewValue == EMeasurementStatus::Invalid) { std::cout << "Measurement Status Invalid . . . \n"; }

//	std::cout << EMeasurementStatusStrings[(int)paramNewValue] << std::endl;

	leica_->setIntegerParam(leica_->L_Value_9, (int)paramNewValue);
	leica_->setStringParam(leica_->L_Label_10, EMeasurementStatusStrings[(int)paramNewValue]);
	leica_->callParamCallbacks();


	//
	// This now an issue, since starting to measure *when* it can means that it is then impossible to stop it, especially whe n you want to change the acquire mode
	// Turn this off for now, just to see what, if anything, happens

	if (paramNewValue == EMeasurementStatus::ReadyToMeasure)
	{
		try {
						GlobalObjects::LMFTracker->Measurement->StartMeasurement();
		}
		catch (...)
		{
		}
	}
}


void leica::OnChanged(LMF::Tracker::BasicTypes::BoolValue::ReadOnlyBoolValue^ sender, bool paramNewValue)
{
	//	throw gcnew System::NotImplementedException();

// Don't actually care . . . 
// 
//	std::cout << "Callback OnBool Value changed: " << paramNewValue;
//	std::cout << " Label: " << (decode)(sender->Label) <<
//		" Value: " << TFS[(int)sender->Value] << " (" << (int)sender->Value << ")" <<
//		std::endl;
}


void leica::OnMeasChanged(LMF::Tracker::BasicTypes::BoolValue::ReadOnlyBoolValue^ sender, bool paramNewValue)
{


	//	throw gcnew System::NotImplementedException();
//	std::cout << "Callback OnMeasChanged Bool Value changed: " << paramNewValue;
//	std::cout << " Label: " << (decode)(sender->Label) <<
//		" Value: " << TFS[(int)sender->Value] << " (" << (int)sender->Value << ")" <<
//		std::endl;

	leica_->setIntegerParam(leica_->L_Value_9, (int)sender->Value);
	leica_->callParamCallbacks();
}

int filenamenumber = 0;

 #define BW 1

void leica::OnWPFBitmapImageArrived(LMF::Tracker::OVC::OverviewCamera^ sender, System::Windows::Media::Imaging::BitmapImage^ image, LMF::Tracker::OVC::ATRCoordinateCollection^ atrCoordinates)
{
	//		filenamenumber++;



			// throw gcnew System::NotImplementedException();
			// 
			// 
		//	std::cout << blue << on_white;
//		std::cout << "Callback OnWPFBitmapImageArrived . . . ";
	//	std::cout << atrCoordinates->Count << " Targets seen in Image.";
	//			<< std::endl;
		//	std::cout << reset;
		//std::cout << ".";

//		std::cout << image->Height << " " <<image->Width << std::endl;


	// WARNNGS: while the resolution of the overviewCameras can be set to low, medium, and high, the position in image commands
	// all assume that you are using medium

	leica_->setIntegerParam(leica_->L_Count_3, atrCoordinates->Count);


	LMF::Tracker::OVC::ATRCoordinateCollection^ gettargetdirections = atrCoordinates;

	// this will get over written by real values, if any . . . 

	leica_->setIntegerParam(leica_->L_x1, 0);
	leica_->setIntegerParam(leica_->L_y1, 0);
	leica_->setIntegerParam(leica_->L_x2, 0);
	leica_->setIntegerParam(leica_->L_y2, 0);
	leica_->setIntegerParam(leica_->L_x3, 0);
	leica_->setIntegerParam(leica_->L_y3, 0);
	leica_->setIntegerParam(leica_->L_x4, 0);
	leica_->setIntegerParam(leica_->L_y4, 0);
	leica_->setIntegerParam(leica_->L_x5, 0);
	leica_->setIntegerParam(leica_->L_y5, 0);
	leica_->setIntegerParam(leica_->L_x6, 0);
	leica_->setIntegerParam(leica_->L_y6, 0);

	// And this is a heck of a lot easier to get the coordinates of the trckers in the camera
	for (int i = 0; i < gettargetdirections->Count; i++)
	{
		//		std::cout << std::endl;

		LMF::Tracker::OVC::ATRCoordinate^ thing = gettargetdirections[i];

		// don't print, just do
		/*		Do_SimpleDoubleValue("AngleHz", thing->AngleHz);
				Do_SimpleDoubleValue("AngleVt", thing->AngleVt);

				Do_SimpleDoubleValue("PixelX", thing->PixelX);
				Do_SimpleDoubleValue("PixelY", thing->PixelY);

		*/

		if (i == 0)
		{
			leica_->setIntegerParam(leica_->L_x1, int(thing->PixelX->Value / 2));
			leica_->setIntegerParam(leica_->L_y1, int(thing->PixelY->Value / 2));
		}
		if (i == 1)
		{
			leica_->setIntegerParam(leica_->L_x2, int(thing->PixelX->Value / 2));
			leica_->setIntegerParam(leica_->L_y2, int(thing->PixelY->Value / 2));
		}
		if (i == 2)
		{
			leica_->setIntegerParam(leica_->L_x3, int(thing->PixelX->Value / 2));
			leica_->setIntegerParam(leica_->L_y3, int(thing->PixelY->Value / 2));
		}
		if (i == 3)
		{
			leica_->setIntegerParam(leica_->L_x4, int(thing->PixelX->Value / 2));
			leica_->setIntegerParam(leica_->L_y4, int(thing->PixelY->Value / 2));
		}
		if (i == 4)
		{
			leica_->setIntegerParam(leica_->L_x5, int(thing->PixelX->Value / 2));
			leica_->setIntegerParam(leica_->L_y5, int(thing->PixelY->Value / 2));
		}
		if (i == 5)
		{
			leica_->setIntegerParam(leica_->L_x6, int(thing->PixelX->Value / 2));
			leica_->setIntegerParam(leica_->L_y6, int(thing->PixelY->Value / 2));
		}

	}



	// this is an attempt to get the raw bitmap values and get it into a byte stream that Area Detector expects
	// which is a pain in the . . . 



	//	try
	//	{
	BitmapImage^ bitmapImage = image;
	WriteableBitmap^ writeableBitmap = gcnew WriteableBitmap(bitmapImage);

	cli::array<unsigned char>^ pixelData = gcnew cli::array<unsigned char>(writeableBitmap->PixelWidth * writeableBitmap->PixelHeight * 4);
	writeableBitmap->CopyPixels(pixelData, writeableBitmap->BackBufferStride, 0);

	// This starts to flip this to greyscale - caqtdm minimal image display only understand (at the instant) greyscale
	// It seems as if Phoebus also hates it, and the image mode *has* to be set at design time . . . . reallY?

#ifdef BW
	std::vector<uint8_t> grayscaleData(writeableBitmap->PixelWidth * writeableBitmap->PixelHeight);
	for (int i = 0; i < pixelData->Length; i += 4) {
		float red = static_cast<float>(pixelData[i]);
		float green = static_cast<float>(pixelData[i + 1]);
		float blue = static_cast<float>(pixelData[i + 2]);

		grayscaleData[i / 4] = static_cast<uint8_t>(0.299 * red + 0.587 * green + 0.114 * blue);
	}
	int ndims = 2;
#else
	std::vector<uint8_t> grayscaleData(writeableBitmap->PixelWidth * writeableBitmap->PixelHeight * 4);
	for (int i = 0; i < pixelData->Length; i += 4) {
		//
		// The image using the *expected* values is horribly blue - if I flip R and B colors are about right.
		//
		grayscaleData[(i / 4) * 3 + 2] = pixelData[i + 0]; // Red component
		grayscaleData[(i / 4) * 3 + 1] = pixelData[i + 1]; // Green component
		grayscaleData[(i / 4) * 3 + 0] = pixelData[i + 2]; // Blue component
		grayscaleData[(i / 4) * 3 + 3] = pixelData[i + 3]; // Alpha component

	}
	int ndims = 3;
#endif


	const char* nullTerminatedData = reinterpret_cast<const char*>(grayscaleData.data());

	size_t imageDims[2];

	NDDataType_t  imageDataType;

	NDArrayInfo arrayInfo;
	epicsTimeStamp currentTime;

#ifdef BW
	imageDims[0] = image->PixelWidth;
	imageDims[1] = image->PixelHeight;

#else
	imageDims[1] = image->PixelWidth;
	imageDims[2] = image->PixelHeight;
	imageDims[0] = 3;
#endif

	imageDataType = NDUInt8;
	leica_->setIntegerParam(leica_->NDDataType, NDUInt8);

	/* Update the image */
	/* First release the copy that we held onto last time */
	if (leica_->pArrays[0]) {
		leica_->pArrays[0]->release();
	}



	/* Allocate a new array */
	leica_->pArrays[0] = leica_->pNDArrayPool->alloc(ndims, imageDims, imageDataType, 0, NULL);
	if (leica_->pArrays[0] != NULL) {
		leica_->pImage = leica_->pArrays[0];
		leica_->pImage->getInfo(&arrayInfo);
		// Copy data from the input to the output, correct 
//			printf("%d %d %d %d\n", imageDims[0], imageDims[1], grayscaleData.size());
//			printf("%d %d %d %d\n", arrayInfo.);

#ifdef BW
		memcpy(leica_->pImage->pData, nullTerminatedData, grayscaleData.size());
#else
		memcpy(leica_->pImage->pData, nullTerminatedData, imageDims[1] * imageDims[2] * imageDims[0]);
		NDColorMode_t colorMode = NDColorModeRGB1;

#endif
//std::cout << "After copy data" << std::endl;

		epicsTimeGetCurrent(&currentTime);
		leica_->pImage->timeStamp = currentTime.secPastEpoch + currentTime.nsec / 1.e9;

		leica_->pImage->uniqueId = filenamenumber++;

		leica_->updateTimeStamp(&leica_->pImage->epicsTS);

		/* Get attributes that have been defined for this driver */
		leica_->getAttributes(leica_->pImage->pAttributeList);
#ifdef BW
#else
		leica_->pAttributeList->add("ColorMode", "Color mode", NDAttrInt32, &colorMode);
#endif


		leica_->doCallbacksGenericPointer(leica_->pImage, leica_->NDArrayData, 0);
	}
	leica_->callParamCallbacks();


}




void leica::OnMeasurementArrived(LMF::Tracker::Measurements::MeasurementSettings^ sender, LMF::Tracker::MeasurementResults::MeasurementCollection^ paramMeasurements, LMF::Tracker::ErrorHandling::LmfException^ paramException)
{
	//	throw gcnew System::NotImplementedException();
	//	std::cout << "I am in the On MeasurementArivved Callback . . . " << std::endl;

	double x = 0.0, y = 0.0, z = 0.0;

	LMF::Tracker::MeasurementResults::Measurement^ LastMeasurement = nullptr;

	// throw gcnew System::NotImplementedException();
//	std::cout << blue << on_white;
//	std::cout << "callback Got a Measurement Value . . . \n";

//	std::cout << "counts :" << paramMeasurements->Count << std::endl;

	if (paramMeasurements)
	{
		//        if (paramMeasurements->Count > 0)
		for (int i = 0; i < paramMeasurements->Count; ++i) {
			{
				LastMeasurement = paramMeasurements[i];


				//							std::cout << "Measurment Humidity: " << LastMeasurement->Humidity->Value << " Pressure: " << LastMeasurement->Pressure->Value << " Temperature: " << LastMeasurement->Temperature->Value << std::endl;
				leica_->setDoubleParam(leica_->L_HumidityValue, LastMeasurement->Humidity->Value);
				leica_->setDoubleParam(leica_->L_PressureValue, LastMeasurement->Pressure->Value);
				leica_->setDoubleParam(leica_->L_TemperatureValue, LastMeasurement->Temperature->Value);

				//
				// This has additional readout fields that I never implemented, since originally I pulled data from the parameters in the LaserTracker classes, and not from method return values
				// or callbacks at all. The current values are in user, so some room to add  in everything, although I might want to store these all as something else, since it can get . . .wordy
				// 
				//
				if (StationaryMeasurement3D^ stationaryMeas3D = dynamic_cast<StationaryMeasurement3D^>(LastMeasurement))
				{
					//											std::cout << "I am a stationary3d measurement \n";

															//	std::cout << " X = " << stationaryMeas3D->Position->Coordinate1->Value << " " << (decode)(stationaryMeas3D->Position->Coordinate1->UnitString);
															//	std::cout << " Y = " << stationaryMeas3D->Position->Coordinate2->Value << " " << (decode)(stationaryMeas3D->Position->Coordinate2->UnitString);
															//	std::cout << " Z = " << stationaryMeas3D->Position->Coordinate3->Value << " " << (decode)(stationaryMeas3D->Position->Coordinate3->UnitString) << std::endl;
															//	std::cout << " Precision = " << stationaryMeas3D->Position->Precision->Value << " " << (decode)(stationaryMeas3D->Position->Precision->UnitString) << std::endl;


					x = stationaryMeas3D->Position->Coordinate1->Value;
					y = stationaryMeas3D->Position->Coordinate2->Value;
					z = stationaryMeas3D->Position->Coordinate3->Value;

					DateTime^ dt;
					dt = stationaryMeas3D->TimeStamp;
					dt = dt->ToLocalTime();
					DateTime^ dt1;
					dt1 = stationaryMeas3D->TimeStampExternal;
					dt1 = dt1->ToLocalTime();

					//			std::cout << "MeasurementProfile: " << (decode)(stationaryMeas3D->MeasurementProfile) <<
					//				" TimeStamp: " << (decode)(dt->ToString("dddd, dd. MMMM yyyy HH:mm:ss.fff")) << 
					//				" TimeStampExternal: " << (decode)(dt1->ToString("dddd, dd. MMMM yyyy HH:mm:ss.fff")) <<
					//				std::endl;

					leica_->setStringParam(leica_->L_MeasurementProfile_1, (decode)(stationaryMeas3D->MeasurementProfile));
					leica_->setStringParam(leica_->L_TimeStamp_12, (decode)(dt->ToString("dddd, dd. MMMM yyyy HH:mm:ss.fff")));
					leica_->setStringParam(leica_->L_TimeStampExternal, (decode)(dt1->ToString("dddd, dd. MMMM yyyy HH:mm:ss.fff")));

					//			Do_SimpleDoubleValue("Humidity", stationaryMeas3D->Humidity);
					//			Do_SimpleDoubleValue("Pressure", stationaryMeas3D->Pressure);
					//			Do_SimpleDoubleValue("Temperature", stationaryMeas3D->Temperature);

					PV_IT(77, 53, 69, stationaryMeas3D->Humidity)
						PV_IT(78, 54, 70, stationaryMeas3D->Pressure)
						PV_IT(79, 55, 71, stationaryMeas3D->Temperature)

						//			std::cout << "Position CoordinateType: " << (int)(stationaryMeas3D->Position->CoordinateType) << " IsFace2: " << (int)stationaryMeas3D->Position->IsFace2 << std::endl;

						leica_->setIntegerParam(leica_->L_CoordinateType_4, (int)(stationaryMeas3D->Position->CoordinateType));
					leica_->setIntegerParam(leica_->L_IsFace2_1, (int)stationaryMeas3D->Position->IsFace2);

					//			Do_SimpleDoubleValue("Coordinate1", stationaryMeas3D->Position->Coordinate1);
					//			Do_SimpleDoubleValue("Precision", stationaryMeas3D->Position->Coordinate1->Precision);
					//			Do_SimpleDoubleValue("Coordinate2", stationaryMeas3D->Position->Coordinate2);
					//			Do_SimpleDoubleValue("Precision", stationaryMeas3D->Position->Coordinate2->Precision);	
					//			Do_SimpleDoubleValue("Coordinate3", stationaryMeas3D->Position->Coordinate3);
					//			Do_SimpleDoubleValue("Precision", stationaryMeas3D->Position->Coordinate3->Precision);

					PV_IT(82, 58, 74, stationaryMeas3D->Position->Coordinate1)
						PV_IT(83, 59, 75, stationaryMeas3D->Position->Coordinate1->Precision)
						PV_IT(84, 60, 76, stationaryMeas3D->Position->Coordinate2)
						PV_IT(85, 61, 77, stationaryMeas3D->Position->Coordinate2->Precision)
						PV_IT(86, 62, 78, stationaryMeas3D->Position->Coordinate3)
						PV_IT(87, 63, 79, stationaryMeas3D->Position->Coordinate3->Precision)

						//			Do_SimpleDoubleValue("Precision", stationaryMeas3D->Position->Precision);
						PV_IT(88, 64, 80, stationaryMeas3D->Position->Precision)

						//			Do_MeasurmentInfo("Info", stationaryMeas3D->Info);

						leica_->setIntegerParam(leica_->L_FirstMeasAfterStartSignal, (int)stationaryMeas3D->Info->FirstMeasAfterStartSignal);
					leica_->setIntegerParam(leica_->L_Flags, (int)stationaryMeas3D->Info->Flags);
					leica_->setIntegerParam(leica_->L_FlagsWithWarnings, (int)stationaryMeas3D->Info->FlagsWithWarnings);
					leica_->setStringParam(leica_->L_GUID_15, (decode)(stationaryMeas3D->Info->GUID));
					leica_->setStringParam(leica_->L_GUIDInfo, (decode)(stationaryMeas3D->Info->GUIDInfo));
					leica_->setIntegerParam(leica_->L_LineIndex, stationaryMeas3D->Info->LineIndex);
					leica_->setIntegerParam(leica_->L_Mode, (int)stationaryMeas3D->Info->Mode);
					leica_->setIntegerParam(leica_->L_MountNr, stationaryMeas3D->Info->MountNr);
					leica_->setIntegerParam(leica_->L_NumberOfLEDsUsed, stationaryMeas3D->Info->NumberOfLEDsUsed);
					leica_->setIntegerParam(leica_->L_NumberOfLEDsVisible, stationaryMeas3D->Info->NumberOfLEDsVisible);
					leica_->setIntegerParam(leica_->L_PointIndex, stationaryMeas3D->Info->PointIndex);
					leica_->setIntegerParam(leica_->L_ProbeFace, stationaryMeas3D->Info->ProbeFace);
					leica_->setStringParam(leica_->L_ProbeSerialNumber, (decode)(stationaryMeas3D->Info->ProbeSerialNumber));
					leica_->setIntegerParam(leica_->L_RegionIndex, stationaryMeas3D->Info->RegionIndex);
					leica_->setIntegerParam(leica_->L_TrackerStatusWord, (int)stationaryMeas3D->Info->TrackerStatusWord);
					leica_->setIntegerParam(leica_->L_Type, (int)stationaryMeas3D->Info->Type);

					PV_IT_5a(80, 15, 56, 72, stationaryMeas3D->Info->DistanceBroadening)
						PV_IT_5a(81, 16, 57, 73, stationaryMeas3D->Info->Intensity)



						// boost all of these into the correct PV's



						//LMF::Tracker::Measurements::MeasurementProfile^ profile = dynamic_cast<LMF::Tracker::Measurements::MeasurementProfile^>(stationaryMeas3D->MeasurementProfile);

						//				if (LMF::Tracker::Measurements::Profiles::StationaryMeasurementProfile^ thisProfile = dynamic_cast<LMF::Tracker::Measurements::Profiles::StationaryMeasurementProfile^>(profile))
						//				{
						//					std::cout << "Label: " << (decode)(thisProfile->Accuracy->Label) << " Value: " << (int) thisProfile->Accuracy->Value << std::endl;
						//				}
						//

											//					leica_->setDoubleParam(leica_->L_x, stationaryMeas3D->Position->Coordinate1->Value);
											//					leica_->setDoubleParam(leica_->L_y, stationaryMeas3D->Position->Coordinate2->Value);
											//					leica_->setDoubleParam(leica_->L_z, stationaryMeas3D->Position->Coordinate3->Value);

						x = stationaryMeas3D->Position->Coordinate1->Value;
					y = stationaryMeas3D->Position->Coordinate2->Value;
					z = stationaryMeas3D->Position->Coordinate3->Value;



				}
				else if (StationaryMeasurement6D^ stationaryMeas6D = dynamic_cast<StationaryMeasurement6D^>(LastMeasurement))
				{
					std::cout << "I am a stationary6d measurement \n";

					//					std::cout << " X = " << stationaryMeas6D->Position->Coordinate1->Value << " " << (decode)(stationaryMeas6D->Position->Coordinate1->UnitString);
					//					std::cout << " Y = " << stationaryMeas6D->Position->Coordinate2->Value << " " << (decode)(stationaryMeas6D->Position->Coordinate2->UnitString);
					//					std::cout << " Z = " << stationaryMeas6D->Position->Coordinate3->Value << " " << (decode)(stationaryMeas6D->Position->Coordinate3->UnitString) << std::endl;

//					leica_->setDoubleParam(leica_->L_x, stationaryMeas6D->Position->Coordinate1->Value);
//					leica_->setDoubleParam(leica_->L_y, stationaryMeas6D->Position->Coordinate2->Value);
//					leica_->setDoubleParam(leica_->L_z, stationaryMeas6D->Position->Coordinate3->Value);

					x = stationaryMeas6D->Position->Coordinate1->Value;
					y = stationaryMeas6D->Position->Coordinate2->Value;
					z = stationaryMeas6D->Position->Coordinate3->Value;

				}
				else if (SingleShotMeasurement3D^ singleshot3dD = dynamic_cast<SingleShotMeasurement3D^>(LastMeasurement))
				{

					//							std::cout << "I am a singleshot 3d measurement \n";

						//						std::cout << " X = " << singleshot3dD->Position->Coordinate1->Value << " " << (decode)(singleshot3dD->Position->Coordinate1->UnitString);
						//						std::cout << " Y = " << singleshot3dD->Position->Coordinate2->Value << " " << (decode)(singleshot3dD->Position->Coordinate2->UnitString);
						//						std::cout << " Z = " << singleshot3dD->Position->Coordinate3->Value << " " << (decode)(singleshot3dD->Position->Coordinate3->UnitString) << std::endl;


						//					    leica_->setDoubleParam(leica_->L_x, singleshot3dD->Position->Coordinate1->Value);
						//						leica_->setDoubleParam(leica_->L_y, singleshot3dD->Position->Coordinate2->Value);
						//						leica_->setDoubleParam(leica_->L_z, singleshot3dD->Position->Coordinate3->Value);

					x = singleshot3dD->Position->Coordinate1->Value;
					y = singleshot3dD->Position->Coordinate2->Value;
					z = singleshot3dD->Position->Coordinate3->Value;



					DateTime^ dt;
					dt = singleshot3dD->TimeStamp;
					dt = dt->ToLocalTime();
					DateTime^ dt1;
					dt1 = singleshot3dD->TimeStampExternal;
					dt1 = dt1->ToLocalTime();

					//		std::cout << "MeasurementProfile: " << (decode)(singleshot3dD->MeasurementProfile) <<
					//			" TimeStamp: " << (decode)(dt->ToString("dddd, dd. MMMM yyyy HH:mm:ss.fff")) <<
					//			" TimeStampExternal: " << (decode)(dt1->ToString("dddd, dd. MMMM yyyy HH:mm:ss.fff")) <<
					//			std::endl;

					leica_->setStringParam(leica_->L_MeasurementProfile_1, (decode)(singleshot3dD->MeasurementProfile));
					leica_->setStringParam(leica_->L_TimeStamp_12, (decode)(dt->ToString("dddd, dd. MMMM yyyy HH:mm:ss.fff")));
					leica_->setStringParam(leica_->L_TimeStampExternal, (decode)(dt1->ToString("dddd, dd. MMMM yyyy HH:mm:ss.fff")));

					//		Do_SimpleDoubleValue("Humidity", singleshot3dD->Humidity);
					//		Do_SimpleDoubleValue("Pressure", singleshot3dD->Pressure);
					//		Do_SimpleDoubleValue("Temperature", singleshot3dD->Temperature);

					PV_IT(77, 53, 69, singleshot3dD->Humidity)
						PV_IT(78, 54, 70, singleshot3dD->Pressure)
						PV_IT(79, 55, 71, singleshot3dD->Temperature)

						//			std::cout << "Position CoordinateType: " << (int)(singleshot3dD->Position->CoordinateType) << " IsFace2: " << (int)singleshot3dD->Position->IsFace2 << std::endl;

						leica_->setIntegerParam(leica_->L_CoordinateType_4, (int)(singleshot3dD->Position->CoordinateType));
					leica_->setIntegerParam(leica_->L_IsFace2_1, (int)singleshot3dD->Position->IsFace2);

					//		Do_SimpleDoubleValue("Coordinate1", singleshot3dD->Position->Coordinate1);
						//	Do_SimpleDoubleValue("Precision", singleshot3dD->Position->Coordinate1->Precision);
						//	Do_SimpleDoubleValue("Coordinate2", singleshot3dD->Position->Coordinate2);
						//	Do_SimpleDoubleValue("Precision", singleshot3dD->Position->Coordinate2->Precision);
						//	Do_SimpleDoubleValue("Coordinate3", singleshot3dD->Position->Coordinate3);
						//	Do_SimpleDoubleValue("Precision", singleshot3dD->Position->Coordinate3->Precision);




					PV_IT(82, 58, 74, singleshot3dD->Position->Coordinate1)
						//		PV_IT(83, 59, 75, singleshot3dD->Position->Coordinate1->Precision)
						PV_IT(84, 60, 76, singleshot3dD->Position->Coordinate2)
						//		PV_IT(85, 61, 77, singleshot3dD->Position->Coordinate2->Precision)
						PV_IT(86, 62, 78, singleshot3dD->Position->Coordinate3)
						//		PV_IT(87, 63, 79, singleshot3dD->Position->Coordinate3->Precision)

						//		Do_SimpleDoubleValue("Precision", singleshot3dD->Position->Precision);
						//	PV_IT(88, 64, 80, singleshot3dD->Position->Precision)

						//		Do_MeasurmentInfo("Info", singleshot3dD->Info);

						leica_->setIntegerParam(leica_->L_FirstMeasAfterStartSignal, (int)singleshot3dD->Info->FirstMeasAfterStartSignal);
					leica_->setIntegerParam(leica_->L_Flags, (int)singleshot3dD->Info->Flags);
					leica_->setIntegerParam(leica_->L_FlagsWithWarnings, (int)singleshot3dD->Info->FlagsWithWarnings);
					leica_->setStringParam(leica_->L_GUID_15, (decode)(singleshot3dD->Info->GUID));
					leica_->setStringParam(leica_->L_GUIDInfo, (decode)(singleshot3dD->Info->GUIDInfo));
					leica_->setIntegerParam(leica_->L_LineIndex, singleshot3dD->Info->LineIndex);
					leica_->setIntegerParam(leica_->L_Mode, (int)singleshot3dD->Info->Mode);
					leica_->setIntegerParam(leica_->L_MountNr, singleshot3dD->Info->MountNr);
					leica_->setIntegerParam(leica_->L_NumberOfLEDsUsed, singleshot3dD->Info->NumberOfLEDsUsed);
					leica_->setIntegerParam(leica_->L_NumberOfLEDsVisible, singleshot3dD->Info->NumberOfLEDsVisible);
					leica_->setIntegerParam(leica_->L_PointIndex, singleshot3dD->Info->PointIndex);
					leica_->setIntegerParam(leica_->L_ProbeFace, singleshot3dD->Info->ProbeFace);
					leica_->setStringParam(leica_->L_ProbeSerialNumber, (decode)(singleshot3dD->Info->ProbeSerialNumber));
					leica_->setIntegerParam(leica_->L_RegionIndex, singleshot3dD->Info->RegionIndex);
					leica_->setIntegerParam(leica_->L_TrackerStatusWord, (int)singleshot3dD->Info->TrackerStatusWord);
					leica_->setIntegerParam(leica_->L_Type, (int)singleshot3dD->Info->Type);

					PV_IT_5a(80, 15, 56, 72, singleshot3dD->Info->DistanceBroadening)
						PV_IT_5a(81, 16, 57, 73, singleshot3dD->Info->Intensity)

						// no precision fields at all - so have to zero them

						double something = 0.0;

					leica_->setDoubleParam(leica_->L_Value_75, something);
					leica_->setDoubleParam(leica_->L_ValueInBaseUnits_59, something);
					leica_->setDoubleParam(leica_->L_Value_77, something);
					leica_->setDoubleParam(leica_->L_ValueInBaseUnits_61, something);
					leica_->setDoubleParam(leica_->L_Value_79, something);
					leica_->setDoubleParam(leica_->L_ValueInBaseUnits_63, something);

					leica_->setDoubleParam(leica_->L_Value_80, something);
					leica_->setDoubleParam(leica_->L_ValueInBaseUnits_64, something);


				}
				else if (SingleShotMeasurement6D^ singleshot6dD = dynamic_cast<SingleShotMeasurement6D^>(LastMeasurement))
				{
					std::cout << "I am a singleshot 6d measurement \n";

					//					std::cout << " X = " << singleshot6dD->Position->Coordinate1->Value << " " << (decode)(singleshot6dD->Position->Coordinate1->UnitString);
					//					std::cout << " Y = " << singleshot6dD->Position->Coordinate2->Value << " " << (decode)(singleshot6dD->Position->Coordinate2->UnitString);
					//					std::cout << " Z = " << singleshot6dD->Position->Coordinate3->Value << " " << (decode)(singleshot6dD->Position->Coordinate3->UnitString) << std::endl;
//					leica_->setDoubleParam(leica_->L_x, singleshot6dD->Position->Coordinate1->Value);
//					leica_->setDoubleParam(leica_->L_y, singleshot6dD->Position->Coordinate2->Value);
//					leica_->setDoubleParam(leica_->L_z, singleshot6dD->Position->Coordinate3->Value);

					x = singleshot6dD->Position->Coordinate1->Value;
					y = singleshot6dD->Position->Coordinate2->Value;
					z = singleshot6dD->Position->Coordinate3->Value;


				}
			}
		}
	}

	leica_->setDoubleParam(leica_->L_x, x);
	leica_->setDoubleParam(leica_->L_y, y);
	leica_->setDoubleParam(leica_->L_z, z);

	// should we update x0,y0,z0 which indicate the last rough position of the targert?

	double x0, y0, z0;
	leica_->getDoubleParam(leica_->L_x0, &x0);
	leica_->getDoubleParam(leica_->L_y0, &y0);
	leica_->getDoubleParam(leica_->L_z0, &z0);

	//	printf(" % d % d % d % d % d % d \n", int(x), int(y), int(z), int(x0), int(y0), int(z0));

	if ((
		abs(round(x - x0)) +
		abs(round(y - y0)) +
		abs(round(z - z0))
		) > 3.0)
	{
		char bufferx[100];
		char buffery[100];
		char bufferz[100];

//#pragma warning(suppress : 4996)
//		sprintf(bufferx, "dbpf(""433LT:LT1:L_x00"", ""%d"") >nul", int(x));
//#pragma warning(suppress : 4996)
//		sprintf(buffery, "dbpf(""433LT:LT1:L_y00"", ""%d"") >nul", int(y));
//#pragma warning(suppress : 4996)
//		sprintf(bufferz, "dbpf(""433LT:LT1:L_z00"", ""%d"") >nul", int(z));
		
		
#pragma warning(suppress : 4996)
		sprintf(bufferx, "dbpf(""8idtracker:LT1:L_x00"", ""%d"") >nul", int(x));
#pragma warning(suppress : 4996)
		sprintf(buffery, "dbpf(""8idtracker:LT1:L_y00"", ""%d"") >nul", int(y));
#pragma warning(suppress : 4996)
		sprintf(bufferz, "dbpf(""8idtracker:LT1:L_z00"", ""%d"") >nul", int(z));


		iocshCmd(bufferx);
		iocshCmd(buffery);
		iocshCmd(bufferz);

		leica_->setDoubleParam(leica_->L_x0, x);
		leica_->setDoubleParam(leica_->L_y0, y);
		leica_->setDoubleParam(leica_->L_z0, z);

	}




	// HVD info, H and V are directory available, D is math.

	Direction^ dir1 = GlobalObjects::LMFTracker->GetDirection();
	leica_->setDoubleParam(leica_->L_h, dir1->HorizontalAngle->Value);
	leica_->setDoubleParam(leica_->L_v, dir1->VerticalAngle->Value);
	// The laser Tracker itself reorts this back in 3 places, math tends to only give 2 . If this is important, change the default coordinate system as required.
	leica_->setDoubleParam(leica_->L_d, std::sqrt(x * x + y * y + z * z));


	leica_->callParamCallbacks();

	// not sure why This callback has to be restrted while onEnviromnetalValuesChanged does not
// *AND* should probably check if it is even leagal to restart
// *AND* having to put this here means that I also can't actually stop it. Unless I check the epics status to see if it shuld be resytarted or not


// If I see something, always measure

//	GlobalObjects::LMFTracker->Measurement->StartMeasurement();


	//	std::cout << reset;
}







void leica::OnFaceChanged(LMF::Tracker::Face^ sender, LMF::Tracker::Enums::EFace paramNewValue)
{
	//	std::cout << "InOnFaceChanged" << std::endl;

	leica_->setIntegerParam(leica_->L_isFace1, int(paramNewValue));
	leica_->setIntegerParam(leica_->L_Value, int(paramNewValue));
	leica_->callParamCallbacks();
}


void leica::OnFaceChangeFinished(LMF::Tracker::Face^ sender, LMF::Tracker::Enums::EFace paramNewValue, LMF::Tracker::ErrorHandling::LmfException^ ex)
{
	//	std::cout << "InOnFaceChangedFinished" << std::endl;
	leica_->setIntegerParam(leica_->L_isFace1, int(paramNewValue));
	leica_->setIntegerParam(leica_->L_Value, int(paramNewValue));
	leica_->callParamCallbacks();
}

/*
void OnWarmChanged(LMF::Tracker::BasicTypes::BoolValue::ReadOnlyBoolValue^ sender, bool paramNewValue)
{
	std::cout << "Bool Value changed: " << paramNewValue;
	std::cout << " Label: " << (decode)(sender->Label) <<
		" Value: " << sender->Value <<
		std::endl;
	throw gcnew System::NotImplementedException();
}


void OnOnChanged(LMF::Tracker::BasicTypes::BoolValue::ReadOnlyBoolValue^ sender, bool paramNewValue)
{
	std::cout << "Bool Value changed: " << paramNewValue;
	std::cout << " Label: " << (decode)(sender->Label) <<
		" Value: " << sender->Value <<
		std::endl;
	throw gcnew System::NotImplementedException();
}

*/




void leica::OnGetInclinationToGravityFinished(LMF::Tracker::Inclination::InclinationSensor^ sender, LMF::Tracker::Inclination::InclinationToGravity^ paramInclinationToGravity, LMF::Tracker::ErrorHandling::LmfException^ ex)
{
	//	throw gcnew System::NotImplementedException();
	std::cout << "OnGetInclinationToGravityFinished" << std::endl;

	GlobalObjects::LMFTracker->InclinationSensor->InclinedToGravity->Value = true;
	leica_->setIntegerParam(leica_->L_Value_1, 1);
	GlobalObjects::LMFTracker->InclinationSensor->BubbleReadout->StartBubbleReadoutStream();
	GlobalObjects::LMFTracker->InclinationSensor->Monitoring->Active->Value = true;
	leica_->setIntegerParam(leica_->L_Value_2, 1);

	leica_->callParamCallbacks();
}

int callbackcount = 0;

void leica::OnBubbleReadoutArrived(LMF::Tracker::Inclination::InclinationBubbleReadout^ sender, LMF::Tracker::Inclination::BubbleReadoutArrivedEventArgs^ paramBubbleReadout)
{
	//	throw gcnew System::NotImplementedException();
	callbackcount++;
	if (callbackcount < 100)
		return;
	callbackcount = 0;

	// 50 events, is about 2.5 seconds, so try this as 10  seconds, with a special case on startup so I get a test backback during initialization

	DateTime^ dt;
	dt = paramBubbleReadout->TimeStamp;
	// for some reason, callback values seem to be in UTC, so need to correct it back to local time
	dt = dt->ToLocalTime();

	/*
	std::cout << "OnBubbleReadoutArrived" << std::endl;
	std::cout << "InclinationL: " << paramBubbleReadout->InclinationL <<
		" InclinationT: " << paramBubbleReadout->InclinationT <<
		" InValidRange: " << TFS[paramBubbleReadout->InValidRange] <<
		" InWorkingRange: " << TFS[paramBubbleReadout->InWorkingRange] <<
		" TimeStamp: " << (decode)(dt->ToString("dddd, dd. MMMM yyyy HH:mm:ss.fff")) <<
		std::endl;
*/

	leica_->setDoubleParam(leica_->L_InclinationL, paramBubbleReadout->InclinationL);
	leica_->setDoubleParam(leica_->L_InclinationT, paramBubbleReadout->InclinationT);
	leica_->setIntegerParam(leica_->L_InclinationInValidRange, (int)paramBubbleReadout->InValidRange);
	leica_->setIntegerParam(leica_->L_InclinationInWorkingRange, (int)paramBubbleReadout->InWorkingRange);
	leica_->setStringParam(leica_->L_BubbleReadoutTimeStamp, (decode)(dt->ToString("dddd, dd. MMMM yyyy HH:mm:ss.fff")));
	leica_->callParamCallbacks();

}


void leica::OnIncChanged(LMF::Tracker::BasicTypes::BoolValue::ReadOnlyBoolValue^ sender, bool paramNewValue)
{
	//	throw gcnew System::NotImplementedException();

	std::cout << "OnIncChanged: " << (decode)(sender->Label) << " " << TFS[paramNewValue] << std::endl;
}


void leica::OnInclinationChanged(LMF::Tracker::Inclination::InclinationMonitoring^ sender)
{
	//	throw gcnew System::NotImplementedException();
//	std::cout << "OnInclinationChanged: "  << std::endl;

		// there is *ALOT* of things that are returned here
// Active,Current, Interval, Theshold, Threshold Exceeded, WorkingRangeExceeded, WorkingRangeExceededTime	
// most of the time, we probably only worry about X and Y, BUT while I was watching these values when people were walking
// around the lab, their stomping was enough to trigger an Out Of Threshold message, which technically means that we have to
// redo the inclination to gravity again . . .  which is a *long* process, that typically messes up EPICS - all the callbacks . . .
// stop calling back, and the commands also don't process even though the trace code says that it is being hit.

// If this has to be redone, not sure if it can be done in another thread, or if I realistically need to call my startup
// "do_calls" again and redo my callbacks or if effectively need to exit and restart . Nothing is thrown as errors anywhere.


//	std::cout << " X = " << sender->Current->X->Value << " Y = " << sender->Current->Y->Value << std::endl;
//	std::cout << sender->Interval->Value << std::endl;
//	std::cout << sender->Threshold->Value << std::endl;

//	std::cout << std::endl;


// there is *ALOT* of things that are returned here
// Active, Current, Interval, Theshold, Threshold Exceeded, WorkingRangeExceeded, WorkingRangeExceededTime	

//do boolValue is just a label and a value
#define PV_IT_3(a,b,thing) \
leica_->setStringParam(leica_->L_Label_##a, (decode)(##thing->Label)); \
leica_->setIntegerParam(leica_->L_Value_##b, ##thing->Value); 


#define PV_IT_A(a,b,thing) \
leica_->setStringParam(leica_->L_Label_##a, (decode)(##thing->Label)); \
leica_->setStringParam(leica_->L_UnitString, (decode)(##thing->UnitString)); \
leica_->setStringParam(leica_->L_UnitType, EUnitTypeStrings[(int)##thing->UnitType]); \
leica_->setDoubleParam(leica_->L_Value_##b, ##thing->Value); \
leica_->setDoubleParam(leica_->L_ValueInBaseUnits, ##thing->ValueInBaseUnits);


//	Do_BoolValue("", sender->Active);	
	PV_IT_3(1, 2, sender->Active)

		DateTime^ dt;
	dt = sender->Current->TimeStamp;
	dt = dt->ToLocalTime();
	//	std::cout << "Current: TimeStamp: " << (decode)(dt->ToString("dddd, dd. MMMM yyyy HH:mm:ss.fff")) << std::endl;
	leica_->setStringParam(leica_->L_TimeStamp_6, (decode)(dt->ToString("dddd, dd. MMMM yyyy HH:mm:ss.fff")));

	//	Do_SimpleDoubleValue("X", sender->Current->X);
	//	Do_SimpleDoubleValue("Y", sender->Current->Y);

		// should be this, but first use of  unitstring, unittype and valueinbaseunits so pattern is off
		//	PV_IT(a, b, c, sender->Current->X)

	PV_IT_A(2, 3, sender->Current->X)
		PV_IT(3, 1, 4, sender->Current->Y)


		//	Do_ReadOnlyDoubleValue("Interval", sender->Interval);	
		//	Do_ReadOnlyDoubleValue("Theshold", sender->Threshold);

		PV_IT(4, 2, 5, sender->Interval)
		PV_IT(5, 3, 6, sender->Threshold)

		//	Do_BoolValue("", sender->ThresholdExceeded);

		PV_IT_3(6, 7, sender->ThresholdExceeded)

		dt = sender->ThresholdExceededTime;
	dt = dt->ToLocalTime();
	//	std::cout << "ThresholdExceededTime: " << (decode)(dt->ToString("dddd, dd. MMMM yyyy HH:mm:ss.fff")) << std::endl;
	leica_->setStringParam(leica_->L_ThresholdExceededTimel, (decode)(dt->ToString("dddd, dd. MMMM yyyy HH:mm:ss.fff")));

	//	Do_BoolValue("", sender->WorkingRangeExceeded);
	PV_IT_3(7, 8, sender->WorkingRangeExceeded)

		dt = sender->WorkingRangeExceededTime;
	dt = dt->ToLocalTime();
	//	std::cout << "WorkingRangeExceededTime: " << (decode)(dt->ToString("dddd, dd. MMMM yyyy HH:mm:ss.fff")) << std::endl;

	leica_->setStringParam(leica_->L_WorkingRangeExceededTime, (decode)(dt->ToString("dddd, dd. MMMM yyyy HH:mm:ss.fff")));

	//	std::cout << std::endl;

	leica_->callParamCallbacks();


}

void leica::OnBrightnessChanged(LMF::Tracker::BasicTypes::DoubleValue::ReadOnlyDoubleValue^ sender, double paramNewValue)
{
	//	throw gcnew System::NotImplementedException();
	std::cout << "OnBrightnessChanged: " <<
		paramNewValue << std::endl;

}


void leica::OnDialogClosed(LMF::Tracker::OVC::Dialog^ sender)
{
	//	throw gcnew System::NotImplementedException();
	std::cout << "OnDialogClosed" << std::endl;
}

void leica::OnQuickChanged(LMF::Tracker::BasicTypes::BoolValue::ReadOnlyBoolValue^ sender, bool paramNewValue)
{
	//	throw gcnew System::NotImplementedException();
	std::cout << "OnQuickReleaseChanged: " <<
		TFS[paramNewValue] <<
		std::endl;
}


void leica::OnPowerChanged(LMF::Tracker::BasicTypes::EnumTypes::ReadOnlyPowerSourceValue^ sender, LMF::Tracker::Enums::EPowerSource paramNewValue)
{
	//	throw gcnew System::NotImplementedException();
	std::cout << "OnPowerChanged: " <<
		EPowerSourceStrings[int(paramNewValue)] <<
		std::endl;
}


void leica::OnSourceChanged(LMF::Tracker::BasicTypes::DoubleValue::ReadOnlyDoubleValue^ sender, double paramNewValue)
{
	//	throw gcnew System::NotImplementedException();
	std::cout << "OnPowerSourceChanged " << std::endl;
}


void leica::OnPowerLockChanged(LMF::Tracker::BasicTypes::BoolValue::ReadOnlyBoolValue^ sender, bool paramNewValue)
{
	//	throw gcnew System::NotImplementedException();
	std::cout << "OnPowerLockChanged " << std::endl;
}


// These changes are also better reflected somewhere else, it isn't as if these values are flipping non stop, so no great savings
void leica::OnMeteoChanged(LMF::Tracker::BasicTypes::DoubleValue::ReadOnlyDoubleValue^ sender, double paramNewValue)
{
	//	throw gcnew System::NotImplementedException();
	//	std::cout << "OnMeteoChanged (Double) " << 	(decode)(sender->Label) << " " << paramNewValue << std::endl;
}

// No idea what might trigger this - changing the units shows those changes in trackerscope, and the number callbacks reflect the new value
void leica::OnMeteoChanged(LMF::Tracker::Meteo::MeteoSource^ sender, LMF::Tracker::Enums::EMeteoSource paramNewValue)
{
	//	throw gcnew System::NotImplementedException();
	std::cout << "OnMeteoChanged (Enum) " << (decode)(sender->Label) << " " << (int)paramNewValue << std::endl;
}


// No idea what might trigger this - changing the units shows those changes in trackerscope, and the number callbacks reflect the new value
void leica::OnMeteoChanged(LMF::Tracker::BasicTypes::BoolValue::ReadOnlyBoolValue^ sender, bool paramNewValue)
{
	//	throw gcnew System::NotImplementedException();
	std::cout << "OnMeteoChanged (Boolean) " << (decode)(sender->Label) << " " << paramNewValue << std::endl;
}

// Realistically, when any of the 3 values change, this triggers, so this is the one to be updating PV's
void leica::OnEnvironmentalValuesChanged(LMF::Tracker::Meteo::MeteoStation^ sender, double paramTemperature, double paramHumidity, double paramPressure)
{
	//	throw gcnew System::NotImplementedException();

	//	std::cout << "OnEnvironmentalValuesChanged" << 
	//	" Temperature: " << paramTemperature <<  
	//	" Humidity: " << paramHumidity << 
	//	" Pressure: " << paramPressure << 
	//	std::endl;

	leica_->setDoubleParam(leica_->L_TemperatureValue, paramTemperature);
	leica_->setDoubleParam(leica_->L_HumidityValue, paramHumidity);
	leica_->setDoubleParam(leica_->L_PressureValue, paramPressure);

	leica_->callParamCallbacks();
}


void leica::OnMeasChanged(LMF::Tracker::BasicTypes::EnumTypes::AccuracyValue^ sender, LMF::Tracker::Enums::EAccuracy paramNewValue)
{
	//	throw gcnew System::NotImplementedException();
	std::cout << "OnMeasChanged Accuracy: " << (decode)(sender->Label) << " " << (int)paramNewValue << std::endl;
}


void leica::OnTriggeredMeasurementsArrived(LMF::Tracker::Measurements::Profiles::CustomTriggerProfile^ sender, LMF::Tracker::MeasurementResults::TriggeredMeasurementCollection^ paramMeasurements)
{
	//	throw gcnew System::NotImplementedException();
	std::cout << "OnTriggeredMeasurementsArrived: " << " " << paramMeasurements->Count << std::endl;
}


void leica::OnMeasChanged(LMF::Tracker::BasicTypes::EnumTypes::ClockSourceValue^ sender, LMF::Tracker::Enums::EClockSource paramNewValue)
{
	//	throw gcnew System::NotImplementedException();
	std::cout << "OnMeasChanged ClockSource: " << (decode)(sender->Label) << " " << (int)paramNewValue << std::endl;
}


void leica::OnMeasChanged(LMF::Tracker::BasicTypes::EnumTypes::ClockTransmissionValue^ sender, LMF::Tracker::Enums::EClockTransmission paramNewValue)
{
	//	throw gcnew System::NotImplementedException();

	std::cout << "OnMeasChanged ClockTransmission: " << (decode)(sender->Label) << " " << (int)paramNewValue << std::endl;
}


void leica::OnMeasChanged(LMF::Tracker::BasicTypes::EnumTypes::StartStopActiveLevelValue^ sender, LMF::Tracker::Enums::EStartStopActiveLevel paramNewValue)
{
	//	throw gcnew System::NotImplementedException();
	std::cout << "OnMeasChanged StartStopActiveLevel: " << (decode)(sender->Label) << " " << (int)paramNewValue << std::endl;
}


void leica::OnMeasChanged(LMF::Tracker::BasicTypes::EnumTypes::StartStopSourceValue^ sender, LMF::Tracker::Enums::EStartStopSource paramNewValue)
{
	//	throw gcnew System::NotImplementedException();
	std::cout << "OnMeasChanged StartStopSource: " << (decode)(sender->Label) << " " << (int)paramNewValue << std::endl;
}



