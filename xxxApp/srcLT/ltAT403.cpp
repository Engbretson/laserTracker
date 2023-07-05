

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <math.h>

#include <epicsTypes.h>
#include <epicsTime.h>
#include <epicsThread.h>
#include <epicsString.h>
#include <epicsTimer.h>
#include <epicsMutex.h>
#include <epicsEvent.h>
#include <iocsh.h>

#include "ADDriver.h"
#include "ltAt403.h"
#include <epicsExport.h>

#include <windows.h>
#pragma comment(lib, "User32.lib")

extern void closeWindowByTitle(const char* title);
extern void closeWindowByPartialTitle(const char* partialTitle);
extern std::string decode(System::String^ something);


static const char* driverName = "LTAt930";


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

void Do_SimpleDoubleValue(String^ Title, LMF::Tracker::BasicTypes::DoubleValue::SimpleDoubleValue^ thing)
{
	std::cout << (decode)(Title) << " Label: " << (decode)(thing->Label) <<
		" UnitString: " << (decode)(thing->UnitString) <<
		" UnitType: " << EUnitTypeStrings[(int)thing->UnitType] <<
		" Value: " << thing->Value <<
		" ValueInBaseUnits: " << thing->ValueInBaseUnits <<
		std::endl;

}


/** Constructor for the LTAt930 class.
  * Calls constructor for the asynPortDriver base class.
  * \param[in] portName The name of the asyn port driver to be created.
  */
//LTAt930::LTAt930(const char* portName)
//	: asynPortDriver(portName,
//		1, /* maxAddr */
//		asynInt32Mask | asynFloat64Mask | asynFloat64ArrayMask | asynEnumMask | asynDrvUserMask | asynOctetMask, /* Interface mask */
//		asynInt32Mask | asynFloat64Mask | asynFloat64ArrayMask | asynEnumMask | asynOctetMask,  /* Interrupt mask */
//		0, /* asynFlags.  This driver does not block and it is not multi-device, so flag is 0 */
//		1, /* Autoconnect */
//		0, /* Default priority */
//		0) /* Default stack size*/

LTAt930::LTAt930(const char* portName, int maxSizeX, int maxSizeY, NDDataType_t dataType,
	int maxBuffers, size_t maxMemory, int priority, int stackSize)

	: ADDriver(portName, 1, 0, maxBuffers, maxMemory,
		0, 0, /* No interfaces beyond those set in ADDriver.cpp */
		0, 1, /* ASYN_CANBLOCK=0, ASYN_MULTIDEVICE=0, autoConnect=1 */
		priority, stackSize)

