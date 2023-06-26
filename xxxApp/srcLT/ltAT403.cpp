

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

#include "ltAt403.h"
#include <epicsExport.h>

#include <windows.h>
#pragma comment(lib, "User32.lib")

extern void closeWindowByTitle(const char* title); 
extern void closeWindowByPartialTitle(const char* partialTitle);
extern std::string decode(System::String^ something);


/*
void closeWindowByTitle(const char* title) {
    HWND window = FindWindow(NULL, title);
    if (window != NULL) {
 //       PostMessage(window, WM_CLOSE, 0, 0);
	
		ShowWindow(window, SW_HIDE);
    }
}

void closeWindowByPartialTitle(const char* partialTitle) {
//	printf("in windows finding code . . . \n");
    HWND window = FindWindow(NULL, NULL);
    while (window != NULL) {
        char title[1024];
        GetWindowText(window, title, sizeof(title));
//		printf("%s \n",title);
        if (strstr(title, partialTitle) != NULL) {
//            PostMessage(window, WM_CLOSE, 0, 0);
		    ShowWindow(window, SW_HIDE);
        }
        window = FindWindowEx(NULL, window, NULL, NULL);
    }
}
*/

static const char *driverName="LTAt403";


std::string decode(System::String^ something)
{
	if (System::String::IsNullOrEmpty(something))
		return "";
	else
		return (msclr::interop::marshal_as<std::string>(something));
}


void OnChanged(LMF::Tracker::BasicTypes::DoubleValue::ReadOnlyDoubleValue^ sender, double paramNewValue)
{
//	std::cout << blue << on_white;
//	std::cout << blue << on_white;
	std::cout << "Double Value changed: " << paramNewValue;

	//	throw gcnew System::NotImplementedException();
	std::cout << " Label: " << (decode)(sender->Label) <<
		" Value: " << sender->Value <<
		std::endl;
//	std::cout << reset;


}

void OnEnvironmentalValuesChanged(LMF::Tracker::Meteo::MeteoStation^ sender, double paramTemperature, double paramHumidity, double paramPressure)
{
//	std::cout << blue << on_white;
	std::cout << "Callback Environment values changed: " <<
		" Temp: " << paramTemperature <<
		" Humidity: " << paramHumidity <<
		" Pressure: " << paramPressure <<
		std::endl;
//	std::cout << reset;

	//	throw gcnew System::NotImplementedException();
}


/** Constructor for the LTAt403 class.
  * Calls constructor for the asynPortDriver base class.
  * \param[in] portName The name of the asyn port driver to be created.
  */
