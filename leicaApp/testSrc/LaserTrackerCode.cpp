//
// LaserTracker.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "QueryTrackerState.h"


const char* EDisplayUnitSystemStrings[] = { "Metric","Imperial" }; //ok
const char* EPowerSourceStrings[] = { "Mains", "Battery", "PowerOverEthernet", "BatteryProblems" }; //ok
const char* EFaceStrings[] = { "Face1", "Face2" }; //ok
const char* EMeasurementStatusStrings[] = { "ReadyToMeasure","MeasurementInProgress","NotReady","Invalid" }; //ok
const char* EUnitTypeStrings[] = { "None", "Angle", "Humidity", "Length", "Pressure", "Temperature", "Time", "Percent" }; //ok
const char* TFS[] = { "False", "True" }; //ok
const char* EMeteoSourceStrings[] = { "ManualMeteo", "LiveMeteo" };



// helper functions

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


// common printable entity templates ??


void Do_ReadOnlyDoubleValue(String^ Title, LMF::Tracker::BasicTypes::DoubleValue::ReadOnlyDoubleValue^ thing)
{
	thing->Changed += gcnew LMF::Tracker::BasicTypes::DoubleValue::ReadOnlyDoubleValue::ChangedEventHandler(&OnChanged);

	std::cout << (decode)(Title) << ": Label: " << (decode)(thing->Label) <<
		" UnitString: " << (decode)(thing->UnitString) <<
		" UnitType: " << EUnitTypeStrings[(int)thing->UnitType] <<
		" Value: " << thing->Value <<
		" ValueInBaseUnits: " << thing->ValueInBaseUnits <<
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
	thing->Changed += gcnew LMF::Tracker::BasicTypes::DoubleValue::ReadOnlyDoubleValue::ChangedEventHandler(&OnChanged);

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
	thing->Changed += gcnew LMF::Tracker::BasicTypes::IntValue::ReadOnlyIntValue::ChangedEventHandler(&OnChanged);

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

void Do_SimpleDoubleValue(String^ Title, LMF::Tracker::BasicTypes::DoubleValue::SimpleDoubleValue^ thing)
{
	std::cout << (decode)(Title) << " Label: " << (decode)(thing->Label) <<
		" UnitString: " << (decode)(thing->UnitString) <<
		" UnitType: " << EUnitTypeStrings[(int)thing->UnitType] <<
		" Value: " << thing->Value <<
		" ValueInBaseUnits: " << thing->ValueInBaseUnits <<
		std::endl;

}

void Do_Reflector(LMF::Tracker::Targets::Reflectors::Reflector^ thing)
{
	std::cout << "Reflector Comment: " << (decode)(thing->Comment) << std::endl;
	std::cout << "GUID: " << (decode)(thing->GUID) << std::endl;
	std::cout << "IsSelectable: " << TFS[thing->IsSelectable] << std::endl;
	std::cout << "Name: " << (decode)(thing->Name) << std::endl;
	std::cout << "Product Name: " << (decode)(thing->ProductName) << std::endl;

	DateTime^ dt;
	dt = thing->TimeStamp;
	std::cout << "TimeStamp: " << (decode)(dt->ToString("dddd, dd. MMMM yyyy HH:mm:ss.fff")) << std::endl;

	Do_ReadOnlyDoubleValue("ADMOffset:", thing->ADMOffset);
	Do_ReadOnlyDoubleValue("SurfaceOffset:", thing->SurfaceOffset);

	thing->ADMOffset->Changed += gcnew LMF::Tracker::BasicTypes::DoubleValue::ReadOnlyDoubleValue::ChangedEventHandler(&OnChanged);
	thing->SurfaceOffset->Changed += gcnew LMF::Tracker::BasicTypes::DoubleValue::ReadOnlyDoubleValue::ChangedEventHandler(&OnChanged);

	std::cout << std::endl;

}
void Do_GenericTarget(LMF::Tracker::Targets::Target^ thing)
{
	std::cout << "***WARNING*** This should resolve to something besides the base class to export all values . . . " << std::endl;
	std::cout << "Target Comment: " << (decode)(thing->Comment) << std::endl;
	std::cout << "GUID: " << (decode)(thing->GUID) << std::endl;
	std::cout << "IsSelectable: " << TFS[thing->IsSelectable] << std::endl;
	std::cout << "Name: " << (decode)(thing->Name) << std::endl;
	std::cout << "Product Name: " << (decode)(thing->ProductName) << std::endl;

	DateTime^ dt;
	dt = thing->TimeStamp;
	std::cout << "TimeStamp: " << (decode)(dt->ToString("dddd, dd. MMMM yyyy HH:mm:ss.fff")) << std::endl;

	std::cout << std::endl;

}

void Do_Compensation(LMF::Tracker::Compensations::Compensation^ thing)
{
	DateTime^ dt;

	std::cout << "Comment: " << (decode)(thing->Comment) << " ";
	std::cout << "GUID: " << (decode)(thing->GUID) << " ";
	std::cout << "Name: " << (decode)(thing->Name) << " ";
	dt = thing->TimeStamp;
	std::cout << "TimeStamp: " << (decode)(dt->ToString("dddd, dd. MMMM yyyy HH:mm:ss.fff")) << std::endl;

}

void Do_BoolValue(String^ Title, LMF::Tracker::BasicTypes::BoolValue::ReadOnlyBoolValue^ thing)
{
	thing->Changed += gcnew LMF::Tracker::BasicTypes::BoolValue::ReadOnlyBoolValue::ChangedEventHandler(&OnChanged);
	std::cout << (decode)(Title) << " Label: " << (decode)(thing->Label) <<
		" Value: " << TFS[(int)thing->Value] <<
		std::endl;

}

// others, that I might also have to implement if used by more than 1 thing
/*
LMF.Tracker.BasicTypes.DoubleValue.ReadOnlyDoubleValue
LMF.Tracker.BasicTypes.DoubleValue.DoubleValue
LMF.Tracker.BasicTypes.DoubleValue.DoubleValueWithRange

LMF.Tracker.BasicTypes.BoolValue.BoolValue
LMF.Tracker.BasicTypes.BoolValue.ReadOnlyBoolValue

LMF.Tracker.Inclination.InclinationMeasurement
LMF.Tracker.Meteo.EnvironmentalSensor
LMF.Tracker.Meteo.ManualEnvironmentalSensor
LMF.Tracker.BasicTypes.EnumTypes.ReadOnlyPowerSourceValue
*/


int CheckForErrors(LMF::Tracker::Tracker^ LMFTracker)
{
	// If and when something throws an error, this is how to decode it 
	Int32 ErrorNumber = -1;

	try {
		LmfError^ err = LMFTracker->GetErrorDescription(ErrorNumber);
		if ((err->Number > 0) && (err->Number != 200069)) {
			std::cout << "Is anything throwing an error code?" << std::endl;;
			std::cout << (decode)(err->Description) << " " <<
				err->Number << " " <<
				(decode)(err->Solution) << " " <<
				(decode)(err->Title) << std::endl;
			return err->Number;
		}
	}
	catch (LMF::Tracker::ErrorHandling::LmfException^ e)
	{
		std::cout << "Error exception " << e->Number << " " << (decode)(e->Description) << std::endl;;
		//		std::cout << "Hit an exception trying to decode Check For Errors  \n";
	}

	return ErrorNumber;

}

int  CheckForMeasurementErrors(LMF::Tracker::Tracker^ LMFTracker)
{
	LMF::Tracker::Enums::EMeasurementStatus statusValue = LMFTracker->Measurement->Status->Value;

	std::cout << EMeasurementStatusStrings[(int)statusValue] << ". . ." << std::endl;

	if (statusValue == EMeasurementStatus::NotReady)
	{
		LMF::Tracker::MeasurementStatus::MeasurementPreconditionCollection^ Preconditions = LMFTracker->Measurement->Status->Preconditions;
		for (int i = 0; i < Preconditions->Count; ++i) {
			LMF::Tracker::MeasurementStatus::MeasurementPrecondition^ firstPrecondition = Preconditions[0];
			std::cout << (decode)(firstPrecondition->Title) << " " << (decode)(firstPrecondition->Description) << " " << (decode)(firstPrecondition->Solution) << std::endl;
		}
	}
	return (int)statusValue;

}
void Do_TrackerAlignment(LMF::Tracker::Tracker^ LMFTracker)
{
	std::cout << std::endl;
	std::cout << "TrackerAlignment . . . basically requires real numbers to calculate anything." << std::endl;

// I guess that I need values for parm1 and param2 for any of this stuff to actually work.
// so all of this requires values and math and I don't have

#ifdef HAVE_VALUES 



	LMF::Tracker::TrackerAlignments::AlignmentOrientationInput^ param1;

	LMFTracker->TrackerAlignment->CalculateOrientation(param1);
	std::cout << "paramInput" << std::endl;
	std::cout << "ApproximateAccuryacy: " << param1->ApproximateAccuracy << std::endl;
	std::cout << "FixedAccuryacy: " << param1->FixedAccuracy << std::endl;
	std::cout << "UnknownAccuryacy: " << param1->UnknownAccuracy << std::endl;

	std::cout << "Points: " << param1->Points->Count << std::endl;
	//methods 
	// 
//	param1->Points->AddPoint()
//	param1->Points->AddPointFromMeasurement();

	Do_SimpleDoubleValueWithAccuracy("RotationX:", param1->RotationX);
	Do_SimpleDoubleValueWithAccuracy("RotationY:", param1->RotationY);
	Do_SimpleDoubleValueWithAccuracy("RotationZ:", param1->RotationZ);
	Do_SimpleDoubleValueWithAccuracy("TranslationX:", param1->TranslationX);
	Do_SimpleDoubleValueWithAccuracy("TranslationY:", param1->TranslationY);
	Do_SimpleDoubleValueWithAccuracy("TranslationZ:", param1->TranslationZ);

	// methods - these all take ValueInBaseUnits and AccuracyInBaseUnits
/*
	param->SetRotationX(0.0);
	param->SetRotationY(0,0);
	param->SetRotationZ(0,0);
	param->SetTranslationX(0, 0);
	param->SetTranslationy(0, 0);
	param->SetTranslationz(0, 0);

*/

	LMF::Tracker::TrackerAlignments::AlignmentTransformationInput^ param2;

	LMFTracker->TrackerAlignment->CalculateTransformation(param2);
	std::cout << "paramInput" << std::endl;
	std::cout << "ApproximateAccuryacy: " << param2->ApproximateAccuracy << std::endl;
	std::cout << "FixedAccuryacy: " << param2->FixedAccuracy << std::endl;
	std::cout << "UnknownAccuryacy: " << param2->UnknownAccuracy << std::endl;

	std::cout << "Points: " << param2->Points->Count << std::endl;
	//methods 
	// 
//	param2->Points->AddPoint()
//	param2->Points->AddPointFromMeasurement();

	Do_SimpleDoubleValueWithAccuracy("RotationX:", param2->RotationX);
	Do_SimpleDoubleValueWithAccuracy("RotationY:", param2->RotationY);
	Do_SimpleDoubleValueWithAccuracy("RotationZ:", param2->RotationZ);
	Do_SimpleDoubleValueWithAccuracy("TranslationX:", param2->TranslationX);
	Do_SimpleDoubleValueWithAccuracy("TranslationY:", param2->TranslationY);
	Do_SimpleDoubleValueWithAccuracy("TranslationZ:", param2->TranslationZ);

	// methods - these all take ValueInBaseUnits and AccuracyInBaseUnits
/*
	param2->SetRotationX(0.0);
	param2->SetRotationY(0,0);
	param2->SetRotationZ(0,0);
	param2->SetTranslationX(0, 0);
	param2->SetTranslationy(0, 0);
	param2->SetTranslationz(0, 0);

*/


// methods

//	LMFTracker->TrackerAlignment->CalculateTransformation();

#endif


	std::cout << std::endl;
}

void Do_OverviewCamera(LMF::Tracker::Tracker^ LMFTracker)
{
	std::cout << std::endl;
	std::cout << "OverviewCamera\n";

	LMFTracker->OverviewCamera->ImageArrived += gcnew LMF::Tracker::OVC::OverviewCamera::ImageArrivedHandler(&OnImageArrived);
	LMFTracker->OverviewCamera->WPFBitmapImageArrived += gcnew LMF::Tracker::OVC::OverviewCamera::WPFBitmapImageArrivedHandler(&OnWPFBitmapImageArrived);

	// METHODS

	LMFTracker->OverviewCamera->GetStillImage(LMF::Tracker::Enums::EStillImageMode::High);
	//	LMFTracker->OverviewCamera->MoveToPixel
	//	LMFTracker->OverviewCamera->StartAsync();
	LMFTracker->OverviewCamera->Stop();

	LMFTracker->OverviewCamera->Brightness->Changed += gcnew LMF::Tracker::BasicTypes::DoubleValue::ReadOnlyDoubleValue::ChangedEventHandler(&OnChanged);


	Do_DoubleValueWithRange("Brightness", LMFTracker->OverviewCamera->Brightness);
	Do_DoubleValueWithRange("Constrast", LMFTracker->OverviewCamera->Contrast);

	// more Methods

	//	LMFTracker->OverviewCamera->Dialog->Closed += gcnew LMF::Tracker::OVC::Dialog::ClosedHandler(&OnClosed);


	//	LMFTracker->OverviewCamera->Dialog->Close();
	//	LMFTracker->OverviewCamera->Dialog->Show();
	//	LMFTracker->OverviewCamera->Dialog->ShowDialog();
	//	LMFTracker->OverviewCamera->Dialog->ShowOnProcess();
	//	LMFTracker->OverviewCamera->Dialog->ShowTopmost();



	std::cout << std::endl;
}
void Do_Measurement(LMF::Tracker::Tracker^ LMFTracker)
{
	std::cout << std::endl;
	std::cout << "Measurement\n";

	LMFTracker->Measurement->MeasurementArrived += gcnew LMF::Tracker::Measurements::MeasurementSettings::MeasurementArrivedHandler(&OnMeasurementArrived);

	// Methods
	/*
		LMFTracker->Measurement->GoAndMeasureStationary();
		LMFTracker->Measurement->GoAndMeasureStationaryAsync();
		LMFTracker->Measurement->MeasureStationary()
		LMFTracker->Measurement->StartMeasurement();
		LMFTracker->Measurement->StopMeasurement();
	*/
	LMFTracker->Measurement->MeasurementInProgress->Changed += gcnew LMF::Tracker::BasicTypes::BoolValue::ReadOnlyBoolValue::ChangedEventHandler(&OnChanged);

	Do_BoolValue("MeasurementInProgress", LMFTracker->Measurement->MeasurementInProgress);
	std::cout << "Profiles: Count: " << LMFTracker->Measurement->Profiles->Count << std::endl;

	std::cout << "Try Quick Measurement . . . " << std::endl;

	LMFTracker->Measurement->StartMeasurement();
	Sleep(20);
	LMFTracker->Measurement->StopMeasurement();

/* This is actually important,

	The first option is basically a one-shot
	The second is a periodic one, so potentially a scan forevery.
*/

	for (int i = 0; i < LMFTracker->Measurement->Profiles->Count; i++)
	{
		std::cout << std::endl;
		LMF::Tracker::Measurements::MeasurementProfile^ profile = LMFTracker->Measurement->Profiles[i];

		if (LMF::Tracker::Measurements::Profiles::StationaryMeasurementProfile^ thisProfile = dynamic_cast<LMF::Tracker::Measurements::Profiles::StationaryMeasurementProfile^>(profile))
		{
			thisProfile->Accuracy->Changed += gcnew LMF::Tracker::BasicTypes::EnumTypes::AccuracyValue::ChangedEventHandler(&OnChanged);

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

			Do_IntValueWithRange("PacketRate", thisProfile->PacketRate);
			Do_DoubleValueWithRange("TimeSeparation", thisProfile->TimeSeparation);

			//methods
					//	thisProfile->Select();
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
			thisProfile->TriggeredMeasurementsArrived += gcnew LMF::Tracker::Measurements::Profiles::CustomTriggerProfile::TriggeredMeasurementsArrivedHandler(&OnTriggeredMeasurementsArrived);

			thisProfile->ClockSource->Changed += gcnew LMF::Tracker::BasicTypes::EnumTypes::ClockSourceValue::ChangedEventHandler(&OnChanged);
			thisProfile->ClockTransmission->Changed += gcnew LMF::Tracker::BasicTypes::EnumTypes::ClockTransmissionValue::ChangedEventHandler(&OnChanged);
			thisProfile->StartStopActiveLevel->Changed += gcnew LMF::Tracker::BasicTypes::EnumTypes::StartStopActiveLevelValue::ChangedEventHandler(&OnChanged);
			thisProfile->StartStopSource->Changed += gcnew LMF::Tracker::BasicTypes::EnumTypes::StartStopSourceValue::ChangedEventHandler(&OnChanged);

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
	std::cout << "GUID: " << (decode)(LMFTracker->Measurement->Profiles->Selected->GUID) << std::endl;
	std::cout << "Name: " << (decode)(LMFTracker->Measurement->Profiles->Selected->Name) << std::endl;

	// and again, I have a listed callback that I can not actually reach via normal methods
			// TODO
			// Research how this might work in the current context
			// afx_msg void OnProfileSelectedChanged(IMeasurementProfileCollectionCOM * sender, IMeasurementProfileCOM * profile);

	//		LMF::Tracker::Measurements::MeasurementProfileCollection^ here = dynamic_cast<LMF::Tracker::Measurements::MeasurementProfileCollection^>(LMFTracker->Measurement->Profiles);
	//		here->


	if (LMF::Tracker::Measurements::Profiles::StationaryMeasurementProfile^ thisProfile = dynamic_cast<LMF::Tracker::Measurements::Profiles::StationaryMeasurementProfile^>(LMFTracker->Measurement->Profiles->Selected))
	{
		thisProfile->Accuracy->Changed += gcnew LMF::Tracker::BasicTypes::EnumTypes::AccuracyValue::ChangedEventHandler(&OnChanged);

		std::cout << "StationaryMeasurementProfile" << std::endl;
		const char* EAccuracyStrings[] = { "Precise", "Standard", "Fast" }; //ok

		std::cout << "Accuracy: Label: " << (decode)(thisProfile->Accuracy->Label) <<
			" Value: " << EAccuracyStrings[(int)thisProfile->Accuracy->Value] <<
			std::endl;

		Do_BoolValue("TwoFace", thisProfile->TwoFace);

		//methods
		//	thisProfile->Select();
	}
	else if (LMF::Tracker::Measurements::Profiles::ContinuousTimeProfile^ thisProfile = dynamic_cast<LMF::Tracker::Measurements::Profiles::ContinuousTimeProfile^>(LMFTracker->Measurement->Profiles->Selected))
	{
		std::cout << "ContinuousTimeProfile" << std::endl;
		Do_IntValueWithRange("PacketRate", thisProfile->PacketRate);
		Do_DoubleValueWithRange("TimeSeparation", thisProfile->TimeSeparation);

		//methods
				//	thisProfile->Select();
	}
	else if (LMF::Tracker::Measurements::Profiles::ContinuousDistanceProfile^ thisProfile = dynamic_cast<LMF::Tracker::Measurements::Profiles::ContinuousDistanceProfile^>(LMFTracker->Measurement->Profiles->Selected))
	{
		std::cout << "ContinuousDistanceProfile" << std::endl;
		Do_DoubleValueWithRange("DistanceSeparation", thisProfile->DistanceSeparation);
	}
	else if (LMF::Tracker::Measurements::Profiles::CustomTriggerProfile^ thisProfile = dynamic_cast<LMF::Tracker::Measurements::Profiles::CustomTriggerProfile^>(LMFTracker->Measurement->Profiles->Selected))
	{
		thisProfile->TriggeredMeasurementsArrived += gcnew LMF::Tracker::Measurements::Profiles::CustomTriggerProfile::TriggeredMeasurementsArrivedHandler(&OnTriggeredMeasurementsArrived);

		thisProfile->ClockSource->Changed += gcnew LMF::Tracker::BasicTypes::EnumTypes::ClockSourceValue::ChangedEventHandler(&OnChanged);
		thisProfile->ClockTransmission->Changed += gcnew LMF::Tracker::BasicTypes::EnumTypes::ClockTransmissionValue::ChangedEventHandler(&OnChanged);
		thisProfile->StartStopActiveLevel->Changed += gcnew LMF::Tracker::BasicTypes::EnumTypes::StartStopActiveLevelValue::ChangedEventHandler(&OnChanged);
		thisProfile->StartStopSource->Changed += gcnew LMF::Tracker::BasicTypes::EnumTypes::StartStopSourceValue::ChangedEventHandler(&OnChanged);

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

	std::cout << "Status" << std::endl;
	LMFTracker->Measurement->Status->Changed += gcnew LMF::Tracker::MeasurementStatus::MeasurementStatusValue::ChangedEventHandler(&OnChanged);

	std::cout << "Label: " << (decode)(LMFTracker->Measurement->Status->Label) << std::endl;
	const char* EMeasurementStatusString[] = { "ReadyToMeasure", "MeasurementInProgress","NotReady","Invalid" };
	std::cout << "Value: " << EMeasurementStatusString[(int)(LMFTracker->Measurement->Status->Value)] << std::endl;
	std::cout << "Preconditions: Count: " << LMFTracker->Measurement->Status->Preconditions->Count << std::endl;

	for (int i = 0; i < LMFTracker->Measurement->Status->Preconditions->Count; i++)
	{
		LMF::Tracker::MeasurementStatus::MeasurementPrecondition^ preconditions = LMFTracker->Measurement->Status->Preconditions[i];
		std::cout << "Descriptions: " << (decode)(preconditions->Description) << std::endl;
		std::cout << "Number: " << preconditions->Number << std::endl;
		std::cout << "Solution: " << (decode)(preconditions->Solution) << std::endl;
		std::cout << "Title: " << (decode)(preconditions->Title) << std::endl;
	}



	std::cout << std::endl;
}

void Do_Triggers(LMF::Tracker::Tracker^ LMFTracker)
{
	std::cout << std::endl;
	std::cout << "Triggers\n";

	std::cout << "Triggers: Count: " << LMFTracker->Triggers->Count << std::endl;

	// I should be getting a triggerhappened callback that again, doesn't exist	

/*
* BUT . . . it just might not be anything that this hardware supports. The Csharp code has references to something, but is never actually
* enabled in the test code. manual implies that this might work on 901's, 930's, (T-Probe ???); 500's (B-probe); 40x's (remote)
* and StableProbing - which seems to be collect a point whenever you actually stop moving, without clicking some remote buttom.
*
* *Might* show up somewhere else . . . like in measurements??
*/


// at the instant, I only now see one of these, and also 1 type, so will only code the 1 that I actually see now
// I *should*  just have this being a LMF::Tracker::Triggers basetype and then do a dynamic_cast
// to each of the things that it *might* be.
//
// if everything else is a clue, if and when I get a different one, I will crash and burn on trying to acress something that it can't actually do
// so will then add/correct it then. * *think* that other probes would enable more of these , but I have only seen this single one.

	for (int i = 0; i < LMFTracker->Triggers->Count; i++)
	{
		LMF::Tracker::Triggers::StableProbingTrigger^ trigger = dynamic_cast<LMF::Tracker::Triggers::StableProbingTrigger^> (LMFTracker->Triggers[i]);

		std::cout << "GUID: " << (decode)(trigger->GUID) << std::endl;
		std::cout << "Name: " << (decode)(trigger->Name) << std::endl;
		std::cout << "IsEnabled: Label: " << (decode)(trigger->IsEnabled->Label) << std::endl;
		std::cout << "Value: " << TFS[(int)trigger->IsEnabled->Value] << std::endl;

		trigger->IsEnabled->Changed += gcnew LMF::Tracker::BasicTypes::BoolValue::ReadOnlyBoolValue::ChangedEventHandler(&OnChanged);

		trigger->TriggerRegion->Changed += gcnew LMF::Tracker::BasicTypes::DoubleValue::ReadOnlyDoubleValue::ChangedEventHandler(&OnChanged);
		trigger->TriggerTime->Changed += gcnew LMF::Tracker::BasicTypes::DoubleValue::ReadOnlyDoubleValue::ChangedEventHandler(&OnChanged);

		Do_DoubleValueWithRange("TriggerRegion", trigger->TriggerRegion);
		Do_DoubleValueWithRange("TriggerTime", trigger->TriggerTime);

	}

	std::cout << std::endl;
}

void Do_InclinationSensor(LMF::Tracker::Tracker^ LMFTracker)
{
	std::cout << std::endl;
	std::cout << "InclinationSensor\n";

	LMFTracker->InclinationSensor->GetInclinationToGravityFinished += gcnew LMF::Tracker::Inclination::InclinationSensor::GetInclinationToGravityFinishedHandler(&OnGetInclinationToGravityFinished);

	// methods
	/*
		LMFTracker->InclinationSensor->GetInclinationToGravity();
		LMFTracker->InclinationSensor->GetInclinationToGravityAsync();
		LMFTracker->InclinationSensor->Measure();
	*/

	LMFTracker->InclinationSensor->BubbleReadout->BubbleReadoutArrived += gcnew LMF::Tracker::Inclination::InclinationBubbleReadout::BubbleReadoutArrivedHandler(&OnBubbleReadoutArrived);

	if (LMFTracker->InclinationSensor->InclinedToGravity->Value == false) {
		std::cout << "Attemping InclinationToGravity . . . This is going to take a while . . .  " << std::endl;
		LMFTracker->InclinationSensor->InclinedToGravity->Value = true;
		LMFTracker->InclinationSensor->GetInclinationToGravity();
	}

	LMFTracker->InclinationSensor->Monitoring->Active->Value = true;

	std::cout << "Attemping StartBubbleReadoutStream . . . " << std::endl;

	// This might be about 1/2 event, just doing this to try to capture the event.

	LMFTracker->InclinationSensor->BubbleReadout->StartBubbleReadoutStream();
	Sleep(20);
	LMFTracker->InclinationSensor->BubbleReadout->StopBubbleReadoutStream();

	// and may as well turn , if we can, so that we don't have to try block it at the instant

	try {
		LMFTracker->InclinationSensor->Measure();
	}
	catch (LMF::Tracker::ErrorHandling::LmfException^ e)
	{
		std::cout << "Error exception " << e->Number << " " << (decode)(e->Description) << std::endl;;
		//		std::cout << "Hit an exception trying to decode Check For Errors  \n";
	}




	DateTime^ dt;
	dt = LMFTracker->InclinationSensor->CurrentInclinationToGravity->TimeStamp;
	std::cout << "CurrentInclinationToGravity: TimeStamp: " << (decode)(dt->ToString("dddd, dd. MMMM yyyy HH:mm:ss.fff")) << std::endl;

	Do_SimpleDoubleValue("InclinationRotX", LMFTracker->InclinationSensor->CurrentInclinationToGravity->InclinationRotX);
	Do_SimpleDoubleValue("InclinationRotY", LMFTracker->InclinationSensor->CurrentInclinationToGravity->InclinationRotY);
	Do_BoolValue("", LMFTracker->InclinationSensor->InclinedToGravity);

	LMFTracker->InclinationSensor->Monitoring->InclinationChanged += gcnew LMF::Tracker::Inclination::InclinationMonitoring::InclinationChangedHandler(&OnInclinationChanged);

	dt = LMFTracker->InclinationSensor->Monitoring->ThresholdExceededTime;
	std::cout << "ThresholdExceededTime: " << (decode)(dt->ToString("dddd, dd. MMMM yyyy HH:mm:ss.fff")) << std::endl;

	dt = LMFTracker->InclinationSensor->Monitoring->WorkingRangeExceededTime;
	std::cout << "WorkingRangeExceededTime: " << (decode)(dt->ToString("dddd, dd. MMMM yyyy HH:mm:ss.fff")) << std::endl;

	Do_BoolValue("", LMFTracker->InclinationSensor->Monitoring->Active);

	dt = LMFTracker->InclinationSensor->Monitoring->Current->TimeStamp;
	std::cout << "Current: TimeStamp: " << (decode)(dt->ToString("dddd, dd. MMMM yyyy HH:mm:ss.fff")) << std::endl;

	Do_SimpleDoubleValue("X", LMFTracker->InclinationSensor->Monitoring->Current->X);
	Do_SimpleDoubleValue("Y", LMFTracker->InclinationSensor->Monitoring->Current->X);

	Do_ReadOnlyDoubleValue("Interval", LMFTracker->InclinationSensor->Monitoring->Interval);
	Do_ReadOnlyDoubleValue("Theshold", LMFTracker->InclinationSensor->Monitoring->Threshold);
	Do_BoolValue("", LMFTracker->InclinationSensor->Monitoring->ThresholdExceeded);
	Do_BoolValue("", LMFTracker->InclinationSensor->Monitoring->WorkingRangeExceeded);

	//	LMFTracker->InclinationSensor->Monitoring->Reset();

		// This might be about 1/2 event, just doing this to try to capture the event.

	LMFTracker->InclinationSensor->BubbleReadout->StartBubbleReadoutStream();
	Sleep(20);
	LMFTracker->InclinationSensor->BubbleReadout->StopBubbleReadoutStream();



	std::cout << std::endl;

}

void Do_MeteoStation(LMF::Tracker::Tracker^ LMFTracker)
{
	std::cout << std::endl;
	std::cout << "MeteoStation\n";

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

	std::cout << "HardwareHumidity: Label: " << (decode)(LMFTracker->MeteoStation->HardwareHumidity->Label) <<
		" SerialNumber: " << (decode)(LMFTracker->MeteoStation->HardwareHumidity->SerialNumber) <<
		" UnitString: " << (decode)(LMFTracker->MeteoStation->HardwareHumidity->UnitString) <<
		" UnitType: " << EUnitTypeStrings[(int)LMFTracker->MeteoStation->HardwareHumidity->UnitType] <<
		" Value: " << LMFTracker->MeteoStation->HardwareHumidity->Value <<
		" ValueInBaseUnits: " << LMFTracker->MeteoStation->HardwareHumidity->ValueInBaseUnits <<
		std::endl;

	LMFTracker->MeteoStation->HardwareHumidity->Available->Changed += gcnew LMF::Tracker::BasicTypes::BoolValue::ReadOnlyBoolValue::ChangedEventHandler(&OnChanged);

	std::cout << "Available: " << (decode)(LMFTracker->MeteoStation->HardwareHumidity->Available->Label) <<
		" Value: " << TFS[(int)LMFTracker->MeteoStation->HardwareHumidity->Available->Value] <<
		std::endl;

	std::cout << "HardwarePressure: Label: " << (decode)(LMFTracker->MeteoStation->HardwarePressure->Label) <<
		" SerialNumber: " << (decode)(LMFTracker->MeteoStation->HardwarePressure->SerialNumber) <<
		" UnitString: " << (decode)(LMFTracker->MeteoStation->HardwarePressure->UnitString) <<
		" UnitType: " << EUnitTypeStrings[(int)LMFTracker->MeteoStation->HardwarePressure->UnitType] <<
		" Value: " << LMFTracker->MeteoStation->HardwarePressure->Value <<
		" ValueInBaseUnits: " << LMFTracker->MeteoStation->HardwarePressure->ValueInBaseUnits <<
		std::endl;

	LMFTracker->MeteoStation->HardwarePressure->Available->Changed += gcnew LMF::Tracker::BasicTypes::BoolValue::ReadOnlyBoolValue::ChangedEventHandler(&OnChanged);

	std::cout << "Available: " << (decode)(LMFTracker->MeteoStation->HardwarePressure->Available->Label) <<
		" Value: " << TFS[(int)LMFTracker->MeteoStation->HardwarePressure->Available->Value] <<
		std::endl;


	std::cout << "HardwareTemperature: Label: " << (decode)(LMFTracker->MeteoStation->HardwareTemperature->Label) <<
		" SerialNumber: " << (decode)(LMFTracker->MeteoStation->HardwareTemperature->SerialNumber) <<
		// The degree symbol from this device is something wierd, so pop it off and replace it with something correct.
		" UnitString: " << '\370' << (decode)(LMFTracker->MeteoStation->HardwareTemperature->UnitString->Substring(1)) <<
		" UnitType: " << EUnitTypeStrings[(int)LMFTracker->MeteoStation->HardwareTemperature->UnitType] <<
		" Value: " << LMFTracker->MeteoStation->HardwareTemperature->Value <<
		" ValueInBaseUnits: " << LMFTracker->MeteoStation->HardwareTemperature->ValueInBaseUnits <<
		std::endl;

	LMFTracker->MeteoStation->HardwareTemperature->Available->Changed += gcnew LMF::Tracker::BasicTypes::BoolValue::ReadOnlyBoolValue::ChangedEventHandler(&OnChanged);

	std::cout << "Available: " << (decode)(LMFTracker->MeteoStation->HardwareTemperature->Available->Label) <<
		" Value: " << TFS[(int)LMFTracker->MeteoStation->HardwareTemperature->Available->Value] <<
		std::endl;

	LMFTracker->MeteoStation->IsAirSensorConnected->Changed += gcnew LMF::Tracker::BasicTypes::BoolValue::ReadOnlyBoolValue::ChangedEventHandler(&OnChanged);
	std::cout << "IsAirSensorConnected: Label: " << (decode)(LMFTracker->MeteoStation->IsAirSensorConnected->Label) <<
		" Value: " << TFS[(int)LMFTracker->MeteoStation->IsAirSensorConnected->Value] <<
		std::endl;

	//
	std::cout << "ManualHumidity: Label: " << (decode)(LMFTracker->MeteoStation->ManualHumidity->Label) <<

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
		std::endl;

	LMFTracker->MeteoStation->ManualHumidity->Available->Changed += gcnew LMF::Tracker::BasicTypes::BoolValue::ReadOnlyBoolValue::ChangedEventHandler(&OnChanged);

	std::cout << "Available: " << (decode)(LMFTracker->MeteoStation->ManualHumidity->Available->Label) <<
		" Value: " << TFS[(int)LMFTracker->MeteoStation->ManualHumidity->Available->Value] <<
		std::endl;

	std::cout << "ManualPressure: Label: " << (decode)(LMFTracker->MeteoStation->ManualPressure->Label) <<

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
		std::endl;

	LMFTracker->MeteoStation->ManualPressure->Available->Changed += gcnew LMF::Tracker::BasicTypes::BoolValue::ReadOnlyBoolValue::ChangedEventHandler(&OnChanged);

	std::cout << "Available: " << (decode)(LMFTracker->MeteoStation->ManualPressure->Available->Label) <<
		" Value: " << TFS[(int)LMFTracker->MeteoStation->ManualPressure->Available->Value] <<
		std::endl;

	std::cout << "ManualTemperature: Label: " << (decode)(LMFTracker->MeteoStation->ManualTemperature->Label) <<

		// extra fields on manual readouts

		" MaxValue: " << LMFTracker->MeteoStation->ManualTemperature->MaxValue <<
		" MaxValueInBaseUnits: " << LMFTracker->MeteoStation->ManualTemperature->MaxValueInBaseUnits <<
		" MinValue: " << LMFTracker->MeteoStation->ManualTemperature->MinValue <<
		" MinValueInBaseUnits: " << LMFTracker->MeteoStation->ManualTemperature->MinValueInBaseUnits <<


		" SerialNumber: " << (decode)(LMFTracker->MeteoStation->ManualTemperature->SerialNumber) <<
		// The degree symbol from this device is something wierd, so pop it off and replace it with something correct.
		" UnitString: " << '\370' << (decode)(LMFTracker->MeteoStation->ManualTemperature->UnitString->Substring(1)) <<
		//		" UnitString: " << (decode)(LMFTracker->MeteoStation->ManualTemperature->UnitString) <<
		" UnitType: " << EUnitTypeStrings[(int)LMFTracker->MeteoStation->ManualTemperature->UnitType] <<
		" Value: " << LMFTracker->MeteoStation->ManualTemperature->Value <<
		" ValueInBaseUnits: " << LMFTracker->MeteoStation->ManualTemperature->ValueInBaseUnits <<
		std::endl;

	LMFTracker->MeteoStation->ManualTemperature->Available->Changed += gcnew LMF::Tracker::BasicTypes::BoolValue::ReadOnlyBoolValue::ChangedEventHandler(&OnChanged);

	std::cout << "Available: " << (decode)(LMFTracker->MeteoStation->ManualTemperature->Available->Label) <<
		" Value: " << TFS[(int)LMFTracker->MeteoStation->ManualTemperature->Available->Value] <<
		std::endl;

	std::cout << "ObjectTemperature: Label: " << (decode)(LMFTracker->MeteoStation->ObjectTemperature->Label) <<
		" SerialNumber: " << (decode)(LMFTracker->MeteoStation->ObjectTemperature->SerialNumber) <<
		// The degree symbol from this device is something wierd, so pop it off and replace it with something correct.
		" UnitString: " << '\370' << (decode)(LMFTracker->MeteoStation->ObjectTemperature->UnitString->Substring(1)) <<
		//		" UnitString: " << (decode)(LMFTracker->MeteoStation->ObjectTemperature->UnitString) <<
		" UnitType: " << EUnitTypeStrings[(int)LMFTracker->MeteoStation->ObjectTemperature->UnitType] <<
		" Value: " << LMFTracker->MeteoStation->ObjectTemperature->Value <<
		" ValueInBaseUnits: " << LMFTracker->MeteoStation->ObjectTemperature->ValueInBaseUnits <<
		std::endl;

	LMFTracker->MeteoStation->ObjectTemperature->Available->Changed += gcnew LMF::Tracker::BasicTypes::BoolValue::ReadOnlyBoolValue::ChangedEventHandler(&OnChanged);

	std::cout << "Available: " << (decode)(LMFTracker->MeteoStation->ObjectTemperature->Available->Label) <<
		" Value: " << TFS[(int)LMFTracker->MeteoStation->ObjectTemperature->Available->Value] <<
		std::endl;

	LMFTracker->MeteoStation->Source->Changed += gcnew LMF::Tracker::Meteo::MeteoSource::ChangedEventHandler(&OnChanged);
	std::cout << "Source: Label: " << (decode)(LMFTracker->MeteoStation->Source->Label) <<
		" Value: " << EMeteoSourceStrings[(int)LMFTracker->MeteoStation->Source->Value] <<
		std::endl;

	std::cout << std::endl;

}

void Do_TargetSearch(LMF::Tracker::Tracker^ LMFTracker)
{
	std::cout << std::endl;
	std::cout << "TargetSearch\n";

	LMFTracker->TargetSearch->Finished += gcnew LMF::Tracker::Targets::TargetSearch::FinishedEventHandler(&OnFinished);

	Do_DoubleValueWithRange("ApproximateDistance", LMFTracker->TargetSearch->ApproximateDistance);
	Do_DoubleValueWithRange("Radius", LMFTracker->TargetSearch->Radius);
	Do_DoubleValueWithRange("TimeOut", LMFTracker->TargetSearch->Timeout);

	// Methods
	/*
		LMFTracker->TargetSearch->Start();
		LMFTracker->TargetSearch->StartAsync();
		LMFTracker->TargetSearch->Stop();
	*/

	std::cout << std::endl;
}


void Do_QuickRelease(LMF::Tracker::Tracker^ LMFTracker)
{
	std::cout << std::endl;
	std::cout << "QuickRelease\n";

	LMFTracker->QuickRelease->Changed += gcnew LMF::Tracker::BasicTypes::BoolValue::ReadOnlyBoolValue::ChangedEventHandler(&OnChanged);

	std::cout << "QuickRelease: Label: " << (decode)(LMFTracker->QuickRelease->Label) << " Value: " << TFS[LMFTracker->QuickRelease->Value] << std::endl;

	std::cout << std::endl;
}

void Do_PowerSource(LMF::Tracker::Tracker^ LMFTracker)
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

	// TODO
	// Most units values are enum values . . . .The string values need to be global assignments, if required
	// I brute forced it in other places, but most probably need to do it right if there is more of this



	LMFTracker->PowerSource->ControllerPowerStatus->RunsOn->Changed += gcnew LMF::Tracker::BasicTypes::EnumTypes::ReadOnlyPowerSourceValue::ChangedEventHandler(&OnChanged);

	Do_ReadOnlyDoubleValue("SensorPowerStatus", LMFTracker->PowerSource->ControllerPowerStatus->Level);

	std::cout << "RunsOn: Label: " << (decode)(LMFTracker->PowerSource->ControllerPowerStatus->RunsOn->Label) <<
		" Value: " << EPowerSourceStrings[(int)LMFTracker->PowerSource->ControllerPowerStatus->RunsOn->Value] <<
		//		" Value: " << (int)LMFTracker->PowerSource->ControllerPowerStatus->RunsOn->Value <<
		std::endl;


	std::cout << std::endl;

}

void Do_PowerLock(LMF::Tracker::Tracker^ LMFTracker)
{
	std::cout << std::endl;
	std::cout << "PowerLock\n";
	LMFTracker->PowerLock->UsePowerLock->Changed += gcnew LMF::Tracker::BasicTypes::BoolValue::ReadOnlyBoolValue::ChangedEventHandler(&OnChanged);
	std::cout << "Use Power Lock: Label: " << (decode)(LMFTracker->PowerLock->UsePowerLock->Label) << " Value: " << TFS[LMFTracker->PowerLock->UsePowerLock->Value] << std::endl;
	LMF::Tracker::OVC::ATRCoordinateCollection^ gettargetdirections = LMFTracker->PowerLock->GetTargetDirections();

	std::cout << "Get Target Directions Count: " << gettargetdirections->Count << std::endl;
	

	// And this is a heck of a lot easier to get the coordinates of the trckers in the camera
	for (int i = 0; i < gettargetdirections->Count; i++)
	{
		std::cout << std::endl;

		LMF::Tracker::OVC::ATRCoordinate^ thing = gettargetdirections[i];

		Do_SimpleDoubleValue("AngleHz", thing->AngleHz);
		Do_SimpleDoubleValue("AngleVt", thing->AngleVt);
		Do_SimpleDoubleValue("PixelX", thing->PixelX);
		Do_SimpleDoubleValue("PixelY", thing->PixelY);
		//		std::cout << std::endl;
	}

	std::cout << std::endl;

}
void Do_Laser(LMF::Tracker::Tracker^ LMFTracker)
{
	std::cout << std::endl;
	std::cout << "Laser\n";

	DateTime^ dt;

	LMFTracker->Laser->IsLaserWarmedUp->Changed += gcnew LMF::Tracker::BasicTypes::BoolValue::ReadOnlyBoolValue::ChangedEventHandler(&OnChanged);
	LMFTracker->Laser->IsOn->Changed += gcnew LMF::Tracker::BasicTypes::BoolValue::ReadOnlyBoolValue::ChangedEventHandler(&OnChanged);

	dt = LMFTracker->Laser->WakeUpTime;

	std::cout << "Wakeup Time: " << (decode)(dt->ToString("dddd, dd. MMMM yyyy HH:mm:ss.fff")) << std::endl;
	std::cout << "Is Laser Warmed Up: Label: " << (decode)(LMFTracker->Laser->IsLaserWarmedUp->Label) << " Value: " << TFS[LMFTracker->Laser->IsLaserWarmedUp->Value] << std::endl;
	std::cout << "Is on: " << (decode)(LMFTracker->Laser->IsOn->Label) << " Value : " << TFS[LMFTracker->Laser->IsOn->Value] << std::endl;

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


void Do_WrtlBoxes(LMF::Tracker::Tracker^ LMFTracker)
{
	std::cout << std::endl;
	// does not seem to be implemented on the 930's . . .	
	std::cout << "WrtlBoxes . . . Not Implemented in this SDK.\n";
	std::cout << std::endl;
}

void Do_Targets(LMF::Tracker::Tracker^ LMFTracker)
{
	std::cout << std::endl;
	std::cout << "Targets\n";

	std::cout << "Target Count: " << LMFTracker->Targets->Count << std::endl << std::endl;

	LMF::Tracker::Targets::TargetCollection^ foundTargets = LMFTracker->Targets;

	// At the instant, I have this just set for what we have, to make programing easier

	for (int i = 0; i < LMFTracker->Targets->Count; i++)
	{

		if (LMF::Tracker::Targets::Reflectors::Reflector^ thisReflector = dynamic_cast<LMF::Tracker::Targets::Reflectors::Reflector^>(foundTargets[i]))
		{
			Do_Reflector(thisReflector);
		}

		else if (LMF::Tracker::Targets::Probes::PassiveProbes::BProbes::BProbe^ thisProbe = dynamic_cast<LMF::Tracker::Targets::Probes::PassiveProbes::BProbes::BProbe^>(foundTargets[i]))
		{

			std::cout << " Probe Comment: " << (decode)(thisProbe->Comment);
			std::cout << " Target Name: " << (decode)(thisProbe->Name);
			std::cout << " Product Name: " << (decode)(thisProbe->ProductName);
			std::cout << " Serial Number: " << (decode)(thisProbe->SerialNumber);
			std::cout << " IsSelectable: " << TFS[thisProbe->IsSelectable] << std::endl;
			// I should be able to 'see' FaceCompensation and TipCompensation params if the type and/or cast is correct.
			std::cout << "     FaceCompensation: " << (decode)(thisProbe->FaceCompensation->Comment);
			std::cout << " Label: " << (decode)(thisProbe->FaceCompensation->IsCompensated->Label);
			std::cout << "IsCompensated: " << TFS[(int)thisProbe->FaceCompensation->IsCompensated->Value];
			std::cout << " Name: " << (decode)(thisProbe->FaceCompensation->Name);
			std::cout << " Product Name: " << (decode)(thisProbe->FaceCompensation->ProductName) << std::endl;

			std::cout << "     TipCompensation BallRadius: " << thisProbe->TipCompensation->BallRadius;
			std::cout << " Comment: " << (decode)(thisProbe->TipCompensation->Comment);
			std::cout << "Label: " << (decode)(thisProbe->TipCompensation->IsCompensated->Label);
			std::cout << "IsComensated: " << TFS[(int)thisProbe->TipCompensation->IsCompensated->Value];
			std::cout << " Length: " << thisProbe->TipCompensation->Length;
			std::cout << " Name: " << (decode)(thisProbe->TipCompensation->Name);
			std::cout << " ProductName: " << (decode)(thisProbe->TipCompensation->ProductName);
			std::cout << " x: " << thisProbe->TipCompensation->X;
			std::cout << " y: " << thisProbe->TipCompensation->Y;
			std::cout << " z: " << thisProbe->TipCompensation->Z << std::endl;

		}
		else if (Target^ thisTarget = foundTargets[i])
		{
			// fall through ?? A target is a reflector that doesn't support ADMOffset and SurfaceOffset
			// hitting this . . .  is technicall an error since this is a base class
			Do_GenericTarget(thisTarget);
		}
	}
	std::cout << std::endl;
}

void Do_Settings(LMF::Tracker::Tracker^ LMFTracker)
{
	std::cout << std::endl;

	std::cout << "Settings\n";

	LMF::Units::ECoordinateType coordtype = LMFTracker->Settings->CoordinateType;
	const char* coordtypeNames[] = { "Spherical", "Cartesian", "Cylindrical" }; //ok
	std::cout << " CoordinateType : " << coordtypeNames[(int)coordtype] << std::endl;

	LMF::Units::ERotationType rottype = LMFTracker->Settings->RotationType;
	const char* rottypeNames[] = { "RotationAngles", "RollPitchYaw", "Quarternion" }; //ok
	std::cout << " RotationType : " << rottypeNames[(int)rottype] << std::endl;

	LMF::Units::EAngleUnit angunit = LMFTracker->Settings->Units->AngleUnit;
	const char* angunitNames[] = { "Radian", "Millirad", "Degree", "Gon", "CC" }; //ok
	std::cout << " AngleUnits : " << angunitNames[(int)angunit] << std::endl;

	LMF::Units::EHumidityUnit humunit = LMFTracker->Settings->Units->HumidityUnit;
	const char* humunitNames[] = { "RelativeHumidity" }; //ok
	std::cout << " HumidityUnits : " << humunitNames[(int)humunit] << std::endl;

	LMF::Units::ELengthUnit lenunit = LMFTracker->Settings->Units->LengthUnit;
	const char* lenunitNames[] = { "Meter", "Millimeter", "Micrometer","Foot","Yard", "Inch" }; //ok
	std::cout << " LengthUnits : " << lenunitNames[(int)lenunit] << std::endl;

	LMF::Units::EPercentUnit perunit = LMFTracker->Settings->Units->PercentUnit;
	const char* perunitNames[] = { "Percent", "None" }; //ok
	std::cout << " PercentUnits : " << perunitNames[(int)perunit] << std::endl;

	LMF::Units::EPressureUnit presunit = LMFTracker->Settings->Units->PressureUnit;
	const char* presunitNames[] = { "mBar", "HPascal","KPascal","MmHg", "Psi", "InH2O","InHg" }; //ok
	std::cout << " PressureUnits : " << presunitNames[(int)presunit] << std::endl;

	LMF::Units::ETemperatureUnit tempunit = LMFTracker->Settings->Units->TemperatureUnit;
	const char* tempunitNames[] = { "Celsius", "Fahrenheit" }; //ok
	std::cout << " TemperatureUnits : " << tempunitNames[(int)tempunit] << std::endl;

	LMF::Units::ETimeUnit timeunit = LMFTracker->Settings->Units->TimeUnit;
	const char* timeunitNames[] = { "Millisecond", "Second", "Minute", "Hour" }; //ok
	std::cout << " TimeUnits : " << timeunitNames[(int)timeunit] << "\n\n";


	std::cout << " Get Orientation \n";
	LMF::Tracker::Alignment^ orient = LMFTracker->Settings->GetOrientation();

	std::cout << "  CoordinateType : " << coordtypeNames[(int)orient->CoordinateType] << std::endl;
	std::cout << "  RotationType : " << rottypeNames[(int)orient->RotationType] << std::endl;
	std::cout << "  Rotation0 : Label: " << (decode)(orient->Rotation0->Label)
		<< "  UnitString: " << (decode)(orient->Rotation0->UnitString) // Note: the marshalling conversion code throws an exception if the starting string is NULL, which it is here
		<< "  Value: " << orient->Rotation0->Value << std::endl;
	std::cout << "  Rotation1 : Label: " << (decode)(orient->Rotation1->Label)
		<< "  UnitString: " << (decode)(orient->Rotation1->UnitString)
		<< "  Value: " << orient->Rotation1->Value << std::endl;
	std::cout << "  Rotation2 : Label: " << (decode)(orient->Rotation2->Label)
		<< "  UnitString: " << (decode)(orient->Rotation2->UnitString)
		<< "  Value: " << orient->Rotation2->Value << std::endl;
	std::cout << "  Rotation3 : Label: " << (decode)(orient->Rotation3->Label)
		<< "  UnitString: " << (decode)(orient->Rotation3->UnitString)
		<< "  Value: " << orient->Rotation3->Value << std::endl;
	std::cout << "  Translation1 : Label: " << (decode)(orient->Translation1->Label)
		<< "  UnitString: " << (decode)(orient->Translation1->UnitString)
		<< "  Value: " << orient->Translation1->Value << std::endl;
	std::cout << "  Translation2 : Label: " << (decode)(orient->Translation2->Label)
		<< "  UnitString: " << (decode)(orient->Translation2->UnitString)
		<< "  Value: " << orient->Translation2->Value << std::endl;
	std::cout << "  Translation3 : Label: " << (decode)(orient->Translation3->Label)
		<< "  UnitString: " << (decode)(orient->Translation3->UnitString)
		<< "  Value: " << orient->Translation3->Value << "\n\n";

	std::cout << " Get Transformation \n";
	LMF::Tracker::AlignmentWithScale^ transf = LMFTracker->Settings->GetTransformation();

	std::cout << "  CoordinateType : " << coordtypeNames[(int)transf->CoordinateType] << std::endl;
	std::cout << "  RotationType : " << rottypeNames[(int)transf->RotationType] << std::endl;
	std::cout << "  Rotation0 : Label: " << (decode)(transf->Rotation0->Label)
		<< "  UnitString: " << (decode)(transf->Rotation0->UnitString)
		<< "  Value: " << transf->Rotation0->Value << std::endl;
	std::cout << "  Rotation1 : Label: " << (decode)(transf->Rotation1->Label)
		<< "  UnitString: " << (decode)(transf->Rotation1->UnitString)
		<< "  Value: " << transf->Rotation1->Value << std::endl;
	std::cout << "  Rotation2 : Label: " << (decode)(transf->Rotation2->Label)
		<< "  UnitString: " << (decode)(transf->Rotation2->UnitString)
		<< "  Value: " << transf->Rotation2->Value << std::endl;
	std::cout << "  Rotation3 : Label: " << (decode)(transf->Rotation3->Label)
		<< "  UnitString: " << (decode)(transf->Rotation3->UnitString)
		<< "  Value: " << transf->Rotation3->Value << std::endl;
	std::cout << "  Scale : Label: " << (decode)(transf->Scale->Label)
		<< "  UnitString: " << (decode)(transf->Scale->UnitString)
		<< "  Value: " << transf->Scale->Value << std::endl;
	std::cout << "  Translation1 : Label: " << (decode)(transf->Translation1->Label)
		<< "  UnitString: " << (decode)(transf->Translation1->UnitString)
		<< "  Value: " << transf->Translation1->Value << std::endl;
	std::cout << "  Translation2 : Label: " << (decode)(transf->Translation2->Label)
		<< "  UnitString: " << (decode)(transf->Translation2->UnitString)
		<< "  Value: " << transf->Translation2->Value << std::endl;
	std::cout << "  Translation3 : Label: " << (decode)(transf->Translation3->Label)
		<< "  UnitString: " << (decode)(transf->Translation3->UnitString)
		<< "  Value: " << transf->Translation3->Value << "\n\n";

	std::cout << std::endl;

}

void Do_Compensations(LMF::Tracker::Tracker^ LMFTracker)
{

	std::cout << std::endl;
	std::cout << "Compensations \n";

	std::cout << "Compensation Count: " << LMFTracker->Compensations->Count << std::endl;

	//	LMF::Tracker::Compensations::CompensationCollection^ me = LMFTracker->Compensations;
	LMF::Tracker::Compensations::CompensationCollection me;







	// there should be 	Changed and SelectedChanged callbacks that Tracker Scope says exists which the compiler says doesn't exist
	// BUT . . . what/who would be changing this except us, so we should already know about that, yes?
	// even the class LMF::Tracker::Compensations::CompensationCollection, which the tlb shows exports theses callbacks, don't actually
	// shows those if just created as a standalone thing.

	//	LMFTracker->Compensations->Changed
	//  LMFTracker->Compensations->SelectedChanged


	for (int i = 0; i < LMFTracker->Compensations->Count; i++)
	{
		Do_Compensation(LMFTracker->Compensations[i]);
	}

	std::cout << "Selected Compensation . . .  \n";

	Do_Compensation(LMFTracker->Compensations->Selected);

	std::cout << std::endl;

}

void Do_Face(LMF::Tracker::Tracker^ LMFTracker)
{
	std::cout << std::endl;
	std::cout << "Face \n";

	//B) List Face

	LMFTracker->Face->Changed += gcnew LMF::Tracker::Face::ChangedHandler(&OnChanged);
	LMFTracker->Face->ChangeFinished += gcnew LMF::Tracker::Face::ChangeFinishedHandler(&OnChangeFinished);

	std::cout << "Face: isface1: " << TFS[LMFTracker->Face->IsFace1] << std::endl;
	std::cout << "Face: Value: " << EFaceStrings[(int)LMFTracker->Face->Value] << std::endl;

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



void OnDisconnected(LMF::Tracker::Tracker^ sender, LMF::Tracker::ErrorHandling::LmfException^ ex)
{
	//   throw gcnew System::NotImplementedException();
	if (ex)
		std::cout << "callback exception code: " << ex->Number << " " << (decode)(ex->Description) << std::endl;
	std::cout << "callback Disconnected finished . . . \n";
}

void OnErrorArrived(LMF::Tracker::Tracker^ sender, LMF::Tracker::ErrorHandling::LmfError^ error)
{
	//   throw gcnew System::NotImplementedException();

	std::cout << "callback OnErrorArivved exception code: " << error->Number << " " << (decode)(error->Description) << std::endl;;
	//	std::cout << "callback Got some sort of error message . . . \n";
}

void OnGetDirectionFinished(LMF::Tracker::Tracker^ sender, LMF::Tracker::Direction^ bm, LMF::Tracker::ErrorHandling::LmfException^ ex)
{
	//   throw gcnew System::NotImplementedException();
	if (ex)
		std::cout << "callback exception " << (decode)(ex->Description);
	//	std::cout << "callback Got some sort of Get Direction finished message . . . \n";

	std::cout << " Direction H Angle: " << bm->HorizontalAngle->Value << " V Angle: " << bm->VerticalAngle->Value << std::endl;


}

void OnGetPrismPositionFinished(LMF::Tracker::Tracker^ sender, LMF::Tracker::MeasurementResults::Measurement^ paramMeasurement, LMF::Tracker::ErrorHandling::LmfException^ ex)
{
	//   throw gcnew System::NotImplementedException();
	if (ex)
		std::cout << "callback exception " << (decode)(ex->Description) << std::endl;;
	std::cout << "callback OnGetPosition Finished . . . \n";

}

void OnGoHomePositionFinished(LMF::Tracker::Tracker^ sender, LMF::Tracker::ErrorHandling::LmfException^ ex)
{
	//   throw gcnew System::NotImplementedException();
	if (ex)
		std::cout << "callback exception " << (decode)(ex->Description) << std::endl;;
	std::cout << "callback Asyn GoHomePosition finished . . . \n";
}

void OnInformationArrived(LMF::Tracker::Tracker^ sender, LMF::Tracker::ErrorHandling::LmfInformation^ paramInfo)
{
	//   throw gcnew System::NotImplementedException();
	std::cout << "OnInformationArrived: " << (decode)(paramInfo->Description) << std::endl;;
	//	std::cout << "callback Got some sort of Information message . . . \n";
}

void OnInitializeFinished(LMF::Tracker::Tracker^ sender, LMF::Tracker::ErrorHandling::LmfException^ ex)
{
	//   throw gcnew System::NotImplementedException();
	if (ex)
		std::cout << "callback exception code: " << ex->Number << " " << (decode)(ex->Description) << std::endl;
	std::cout << "callback Initialization finished . . . \n";

}

void OnPositionToFinished(LMF::Tracker::Tracker^ sender, LMF::Tracker::Targets::Target^ foundTarget, LMF::Tracker::ErrorHandling::LmfException^ ex)
{
	//  throw gcnew System::NotImplementedException();

	std::cout << "callback PositionTo finished . . . ";

	std::cout << " Found a " << (decode)(foundTarget->ProductName) << std::endl;

	// If you determine what sort of target this is, you can pull a bit more information out, but nothing earth shaking.  
	// I.e., position you still need to grab from someone else

	try {
		std::cout << (decode)(ex->Description) << std::endl;;
	}
	catch (...)
	{

	}

}

void OnPositionToTargetFinished(LMF::Tracker::Tracker^ sender, LMF::Tracker::Targets::Target^ foundTarget, LMF::Tracker::ErrorHandling::LmfException^ ex)
{
	//   throw gcnew System::NotImplementedException();
	std::cout << "callback PositionToTarget finished . . . \n";

}

void OnWarningArrived(LMF::Tracker::Tracker^ sender, LMF::Tracker::ErrorHandling::LmfWarning^ warning)
{
	//   throw gcnew System::NotImplementedException();
	std::cout << "callback Warning message: " << warning->Number << " . . . " << decode(warning->Description) << std::endl;


}


void OnMeasurementArrived(LMF::Tracker::Measurements::MeasurementSettings^ sender, LMF::Tracker::MeasurementResults::MeasurementCollection^ paramMeasurements, LMF::Tracker::ErrorHandling::LmfException^ paramException)
{
	LMF::Tracker::MeasurementResults::Measurement^ LastMeasurement = nullptr;

	// throw gcnew System::NotImplementedException();
	std::cout << blue << on_white;
	std::cout << "callback Got a Measurement Value . . . \n";

	std::cout << "counts :" << paramMeasurements->Count << std::endl;

	if (paramMeasurements)
	{
		//        if (paramMeasurements->Count > 0)
		for (int i = 0; i < paramMeasurements->Count; ++i) {
			{
				LastMeasurement = paramMeasurements[i];

				std::cout << "Measurment Humidity: " << LastMeasurement->Humidity->Value << " Pressure: " << LastMeasurement->Pressure->Value << " Temperature: " << LastMeasurement->Temperature->Value << std::endl;


				if (StationaryMeasurement3D^ stationaryMeas3D = dynamic_cast<StationaryMeasurement3D^>(LastMeasurement))
				{
					std::cout << "I am a stationary3d measurement \n";

					std::cout << " X = " << stationaryMeas3D->Position->Coordinate1->Value << " " << (decode)(stationaryMeas3D->Position->Coordinate1->UnitString);
					std::cout << " Y = " << stationaryMeas3D->Position->Coordinate2->Value << " " << (decode)(stationaryMeas3D->Position->Coordinate2->UnitString);
					std::cout << " Z = " << stationaryMeas3D->Position->Coordinate3->Value << " " << (decode)(stationaryMeas3D->Position->Coordinate3->UnitString) << std::endl;



				}
				else if (StationaryMeasurement6D^ stationaryMeas6D = dynamic_cast<StationaryMeasurement6D^>(LastMeasurement))
				{
					std::cout << "I am a stationary6d measurement \n";

					std::cout << " X = " << stationaryMeas6D->Position->Coordinate1->Value << " " << (decode)(stationaryMeas6D->Position->Coordinate1->UnitString);
					std::cout << " Y = " << stationaryMeas6D->Position->Coordinate2->Value << " " << (decode)(stationaryMeas6D->Position->Coordinate2->UnitString);
					std::cout << " Z = " << stationaryMeas6D->Position->Coordinate3->Value << " " << (decode)(stationaryMeas6D->Position->Coordinate3->UnitString) << std::endl;


				}
				else if (SingleShotMeasurement3D^ singleshot3dD = dynamic_cast<SingleShotMeasurement3D^>(LastMeasurement))
				{
					std::cout << "I am a singleshot 3d measurement \n";

					std::cout << " X = " << singleshot3dD->Position->Coordinate1->Value << " " << (decode)(singleshot3dD->Position->Coordinate1->UnitString);
					std::cout << " Y = " << singleshot3dD->Position->Coordinate2->Value << " " << (decode)(singleshot3dD->Position->Coordinate2->UnitString);
					std::cout << " Z = " << singleshot3dD->Position->Coordinate3->Value << " " << (decode)(singleshot3dD->Position->Coordinate3->UnitString) << std::endl;


				}
				else if (SingleShotMeasurement6D^ singleshot6dD = dynamic_cast<SingleShotMeasurement6D^>(LastMeasurement))
				{
					std::cout << "I am a singleshot 6d measurement \n";

					std::cout << " X = " << singleshot6dD->Position->Coordinate1->Value << " " << (decode)(singleshot6dD->Position->Coordinate1->UnitString);
					std::cout << " Y = " << singleshot6dD->Position->Coordinate2->Value << " " << (decode)(singleshot6dD->Position->Coordinate2->UnitString);
					std::cout << " Z = " << singleshot6dD->Position->Coordinate3->Value << " " << (decode)(singleshot6dD->Position->Coordinate3->UnitString) << std::endl;

				}
			}
		}
	}
	std::cout << reset;
}

void OnChanged(LMF::Tracker::MeasurementStatus::MeasurementStatusValue^ sender, LMF::Tracker::Enums::EMeasurementStatus paramNewValue)
{
	std::cout << blue << on_white;
	//    throw gcnew System::NotImplementedException();
	std::cout << "Measurement Status Value changed: " << std::endl;

//	if (paramNewValue == EMeasurementStatus::ReadyToMeasure) { std::cout << "Ready To Measure . . . \n"; }
//	if (paramNewValue == EMeasurementStatus::MeasurementInProgress) { std::cout << "Measurement in Progress . . . \n"; }
//	if (paramNewValue == EMeasurementStatus::NotReady) { std::cout << "Not Ready . . . \n"; }
//	if (paramNewValue == EMeasurementStatus::Invalid) { std::cout << "Measurement Status Invalid . . . \n"; }
		std::cout << EMeasurementStatusStrings[(int)paramNewValue] << std::endl;
	std::cout << reset;

}

//decoding these gets . . .  messy, since dozens of things all normally feed into these, so would have to parse through alot of senders to see what actual field and/or PV 
//we are talking about and might need to be updated. If you want to be more verbose, could make these unique

void OnChanged(LMF::Tracker::BasicTypes::BoolValue::ReadOnlyBoolValue^ sender, bool paramNewValue)
{
	std::cout << blue << on_white;
	std::cout << "Bool Value changed: " << paramNewValue;
	std::cout << " Label: " << (decode)(sender->Label) <<
		" Value: " << sender->Value <<
		std::endl;
	//	throw gcnew System::NotImplementedException();
	std::cout << reset;

}

void OnChanged(LMF::Tracker::BasicTypes::DoubleValue::ReadOnlyDoubleValue^ sender, double paramNewValue)
{
	std::cout << blue << on_white;
	std::cout << blue << on_white;
	std::cout << "Double Value changed: " << paramNewValue;

	//	throw gcnew System::NotImplementedException();
	std::cout << " Label: " << (decode)(sender->Label) <<
		" Value: " << sender->Value <<
		std::endl;
	std::cout << reset;
	std::cout << reset;

}

void OnChanged(LMF::Tracker::Face^ sender, LMF::Tracker::Enums::EFace paramNewValue)
{
	std::cout << blue << on_white;
	std::cout << "Callback Face value changed: " << (int)paramNewValue << std::endl;
	std::cout << reset;

	//	throw gcnew System::NotImplementedException();
}

void OnChangeFinished(LMF::Tracker::Face^ sender, LMF::Tracker::Enums::EFace paramNewValue, LMF::Tracker::ErrorHandling::LmfException^ ex)
{
	std::cout << blue << on_white;
	std::cout << "Callback On Face Change Finished . . . ";
	std::cout << reset;

	//throw gcnew System::NotImplementedException();
}

void OnChanged(LMF::Tracker::BasicTypes::EnumTypes::ReadOnlyPowerSourceValue^ sender, LMF::Tracker::Enums::EPowerSource paramNewValue)
{
	std::cout << blue << on_white;
	std::cout << "Callback PowerSouceEnum value changed: " << TFS[(int)paramNewValue] << std::endl;
	std::cout << reset;

	//	throw gcnew System::NotImplementedException();
}

void OnFinished(LMF::Tracker::Targets::TargetSearch^ sender, LMF::Tracker::Targets::Target^ foundTarget, LMF::Tracker::ErrorHandling::LmfException^ ex)
{
	throw gcnew System::NotImplementedException();
}

void OnEnvironmentalValuesChanged(LMF::Tracker::Meteo::MeteoStation^ sender, double paramTemperature, double paramHumidity, double paramPressure)
{
	std::cout << blue << on_white;
	std::cout << "Callback Environment values changed: " <<
		" Temp: " << paramTemperature <<
		" Humidity: " << paramHumidity <<
		" Pressure: " << paramPressure <<
		std::endl;
	std::cout << reset;

	//	throw gcnew System::NotImplementedException();
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
	std::cout << blue << on_white;
	//	throw gcnew System::NotImplementedException();
	std::cout << "Callback OnBubbleReadoutArrived . . . ";
	std::cout << " InclinationL: " << paramBubbleReadout->InclinationL;
	std::cout << " InclinationT: " << paramBubbleReadout->InclinationT;
	std::cout << " InValidRange: " << TFS[(int)paramBubbleReadout->InValidRange];
	std::cout << " InWorkingRange: " << TFS[(int)paramBubbleReadout->InWorkingRange];
	DateTime^ dt;
	dt = paramBubbleReadout->TimeStamp;
	// for some reason, callback values seem to be in UTC, so need to correct it back to local time
	dt = dt->ToLocalTime();
	std::cout << " TimeStamp: " << (decode)(dt->ToString("dddd, dd. MMMM yyyy HH:mm:ss.fff")) << std::endl;
	std::cout << std::endl;
	std::cout << reset;
}

void OnInclinationChanged(LMF::Tracker::Inclination::InclinationMonitoring^ sender)
{
	//	throw gcnew System::NotImplementedException();
	std::cout << blue << on_white;
	std::cout << "Callback OnInclinationChange  . . . " <<
		" X: " << sender->Current->X->Value <<
		" Y: " << sender->Current->Y->Value <<
		std::endl;


	std::cout << reset;
}

void OnChanged(LMF::Tracker::BasicTypes::EnumTypes::AccuracyValue^ sender, LMF::Tracker::Enums::EAccuracy paramNewValue)
{
	throw gcnew System::NotImplementedException();
}

void OnChanged(LMF::Tracker::BasicTypes::IntValue::ReadOnlyIntValue^ sender, int paramNewValue)
{
	throw gcnew System::NotImplementedException();
}

void OnChanged(LMF::Tracker::BasicTypes::EnumTypes::ClockSourceValue^ sender, LMF::Tracker::Enums::EClockSource paramNewValue)
{
	throw gcnew System::NotImplementedException();
}

void OnChanged(LMF::Tracker::BasicTypes::EnumTypes::ClockTransmissionValue^ sender, LMF::Tracker::Enums::EClockTransmission paramNewValue)
{
	throw gcnew System::NotImplementedException();
}

void OnChanged(LMF::Tracker::BasicTypes::EnumTypes::StartStopActiveLevelValue^ sender, LMF::Tracker::Enums::EStartStopActiveLevel paramNewValue)
{
	throw gcnew System::NotImplementedException();
}

void OnChanged(LMF::Tracker::BasicTypes::EnumTypes::StartStopSourceValue^ sender, LMF::Tracker::Enums::EStartStopSource paramNewValue)
{
	throw gcnew System::NotImplementedException();
}

void OnTriggeredMeasurementsArrived(LMF::Tracker::Measurements::Profiles::CustomTriggerProfile^ sender, LMF::Tracker::MeasurementResults::TriggeredMeasurementCollection^ paramMeasurements)
{
	throw gcnew System::NotImplementedException();
}

void OnClosed(LMF::Tracker::OVC::Dialog^ sender)
{
	throw gcnew System::NotImplementedException();
}


void OnImageArrived(LMF::Tracker::OVC::OverviewCamera^ sender, array<unsigned char, 1>^% image, LMF::Tracker::OVC::ATRCoordinateCollection^ atrCoordinates)
{
	//	throw gcnew System::NotImplementedException();
	std::cout << blue << on_white;
	std::cout << "Callback OnImageArrived . . . ";
	std::cout << atrCoordinates->Count << " Targets seen in Image." << std::endl;
	std::cout << reset;
}


using namespace System::Windows::Media::Imaging;
using namespace System::Windows::Media;
using namespace System::IO;

#using <System.Xaml.dll>

int filenamenumber = 0;

void OnWPFBitmapImageArrived(LMF::Tracker::OVC::OverviewCamera^ sender, System::Windows::Media::Imaging::BitmapImage^ image, LMF::Tracker::OVC::ATRCoordinateCollection^ atrCoordinates)
{
	filenamenumber++;


	// throw gcnew System::NotImplementedException();
	std::cout << blue << on_white;
	std::cout << "Callback OnWPFBitmapImageArrived . . . ";
	std::cout << atrCoordinates->Count << " Targets seen in Image." << std::endl;
	std::cout << reset;

	// Some sort of image should be in Image.
	std::cout << "Height: " << image->Height <<
		" Width: " << image->Width <<
	 std::endl;
//	SaveBitmap(image, "test.png");

//	String^ temp = filenamenumber.ToString();
	FileStream^ fileStream = gcnew FileStream("test" + filenamenumber.ToString() + ".png", FileMode::OpenOrCreate);
	PngBitmapEncoder^ encoder = gcnew PngBitmapEncoder();
	encoder->Frames->Add(BitmapFrame::Create(image));
	encoder->Save(fileStream);
	fileStream->Close();

}