{
	//    asynStatus status;
	//    int i;
	const char* functionName = "LTAt930";

	LTAt930_ = this;

	//	initializeHardware(portName);

	createParam(L_iscompatibleFirmwareString, asynParamInt32, &L_iscompatibleFirmware);
	createParam(L_faceString, asynParamInt32, &L_face);
	createParam(L_face_commandString, asynParamInt32, &L_face_command);

	createParam(L_islaseronString, asynParamInt32, &L_islaseron);
	createParam(L_islaserwarmString, asynParamInt32, &L_islaserwarm);
	createParam(L_laseronoff_commandString, asynParamInt32, &L_laseronoff_command);

	createParam(L_measonoff_commandString, asynParamInt32, &L_measonoff_command);
	createParam(L_meas_in_progString, asynParamInt32, &L_meas_in_prog);

	createParam(L_commentString, asynParamOctet, &L_comment);
	createParam(L_expectedFirmwareString, asynParamOctet, &L_expectedFirmware);
	createParam(L_installedFirmwareString, asynParamOctet, &L_installedFirmware);
	createParam(L_ipAddressString, asynParamOctet, &L_ipAddress);
	createParam(L_nameString, asynParamOctet, &L_name);
	createParam(L_productNameString, asynParamOctet, &L_productName);
	createParam(L_serialNumberString, asynParamOctet, &L_serialNumber);


	createParam(L_angleUnitsString, asynParamOctet, &L_angleUnits);
	createParam(L_humidityUnitsString, asynParamOctet, &L_humidityUnits);
	createParam(L_pressureUnitsString, asynParamOctet, &L_pressureUnits);
	createParam(L_temperatureUnitsString, asynParamOctet, &L_temperatureUnits);
	createParam(L_xUnitsString, asynParamOctet, &L_xUnits);
	createParam(L_yUnitsString, asynParamOctet, &L_yUnits);
	createParam(L_zUnitsString, asynParamOctet, &L_zUnits);

	createParam(L_horizontalAngleString, asynParamFloat64, &L_horizontalAngle);
	createParam(L_verticalAngleString, asynParamFloat64, &L_verticalAngle);
	createParam(L_humidityString, asynParamFloat64, &L_humidity);
	createParam(L_pressureString, asynParamFloat64, &L_pressure);
	createParam(L_temperatureString, asynParamFloat64, &L_temperature);
	createParam(L_xString, asynParamFloat64, &L_x);
	createParam(L_yString, asynParamFloat64, &L_y);
	createParam(L_zString, asynParamFloat64, &L_z);



	// lets do some test commands 

	initializeHardware(portName);

	printf("\n***********************************\n");
	printf("\nConnected to Laser Tracker, checking default parameters . . . \n");

	String^ Comment = GlobalObjects::LMFTracker->Comment;
	cout << "Comment: " << (decode)(Comment) << "\n";
	setStringParam(L_comment, (decode)(Comment));

	String^ ExpectedFirmware = GlobalObjects::LMFTracker->ExpectedFirmware;
	cout << "Firmware: " << (decode)(ExpectedFirmware) << "\n";
	setStringParam(L_expectedFirmware, (decode)(ExpectedFirmware));

	String^ InstalledFirmware = GlobalObjects::LMFTracker->InstalledFirmware;
	cout << "InstalledFirmware: " << (decode)(InstalledFirmware) << "\n";
	setStringParam(L_installedFirmware, (decode)(InstalledFirmware));

	String^ IP = GlobalObjects::LMFTracker->IPAddress;
	cout << "IP: " << (decode)(IP) << "\n";
	setStringParam(L_ipAddress, (decode)(IP));

	Boolean CompatFirmware = GlobalObjects::LMFTracker->IsCompatibleWithInstalledFirmware;
	cout << "Is Compatible With Installed Firmware: " << CompatFirmware << "\n";
	setIntegerParam(L_iscompatibleFirmware, CompatFirmware);

	String^ Name = GlobalObjects::LMFTracker->Name;
	cout << "Name: " << (decode)(Name) << "\n";
	setStringParam(L_name, (decode)(Name));

	String^ ProductName = GlobalObjects::LMFTracker->ProductName;
	cout << "ProductName: " << (decode)(ProductName) << "\n";
	setStringParam(L_productName, (decode)(ProductName));

	String^ SerialNumber = GlobalObjects::LMFTracker->SerialNumber;
	cout << "Serial: " << (decode)(SerialNumber) << "\n";
	setStringParam(L_serialNumber, (decode)(SerialNumber));

	// none of this is particularly useful to anyone
	// I don't want people flipping any of these, if I can help it

	/*
		cout << "Getting Info from Settings . . .  \n";

		LMF::Units::ECoordinateType coordtype = GlobalObjects::LMFTracker->Settings->CoordinateType;
		const char* coordtypeNames[] = { "Spherical", "Cartesian", "Cylindrical" };
		cout << " CoordinateType : " << coordtypeNames[(int)coordtype] << "\n";

		LMF::Units::ERotationType rottype = GlobalObjects::LMFTracker->Settings->RotationType;
		const char* rottypeNames[] = { "RotationAngles", "RollPitchYaw", "Quarternion" };
		cout << " RotationType : " << rottypeNames[(int)coordtype] << "\n";

		LMF::Units::EAngleUnit angunit = GlobalObjects::LMFTracker->Settings->Units->AngleUnit;
		const char* angunitNames[] = { "Radian", "Millirad", "Degree", "Gon", "CC" };
		cout << " AngleUnits : " << angunitNames[(int)angunit] << "\n";

		LMF::Units::EHumidityUnit humunit = GlobalObjects::LMFTracker->Settings->Units->HumidityUnit;
		const char* humunitNames[] = { "RelativeHumidity" };
		cout << " HumidityUnits : " << humunitNames[(int)humunit] << "\n";

		LMF::Units::ELengthUnit lenunit = GlobalObjects::LMFTracker->Settings->Units->LengthUnit;
		const char* lenunitNames[] = { "Meter", "Millimeter", "Micrometer","Foot","Yard", "Inch" };
		cout << " LengthUnits : " << lenunitNames[(int)humunit] << "\n";

		LMF::Units::EPercentUnit perunit = GlobalObjects::LMFTracker->Settings->Units->PercentUnit;
		const char* perunitNames[] = { "Percent", "None" };
		cout << " PercentUnits : " << perunitNames[(int)perunit] << "\n";

		LMF::Units::EPressureUnit presunit = GlobalObjects::LMFTracker->Settings->Units->PressureUnit;
		const char* presunitNames[] = { "mBar", "HPascal","KPascal","MmHg", "Psi", "InH2O","InHg" };
		cout << " PressureUnits : " << presunitNames[(int)presunit] << "\n";

		LMF::Units::ETemperatureUnit tempunit = GlobalObjects::LMFTracker->Settings->Units->TemperatureUnit;
		const char* tempunitNames[] = { "Celsius", "Fahrenheit" };
		cout << " TemperatureUnits : " << tempunitNames[(int)tempunit] << "\n";

		LMF::Units::ETimeUnit timeunit = GlobalObjects::LMFTracker->Settings->Units->TimeUnit;
		const char* timeunitNames[] = { "Millisecond", "Second", "Minute", "Hour" };
		cout << " TimeUnits : " << timeunitNames[(int)timeunit] << "\n";

	*/
	GlobalObjects::LMFTracker->GetDirectionAsync();
	Direction^ dir1 = GlobalObjects::LMFTracker->GetDirection();

	cout << "Direction H Angle: " << dir1->HorizontalAngle->Value << " " << (decode)(dir1->HorizontalAngle->UnitString)
		<< " V Angle: " << dir1->VerticalAngle->Value << " " << (decode)(dir1->VerticalAngle->UnitString) << "\n";
	setDoubleParam(L_horizontalAngle, dir1->HorizontalAngle->Value);
	setDoubleParam(L_verticalAngle, dir1->VerticalAngle->Value);
	setStringParam(L_angleUnits, (decode)(dir1->VerticalAngle->UnitString));

	// this code you can't actually do unless a tracker actually exists

	GlobalObjects::LMFTracker->Measurement->MeasurementArrived += gcnew LMF::Tracker::Measurements::MeasurementSettings::MeasurementArrivedHandler(&OnMeasurementArrived);
	// also not very helpful
	GlobalObjects::LMFTracker->Measurement->Status->Changed += gcnew LMF::Tracker::MeasurementStatus::MeasurementStatusValue::ChangedEventHandler(&OnChanged);
	//almost too generic to be of much use, I.e., not sure that putting this into a PV would be helpful
	GlobalObjects::LMFTracker->Measurement->MeasurementInProgress->Changed += gcnew LMF::Tracker::BasicTypes::BoolValue::ReadOnlyBoolValue::ChangedEventHandler(&OnMeasChanged);


	LMF::Tracker::MeasurementResults::Measurement^ data = GlobalObjects::LMFTracker->Measurement->MeasureStationary();

	GlobalObjects::LMFTracker->Measurement->StartMeasurement();
	setIntegerParam(L_measonoff_command, 1);
	
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


	// This/these may not actually be required since values also appear from various other sources.

//	GlobalObjects::LMFTracker->MeteoStation->EnvironmentalValuesChanged += gcnew LMF::Tracker::Meteo::MeteoStation::EnvironmentalValuesChangedEventHandler(&OnEnvironmentalValuesChanged);

	// I do not know if this/these are required since  should be handled by the OnEnvironmentalValuesChanged callback but also values come back with each data pointer
//	GlobalObjects::LMFTracker->MeteoStation->HardwareHumidity->Changed += gcnew LMF::Tracker::BasicTypes::DoubleValue::ReadOnlyDoubleValue::ChangedEventHandler(&OnChanged);
//	GlobalObjects::LMFTracker->MeteoStation->HardwarePressure->Changed += gcnew LMF::Tracker::BasicTypes::DoubleValue::ReadOnlyDoubleValue::ChangedEventHandler(&OnChanged);
//	GlobalObjects::LMFTracker->MeteoStation->HardwareTemperature->Changed += gcnew LMF::Tracker::BasicTypes::DoubleValue::ReadOnlyDoubleValue::ChangedEventHandler(&OnChanged);


	cout << "Measurment Humidity: " << data->Humidity->Value << " " << (decode)(data->Humidity->UnitString)
		<< " Pressure: " << data->Pressure->Value << " " << (decode)(data->Pressure->UnitString)
		<< " Temperature: " << data->Temperature->Value << " " << '\370' << (decode)(data->Temperature->UnitString->Substring(1)) << "\n";

	setDoubleParam(L_humidity, data->Humidity->Value);
	setDoubleParam(L_pressure, data->Pressure->Value);
	setDoubleParam(L_temperature, data->Temperature->Value);
	setStringParam(L_humidityUnits, (decode)(data->Humidity->UnitString));
	setStringParam(L_pressureUnits, (decode)(data->Pressure->UnitString));
	// The degree symbol from this device is something wierd, so pop it off and replace it with something correct.
//	String^ temptempunits = (decode)(data->Temperature->UnitString);
//	string aa = (decode)(data->Temperature->UnitString);
//	aa[0] = '\370';
//std::cout << aa;
// prints correctly but is not displayed correctly in medm/caqtdm, so just pop off the degree symbol

	setStringParam(L_temperatureUnits, (decode)(data->Temperature->UnitString->Substring(1)));

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

	printf("\n***********************************\n\n");


	callParamCallbacks();
}