LTAt403::LTAt403(const char *portName)
   : asynPortDriver(portName,
                    1, /* maxAddr */
                    asynInt32Mask | asynFloat64Mask | asynFloat64ArrayMask | asynEnumMask | asynDrvUserMask | asynOctetMask, /* Interface mask */
                    asynInt32Mask | asynFloat64Mask | asynFloat64ArrayMask | asynEnumMask | asynOctetMask,  /* Interrupt mask */
                    0, /* asynFlags.  This driver does not block and it is not multi-device, so flag is 0 */
                    1, /* Autoconnect */
                    0, /* Default priority */
                    0) /* Default stack size*/
{
//    asynStatus status;
//    int i;
    const char *functionName = "LTAt403";

	LTAt403_ = this;

    initializeHardware(portName);	
 
	createParam(L_iscompatibleFirmwareString,          asynParamInt32,         &L_iscompatibleFirmware);	
	
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

 
    printf("\n***********************************\n");
	printf("\nConnected to Laser Tracker, checking default parameters . . . \n");
 
 
    String^ Comment = GlobalObjects::LMFTracker->Comment;
    cout << "Comment: " << (decode)(Comment) << "\n";   
    setStringParam(L_comment,(decode)(Comment));
     
    String^ ExpectedFirmware = GlobalObjects::LMFTracker->ExpectedFirmware;
    cout << "Firmware: " << (decode)(ExpectedFirmware) << "\n";
    setStringParam(L_expectedFirmware,(decode)(ExpectedFirmware));

    String^ InstalledFirmware = GlobalObjects::LMFTracker->InstalledFirmware;
    cout << "InstalledFirmware: " << (decode)(InstalledFirmware) << "\n";
	setStringParam(L_installedFirmware,(decode)(InstalledFirmware));
	
    String^ IP = GlobalObjects::LMFTracker->IPAddress;
    cout << "IP: " << (decode)(IP) << "\n";
	setStringParam(L_ipAddress,(decode)(IP));

    Boolean CompatFirmware = GlobalObjects::LMFTracker->IsCompatibleWithInstalledFirmware;
    cout << "Is Compatible With Installed Firmware: " << CompatFirmware << "\n";
	setIntegerParam(L_iscompatibleFirmware, CompatFirmware); 
	
    String^ Name = GlobalObjects::LMFTracker->Name;
    cout << "Name: " << (decode)(Name) << "\n";
	setStringParam(L_name,(decode)(Name));
    
    String^ ProductName = GlobalObjects::LMFTracker->ProductName;
    cout << "ProductName: " << (decode)(ProductName) << "\n";
	setStringParam(L_productName,(decode)(ProductName));

    String^ SerialNumber = GlobalObjects::LMFTracker->SerialNumber;
    cout << "Serial: " << (decode)(SerialNumber) << "\n";
	setStringParam(L_serialNumber,(decode)(SerialNumber));
	
	cout << "Getting Info from Settings . . .  \n";

	LMF::Units::ECoordinateType coordtype = GlobalObjects::LMFTracker->Settings->CoordinateType;
	const char* coordtypeNames[] = {"Spherical", "Cartesian", "Cylindrical"};
	cout << " CoordinateType : " << coordtypeNames[(int)coordtype] << "\n";

	LMF::Units::ERotationType rottype = GlobalObjects::LMFTracker->Settings->RotationType;
	const char* rottypeNames[] = { "RotationAngles", "RollPitchYaw", "Quarternion" };
	cout << " RotationType : " << rottypeNames[(int)coordtype] << "\n";

	LMF::Units::EAngleUnit angunit = GlobalObjects::LMFTracker->Settings->Units->AngleUnit;
	const char* angunitNames[] = { "Radian", "Millirad", "Degree", "Gon", "CC"};
	cout << " AngleUnits : " << angunitNames[(int)angunit] << "\n";

	LMF::Units::EHumidityUnit humunit = GlobalObjects::LMFTracker->Settings->Units->HumidityUnit;
	const char* humunitNames[] = { "RelativeHumidity"};
	cout << " HumidityUnits : " << humunitNames[(int)humunit] << "\n";

	LMF::Units::ELengthUnit lenunit = GlobalObjects::LMFTracker->Settings->Units->LengthUnit;
	const char* lenunitNames[] = { "Meter", "Millimeter", "Micrometer","Foot","Yard", "Inch"};
	cout << " LengthUnits : " << lenunitNames[(int)humunit] << "\n";

	LMF::Units::EPercentUnit perunit = GlobalObjects::LMFTracker->Settings->Units->PercentUnit;
	const char* perunitNames[] = { "Percent", "None" };
	cout << " PercentUnits : " << perunitNames[(int)perunit] << "\n";

	LMF::Units::EPressureUnit presunit = GlobalObjects::LMFTracker->Settings->Units->PressureUnit;
	const char* presunitNames[] = { "mBar", "HPascal","KPascal","MmHg", "Psi", "InH2O","InHg"};
	cout << " PressureUnits : " << presunitNames[(int)presunit] << "\n";

	LMF::Units::ETemperatureUnit tempunit = GlobalObjects::LMFTracker->Settings->Units->TemperatureUnit;
	const char* tempunitNames[] = { "Celsius", "Fahrenheit"};
	cout << " TemperatureUnits : " << tempunitNames[(int)tempunit] << "\n";

	LMF::Units::ETimeUnit timeunit = GlobalObjects::LMFTracker->Settings->Units->TimeUnit;
	const char* timeunitNames[] = { "Millisecond", "Second", "Minute", "Hour"};
	cout << " TimeUnits : " << timeunitNames[(int)timeunit] << "\n";

	
    GlobalObjects::LMFTracker->GetDirectionAsync();    
    Direction^ dir1 = GlobalObjects::LMFTracker->GetDirection(); 
	cout << "Direction H Angle: " << dir1->HorizontalAngle->Value << " " << (decode)(dir1->HorizontalAngle->UnitString)
		 << " V Angle: " << dir1->VerticalAngle->Value << " " << (decode)(dir1->VerticalAngle->UnitString) << "\n";
 	setDoubleParam(L_horizontalAngle, dir1->HorizontalAngle->Value); 
	setDoubleParam(L_verticalAngle, dir1->VerticalAngle->Value);
	setStringParam(L_angleUnits,(decode)(dir1->VerticalAngle->UnitString));
	
// this code you can't actually do unless a tracker actually exists
	GlobalObjects::LMFTracker->Measurement->MeasurementArrived += gcnew LMF::Tracker::Measurements::MeasurementSettings::MeasurementArrivedHandler(&OnMeasurementArrived);
	LMF::Tracker::MeasurementResults::Measurement^ data = GlobalObjects::LMFTracker->Measurement->MeasureStationary();
	


GlobalObjects::LMFTracker->Measurement->StartMeasurement();

	 GlobalObjects::LMFTracker->MeteoStation->EnvironmentalValuesChanged += gcnew LMF::Tracker::Meteo::MeteoStation::EnvironmentalValuesChangedEventHandler(&OnEnvironmentalValuesChanged);

	 GlobalObjects::LMFTracker->MeteoStation->HardwareHumidity->Changed += gcnew LMF::Tracker::BasicTypes::DoubleValue::ReadOnlyDoubleValue::ChangedEventHandler(&OnChanged);
	 GlobalObjects::LMFTracker->MeteoStation->HardwarePressure->Changed += gcnew LMF::Tracker::BasicTypes::DoubleValue::ReadOnlyDoubleValue::ChangedEventHandler(&OnChanged);
	 GlobalObjects::LMFTracker->MeteoStation->HardwareTemperature->Changed += gcnew LMF::Tracker::BasicTypes::DoubleValue::ReadOnlyDoubleValue::ChangedEventHandler(&OnChanged);


	cout << "Measurment Humidity: " << data->Humidity->Value << " " << (decode) (data->Humidity->UnitString)
		<< " Pressure: " << data->Pressure->Value << " " << (decode)(data->Pressure->UnitString)
		<< " Temperature: " << data->Temperature->Value << " " << (decode)(data->Temperature->UnitString) << "\n";
 
 	setDoubleParam(L_humidity, data->Humidity->Value);
	setDoubleParam(L_pressure, data->Pressure->Value);
	setDoubleParam(L_temperature, data->Temperature->Value);
	setStringParam(L_humidityUnits,(decode) (data->Humidity->UnitString));
	setStringParam(L_pressureUnits,(decode)(data->Pressure->UnitString));
	setStringParam(L_temperatureUnits,(decode)(data->Temperature->UnitString) );

 
 StationaryMeasurement3D^ stationaryMeas3D = dynamic_cast<StationaryMeasurement3D^>(data);
	cout << " X = " << stationaryMeas3D->Position->Coordinate1->Value << " " << (decode)(stationaryMeas3D->Position->Coordinate1->UnitString);
	cout << " Y = " << stationaryMeas3D->Position->Coordinate2->Value << " " << (decode)(stationaryMeas3D->Position->Coordinate2->UnitString);
	cout << " Z = " << stationaryMeas3D->Position->Coordinate3->Value << " " << (decode)(stationaryMeas3D->Position->Coordinate3->UnitString) << "\n";
 
 	setDoubleParam(L_x, stationaryMeas3D->Position->Coordinate1->Value);
	setDoubleParam(L_y, stationaryMeas3D->Position->Coordinate2->Value);
	setDoubleParam(L_z, stationaryMeas3D->Position->Coordinate3->Value);
	setStringParam(L_xUnits,(decode)(stationaryMeas3D->Position->Coordinate1->UnitString) );
	setStringParam(L_yUnits,(decode)(stationaryMeas3D->Position->Coordinate2->UnitString) );
	setStringParam(L_zUnits,(decode)(stationaryMeas3D->Position->Coordinate3->UnitString) );


// end of test commands 
    
	printf("\n***********************************\n\n");
	

    callParamCallbacks();	

	

}