/*
I need the msvc compiler development environment to more easily deal with resolving references to the TBL
I don't expect to actually be able to run this as a binary.
*/
#ifdef DO_MAIN
int main(void) {

}
#endif


void LTAt930::initializeHardware(const char* portName)
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

	// laser status 

	GlobalObjects::LMFTracker->Laser->IsLaserWarmedUp->Changed += gcnew LMF::Tracker::BasicTypes::BoolValue::ReadOnlyBoolValue::ChangedEventHandler(&OnLaserChanged);
	GlobalObjects::LMFTracker->Laser->IsOn->Changed += gcnew LMF::Tracker::BasicTypes::BoolValue::ReadOnlyBoolValue::ChangedEventHandler(&OnLaserChanged);

	std::cout << "Is Laser Warmed Up: Label: " << (decode)(GlobalObjects::LMFTracker->Laser->IsLaserWarmedUp->Label) << " Value: " << TFS[GlobalObjects::LMFTracker->Laser->IsLaserWarmedUp->Value] << std::endl;
	std::cout << "Is Laser on: " << (decode)(GlobalObjects::LMFTracker->Laser->IsOn->Label) << " Value : " << TFS[GlobalObjects::LMFTracker->Laser->IsOn->Value] << std::endl;

	setIntegerParam(L_islaseron, (int)GlobalObjects::LMFTracker->Laser->IsOn->Value);
	setIntegerParam(L_laseronoff_command, (int)GlobalObjects::LMFTracker->Laser->IsOn->Value);
	setIntegerParam(L_islaserwarm, (int)GlobalObjects::LMFTracker->Laser->IsLaserWarmedUp->Value);
	callParamCallbacks();

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

	GlobalObjects::LMFTracker->PositionTo(true, false, 546, 3059, 690);

};

asynStatus LTAt930::writeInt32(asynUser* pasynUser, epicsInt32 value)
{
	int function = pasynUser->reason;
	asynStatus status = asynSuccess;
	const char* paramName;
	const char* functionName = "writeInt32";

	/* Set the parameter in the parameter library. */
	status = (asynStatus)setIntegerParam(function, value);

	/* Fetch the parameter string name for possible use in debugging */
	getParamName(function, &paramName);

	printf("in writeInt32 . . . function %d  value %d\n", function, value);

	if (function == L_laseronoff_command) {
		GlobalObjects::LMFTracker->Laser->IsOn->Value = value;
	}
	else if (function == L_measonoff_command) {
		if (value == 1)
			GlobalObjects::LMFTracker->Measurement->StartMeasurement();
		if (value == 0)
			GlobalObjects::LMFTracker->Measurement->StopMeasurement();
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

asynStatus LTAt930::readInt32(asynUser* pasynUser, epicsInt32* value)
{
	int addr;
	int function = pasynUser->reason;
	int status = 0;
	//	epicsInt32 temp;

	static const char* functionName = "readInt32";

	this->getAddress(pasynUser, &addr);



	// what here is likely to change, that isn't updated via callbacks or other meams?
	/*
	The actual stuff down in the startup code could be here, but then I would have to deal with doing callbacks for things that do not as of yet exist
	*/

	//  if (function == L_iscompatibleFirmware) {
	//	getIntegerParam(L_iscompatibleFirmware, &temp); 
	//	printf("ReadInt32 values is  %d\n", temp);
	//    temp = temp +1;
	//	asynPortDriver::readInt32(pasynUser, &temp);
	//		asynPortDriver::writeInt32(pasynUser, temp);
	//	callParamCallbacks(addr);
	//   }

	  // Other functions we call the base class method
	//  else {
	status = asynPortDriver::readInt32(pasynUser, value);
	// }
	printf("in readInt32 . . . function %d  value %d\n", function, value);
	callParamCallbacks(addr);
	return (status == 0) ? asynSuccess : asynError;
}

/* Configuration routine.  Called directly, or from the iocsh function below */

extern "C" {

	/** EPICS iocsh callable function to call constructor for the LTAt930 class.
	  * \param[in] portName The name of the asyn port driver to be created.
	  */
//	int LTAt930Configure(const char* portName)
//	{
//		new LTAt930(portName);

//		return(asynSuccess);
//	}
	extern "C" int LTAt930Configure(const char* portName, int maxSizeX, int maxSizeY, int dataType,
		int maxBuffers, int maxMemory, int priority, int stackSize)
	{
		new LTAt930(portName, maxSizeX, maxSizeY, (NDDataType_t)dataType,
			(maxBuffers < 0) ? 0 : maxBuffers,
			(maxMemory < 0) ? 0 : maxMemory,
			priority, stackSize);

		return(asynSuccess);
	}


	/* EPICS iocsh shell commands */

	static const iocshArg initArg0 = { "portName",iocshArgString };

	static const iocshArg* const initArgs[] = { &initArg0 };
	static const iocshFuncDef initFuncDef = { "LTAt930Configure",1,initArgs };
	static void initCallFunc(const iocshArgBuf* args)
	{
//		LTAt930Configure(args[0].sval);
    LTAt930Configure(args[0].sval, args[1].ival, args[2].ival, args[3].ival,
                      args[4].ival, args[5].ival, args[6].ival, args[7].ival);

	}

	void LTAt930Register(void)
	{
		iocshRegister(&initFuncDef, initCallFunc);
	}

	epicsExportRegistrar(LTAt930Register);

}

/*
Laser Tracker Related Callbacks !!!!!!
*/


void LTAt930::OnChanged(LMF::Tracker::BasicTypes::DoubleValue::ReadOnlyDoubleValue^ sender, double paramNewValue)
{
	//	std::cout << blue << on_white;
	//	std::cout << blue << on_white;
	std::cout << "OnChange Double Value: " << paramNewValue;

	//	throw gcnew System::NotImplementedException();
	std::cout << " Label: " << (decode)(sender->Label) <<
		" Value: " << sender->Value <<
		std::endl;
	//	std::cout << reset;

}
void LTAt930::OnMeasurementArrived(LMF::Tracker::Measurements::MeasurementSettings^ sender, LMF::Tracker::MeasurementResults::MeasurementCollection^ paramMeasurements, LMF::Tracker::ErrorHandling::LmfException^ paramException)
{
	//	throw gcnew System::NotImplementedException();
	//	std::cout << "I am in the On MeasurementArivved Callback . . . " << std::endl;

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


				//			std::cout << "Measurment Humidity: " << LastMeasurement->Humidity->Value << " Pressure: " << LastMeasurement->Pressure->Value << " Temperature: " << LastMeasurement->Temperature->Value << std::endl;
				LTAt930_->setDoubleParam(LTAt930_->L_humidity, LastMeasurement->Humidity->Value);
				LTAt930_->setDoubleParam(LTAt930_->L_pressure, LastMeasurement->Pressure->Value);
				LTAt930_->setDoubleParam(LTAt930_->L_temperature, LastMeasurement->Temperature->Value);


				if (StationaryMeasurement3D^ stationaryMeas3D = dynamic_cast<StationaryMeasurement3D^>(LastMeasurement))
				{
					//					std::cout << "I am a stationary3d measurement \n";

					//					std::cout << " X = " << stationaryMeas3D->Position->Coordinate1->Value << " " << (decode)(stationaryMeas3D->Position->Coordinate1->UnitString);
					//					std::cout << " Y = " << stationaryMeas3D->Position->Coordinate2->Value << " " << (decode)(stationaryMeas3D->Position->Coordinate2->UnitString);
					//					std::cout << " Z = " << stationaryMeas3D->Position->Coordinate3->Value << " " << (decode)(stationaryMeas3D->Position->Coordinate3->UnitString) << std::endl;



					LTAt930_->setDoubleParam(LTAt930_->L_x, stationaryMeas3D->Position->Coordinate1->Value);
					LTAt930_->setDoubleParam(LTAt930_->L_y, stationaryMeas3D->Position->Coordinate2->Value);
					LTAt930_->setDoubleParam(LTAt930_->L_z, stationaryMeas3D->Position->Coordinate3->Value);

				}
				else if (StationaryMeasurement6D^ stationaryMeas6D = dynamic_cast<StationaryMeasurement6D^>(LastMeasurement))
				{
					//					std::cout << "I am a stationary6d measurement \n";

					//					std::cout << " X = " << stationaryMeas6D->Position->Coordinate1->Value << " " << (decode)(stationaryMeas6D->Position->Coordinate1->UnitString);
					//					std::cout << " Y = " << stationaryMeas6D->Position->Coordinate2->Value << " " << (decode)(stationaryMeas6D->Position->Coordinate2->UnitString);
					//					std::cout << " Z = " << stationaryMeas6D->Position->Coordinate3->Value << " " << (decode)(stationaryMeas6D->Position->Coordinate3->UnitString) << std::endl;

					LTAt930_->setDoubleParam(LTAt930_->L_x, stationaryMeas3D->Position->Coordinate1->Value);
					LTAt930_->setDoubleParam(LTAt930_->L_y, stationaryMeas3D->Position->Coordinate2->Value);
					LTAt930_->setDoubleParam(LTAt930_->L_z, stationaryMeas3D->Position->Coordinate3->Value);


				}
				else if (SingleShotMeasurement3D^ singleshot3dD = dynamic_cast<SingleShotMeasurement3D^>(LastMeasurement))
				{
					//					std::cout << "I am a singleshot 3d measurement \n";

					//					std::cout << " X = " << singleshot3dD->Position->Coordinate1->Value << " " << (decode)(singleshot3dD->Position->Coordinate1->UnitString);
					//					std::cout << " Y = " << singleshot3dD->Position->Coordinate2->Value << " " << (decode)(singleshot3dD->Position->Coordinate2->UnitString);
					//					std::cout << " Z = " << singleshot3dD->Position->Coordinate3->Value << " " << (decode)(singleshot3dD->Position->Coordinate3->UnitString) << std::endl;
					LTAt930_->setDoubleParam(LTAt930_->L_x, stationaryMeas3D->Position->Coordinate1->Value);
					LTAt930_->setDoubleParam(LTAt930_->L_y, stationaryMeas3D->Position->Coordinate2->Value);
					LTAt930_->setDoubleParam(LTAt930_->L_z, stationaryMeas3D->Position->Coordinate3->Value);


				}
				else if (SingleShotMeasurement6D^ singleshot6dD = dynamic_cast<SingleShotMeasurement6D^>(LastMeasurement))
				{
					//					std::cout << "I am a singleshot 6d measurement \n";

					//					std::cout << " X = " << singleshot6dD->Position->Coordinate1->Value << " " << (decode)(singleshot6dD->Position->Coordinate1->UnitString);
					//					std::cout << " Y = " << singleshot6dD->Position->Coordinate2->Value << " " << (decode)(singleshot6dD->Position->Coordinate2->UnitString);
					//					std::cout << " Z = " << singleshot6dD->Position->Coordinate3->Value << " " << (decode)(singleshot6dD->Position->Coordinate3->UnitString) << std::endl;
					LTAt930_->setDoubleParam(LTAt930_->L_x, stationaryMeas3D->Position->Coordinate1->Value);
					LTAt930_->setDoubleParam(LTAt930_->L_y, stationaryMeas3D->Position->Coordinate2->Value);
					LTAt930_->setDoubleParam(LTAt930_->L_z, stationaryMeas3D->Position->Coordinate3->Value);

				}
			}
		}
	}
	LTAt930_->callParamCallbacks();

	// not sure why This callback has to be restrted while onEnviromnetalValuesChanged does not