//extern int mainLT(void);
/*
I need the msvc compiler development environment to more easily deal with resolving references to the TBL
I don't expect to actually be able to run this as a binary.
*/
#ifdef DO_MAIN
int main(void) {

}
#endif



void LTAt403::initializeHardware(const char *portName)
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
//    closeWindowByTitle("AT403 Simulator 1.8.0.2250"); // up to 1.9.1.11 now
//	closeWindowByTitle("AT930 Simulator 1.8.0.2250"); // up to 1.9.1.11 now
// Not sure what a normal laser tracker might show , maybe key on the SDK value?

//	closeWindowByPartialTitle("Simulator"); //not working, but the above ones did.

//	closeWindowByPartialTitle("Simulator"); //but works here, so is the windows not always finsihed generating when this happens?

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
	
	GlobalObjects::LMFTracker->PositionTo(true, false, 546, 3059, 690);

	
// Now we actually need to do more than just connect to it since certain other things should also be happening . . . 


};



asynStatus LTAt403::readInt32(asynUser *pasynUser, epicsInt32 *value)
{
  int addr;
  int function = pasynUser->reason;
  int status=0;
  epicsInt32 temp;
  
  static const char *functionName = "readInt32";

   
  this->getAddress(pasynUser, &addr);
  
//  printf("in readInt32 . . . function %d \n",function);

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

  callParamCallbacks(addr);
  return (status==0) ? asynSuccess : asynError;
}