// *AND* should probably check if it is even leagal to restart
// *AND* having to put this here means that I also can't actually stop it. Unless I check the epics status to see if it shuld be resytarted or not


//	GlobalObjects::LMFTracker->Measurement->StartMeasurement();

	//	std::cout << reset;
}


void LTAt930::OnEnvironmentalValuesChanged(LMF::Tracker::Meteo::MeteoStation^ sender, double paramTemperature, double paramHumidity, double paramPressure)
{
	//	std::cout << blue << on_white;
	std::cout << "Callback Environment values changed: " <<
		" Temp: " << paramTemperature <<
		" Humidity: " << paramHumidity <<
		" Pressure: " << paramPressure <<
		std::endl;

	LTAt930_->setDoubleParam(LTAt930_->L_humidity, paramHumidity);
	LTAt930_->setDoubleParam(LTAt930_->L_pressure, paramPressure);
	LTAt930_->setDoubleParam(LTAt930_->L_temperature, paramTemperature);

	LTAt930_->callParamCallbacks();

	//	std::cout << reset;

		//	throw gcnew System::NotImplementedException();
}

void LTAt930::OnErrorArrived(LMF::Tracker::Tracker^ sender, LMF::Tracker::ErrorHandling::LmfError^ error)
{
	//	throw gcnew System::NotImplementedException();
	std::cout << "Callback OnErrorArrived:" <<
		" Number: " << (int)(error->Number) <<
		" Title : " << (decode)(error->Title) <<
		" Description: " << (decode)(error->Description) <<
		" Solution: " << (decode)(error->Solution) <<
		std::endl;
}


void LTAt930::OnInformationArrived(LMF::Tracker::Tracker^ sender, LMF::Tracker::ErrorHandling::LmfInformation^ paramInfo)
{
	//	throw gcnew System::NotImplementedException();
	std::cout << "Callback OnInformationArrived: " <<
		" Number: " << (int)(paramInfo->Number) <<
		" Title : " << (decode)(paramInfo->Title) <<
		" Description: " << (decode)(paramInfo->Description) <<
		" Solution: " << (decode)(paramInfo->Solution) <<
		std::endl;
}


void LTAt930::OnWarningArrived(LMF::Tracker::Tracker^ sender, LMF::Tracker::ErrorHandling::LmfWarning^ warning)
{
	//	throw gcnew System::NotImplementedException();
	std::cout << "Callback OnWarningArrived:" <<
		" Number: " << (int)(warning->Number) <<
		" Title : " << (decode)(warning->Title) <<
		" Description: " << (decode)(warning->Description) <<
		" Solution: " << (decode)(warning->Solution) <<
		std::endl;
}


void LTAt930::OnChanged(LMF::Tracker::MeasurementStatus::MeasurementStatusValue^ sender, LMF::Tracker::Enums::EMeasurementStatus paramNewValue)
{
	//	throw gcnew System::NotImplementedException();

	const char* EMeasurementStatusStrings[] = { "ReadyToMeasure","MeasurementInProgress","NotReady","Invalid" }; //ok

	std::cout << "Callback OnMeasurementStatusChanged: ";

	//	if (paramNewValue == EMeasurementStatus::ReadyToMeasure) { std::cout << "Ready To Measure . . . \n"; }
	//	if (paramNewValue == EMeasurementStatus::MeasurementInProgress) { std::cout << "Measurement in Progress . . . \n"; }
	//	if (paramNewValue == EMeasurementStatus::NotReady) { std::cout << "Not Ready . . . \n"; }
	//	if (paramNewValue == EMeasurementStatus::Invalid) { std::cout << "Measurement Status Invalid . . . \n"; }

	std::cout << EMeasurementStatusStrings[(int)paramNewValue] << std::endl;

	LTAt930_->setIntegerParam(LTAt930_->L_meas_in_prog, (int)paramNewValue);
	LTAt930_->callParamCallbacks();
}