/* Configuration routine.  Called directly, or from the iocsh function below */

extern "C" {

/** EPICS iocsh callable function to call constructor for the LTAt403 class.
  * \param[in] portName The name of the asyn port driver to be created.
  */
int LTAt403Configure(const char *portName)
{	
    new LTAt403(portName);
	
    return(asynSuccess);
}


/* EPICS iocsh shell commands */

static const iocshArg initArg0 = { "portName",iocshArgString};

static const iocshArg * const initArgs[] = {&initArg0};
static const iocshFuncDef initFuncDef = {"LTAt403Configure",1,initArgs};
static void initCallFunc(const iocshArgBuf *args)
{
    LTAt403Configure(args[0].sval);
}

void LTAt403Register(void)
{
    iocshRegister(&initFuncDef,initCallFunc);
}

epicsExportRegistrar(LTAt403Register);

}

/*

void LTAt403::OnMeasurementArrived(LMF::Tracker::Measurements::MeasurementSettings^ sender, LMF::Tracker::MeasurementResults::MeasurementCollection^ paramMeasurements, LMF::Tracker::ErrorHandling::LmfException^ paramException)
{
//	throw gcnew System::NotImplementedException();
	std::cout << "I am in the On MeasurementArivved Callback . . . " << std::endl;

	LMF::Tracker::MeasurementResults::Measurement^ LastMeasurement = nullptr;

	// throw gcnew System::NotImplementedException();
//	std::cout << blue << on_white;
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
GlobalObjects::myLTAt403->setDoubleParam(L_x, stationaryMeas3D->Position->Coordinate1->Value);
 //	setDoubleParam(L_x, stationaryMeas3D->Position->Coordinate1->Value);
//	setDoubleParam(L_y, stationaryMeas3D->Position->Coordinate2->Value);
//	setDoubleParam(L_z, stationaryMeas3D->Position->Coordinate3->Value);

				}
				else if (StationaryMeasurement6D^ stationaryMeas6D = dynamic_cast<StationaryMeasurement6D^>(LastMeasurement))
				{
					std::cout << "I am a stationary6d measurement \n";

					std::cout << " X = " << stationaryMeas6D->Position->Coordinate1->Value << " " << (decode)(stationaryMeas6D->Position->Coordinate1->UnitString);
					std::cout << " Y = " << stationaryMeas6D->Position->Coordinate2->Value << " " << (decode)(stationaryMeas6D->Position->Coordinate2->UnitString);
					std::cout << " Z = " << stationaryMeas6D->Position->Coordinate3->Value << " " << (decode)(stationaryMeas6D->Position->Coordinate3->UnitString) << std::endl;
 //	setDoubleParam(L_x, stationaryMeas3D->Position->Coordinate1->Value);
//	setDoubleParam(L_y, stationaryMeas3D->Position->Coordinate2->Value);
//	setDoubleParam(L_z, stationaryMeas3D->Position->Coordinate3->Value);


				}
				else if (SingleShotMeasurement3D^ singleshot3dD = dynamic_cast<SingleShotMeasurement3D^>(LastMeasurement))
				{
					std::cout << "I am a singleshot 3d measurement \n";

					std::cout << " X = " << singleshot3dD->Position->Coordinate1->Value << " " << (decode)(singleshot3dD->Position->Coordinate1->UnitString);
					std::cout << " Y = " << singleshot3dD->Position->Coordinate2->Value << " " << (decode)(singleshot3dD->Position->Coordinate2->UnitString);
					std::cout << " Z = " << singleshot3dD->Position->Coordinate3->Value << " " << (decode)(singleshot3dD->Position->Coordinate3->UnitString) << std::endl;
// 	setDoubleParam(L_x, stationaryMeas3D->Position->Coordinate1->Value);
//	setDoubleParam(L_y, stationaryMeas3D->Position->Coordinate2->Value);
//	setDoubleParam(L_z, stationaryMeas3D->Position->Coordinate3->Value);


				}
				else if (SingleShotMeasurement6D^ singleshot6dD = dynamic_cast<SingleShotMeasurement6D^>(LastMeasurement))
				{
					std::cout << "I am a singleshot 6d measurement \n";

					std::cout << " X = " << singleshot6dD->Position->Coordinate1->Value << " " << (decode)(singleshot6dD->Position->Coordinate1->UnitString);
					std::cout << " Y = " << singleshot6dD->Position->Coordinate2->Value << " " << (decode)(singleshot6dD->Position->Coordinate2->UnitString);
					std::cout << " Z = " << singleshot6dD->Position->Coordinate3->Value << " " << (decode)(singleshot6dD->Position->Coordinate3->UnitString) << std::endl;
 //	setDoubleParam(L_x, stationaryMeas3D->Position->Coordinate1->Value);
//	setDoubleParam(L_y, stationaryMeas3D->Position->Coordinate2->Value);
//	setDoubleParam(L_z, stationaryMeas3D->Position->Coordinate3->Value);

				}
			}
		}
	}
//	    callParamCallbacks();	
	GlobalObjects::LMFTracker->Measurement->StartMeasurement();
//	std::cout << reset;
}
*/