void LTAt930::OnChanged(LMF::Tracker::BasicTypes::BoolValue::ReadOnlyBoolValue^ sender, bool paramNewValue)
{
	//	throw gcnew System::NotImplementedException();
	std::cout << "Callback OnBool Value changed: " << paramNewValue;
	std::cout << " Label: " << (decode)(sender->Label) <<
		" Value: " << TFS[(int)sender->Value] << " (" << (int)sender->Value << ")" <<
		std::endl;
}


void LTAt930::OnMeasChanged(LMF::Tracker::BasicTypes::BoolValue::ReadOnlyBoolValue^ sender, bool paramNewValue)
{
	//	throw gcnew System::NotImplementedException();
	std::cout << "Callback OnMeasChanged Bool Value changed: " << paramNewValue;
	std::cout << " Label: " << (decode)(sender->Label) <<
		" Value: " << TFS[(int)sender->Value] << " (" << (int)sender->Value << ")" <<
		std::endl;

	LTAt930_->setIntegerParam(LTAt930_->L_meas_in_prog, (int)sender->Value);
	LTAt930_->callParamCallbacks();
}

void LTAt930::OnLaserChanged(LMF::Tracker::BasicTypes::BoolValue::ReadOnlyBoolValue^ sender, bool paramNewValue)
{
	//	throw gcnew System::NotImplementedException();
	std::cout << "Callback OnLaserChanged Bool Value changed: " << paramNewValue;
	std::cout << " Label: " << (decode)(sender->Label) <<
		" Value: " << TFS[(int)sender->Value] <<
		std::endl;
	// Now these stupid callbacks get  . . . messy . . . since the "Label", what this value actually means could be *dozens* of values if we used a generic callback
	// as is, we have to choices "Is laser on" and "Is laser warmed up"		

	string str1 = "Is laser on";
	string str2 = "Is laser warmed up";

	if (str1 == (decode)(sender->Label)) {
		LTAt930_->setIntegerParam(LTAt930_->L_islaseron, (int)sender->Value);
		LTAt930_->callParamCallbacks();
	}
	else if (str2 == (decode)(sender->Label)) {
		LTAt930_->setIntegerParam(LTAt930_->L_islaserwarm, (int)sender->Value);
		LTAt930_->callParamCallbacks();
	}

}