/*
Callbacks gets strange since I can not talk to EPICS from a LMF callback since The attmpt is basically flagged as attempting to call a 
non static method from static code. 

Lightfield gets arround this is 2 ways, the class callback more or less calls a epics callback routine, so the effect gives toy all over the parameters
provided by the vendor class callback, and since the epics asyn callbacks also have the correct references to the asyn pointer to the object, everything *might*
actually resolve.

So giving it a try . . . 


*/
/*
// step #2 The specific call back into the epics side of things . . . 
void LTAt403::myOnMeasurementArrived(asynUser* pasynUser, LMF::Tracker::MeasurementResults::MeasurementCollection^)
{
	callParamCallbacks();
}

// step #1 the hardware level callback

void OnMeasurementArrived(LMF::Tracker::Measurements::MeasurementSettings^ sender, LMF::Tracker::MeasurementResults::MeasurementCollection^ paramMeasurements, LMF::Tracker::ErrorHandling::LmfException^ paramException)
{

//	LTAt403::myOnMeasurementArrived(paramMeasurements);
}
*/

void LTAt403::myOnMeasurementArrived(LMF::Tracker::MeasurementResults::MeasurementCollection^ paramMeasurements)
{
	LTAt403_->callParamCallbacks();
	std::cout << paramMeasurements->Count << std::endl;

//		callParamCallbacks();
}

void LTAt403::OnMeasurementArrived(LMF::Tracker::Measurements::MeasurementSettings^ sender, LMF::Tracker::MeasurementResults::MeasurementCollection^ paramMeasurements, LMF::Tracker::ErrorHandling::LmfException^ paramException)
{
//	throw gcnew System::NotImplementedException();
//	myOnMeasurementArrived(paramMeasurements);
	
		LTAt403_->callParamCallbacks();
	std::cout << "in callback " << paramMeasurements->Count << std::endl;
	GlobalObjects::LMFTracker->Measurement->StartMeasurement();

}
