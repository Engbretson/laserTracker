
#include "asynPortDriver.h"


// may not need any of these actually, could hard encode it at point of actual use

//ints


//strings


//doubles




/** Class that demonstrates the use of the asynPortDriver base class to greatly simplify the task
  * of writing an asyn port driver.
  * This class does a simple simulation of a digital oscilloscope.  It computes a waveform, computes
  * statistics on the waveform, and does callbacks with the statistics and the waveform data itself.
  * I have made the methods of this class public in order to generate doxygen documentation for them,
  * but they should really all be private. */
class SESI : public asynPortDriver {
public:
    SESI(const char *portName);
    virtual asynStatus readInt32(asynUser *pasynUser, epicsInt32 *value);
    /* These are the methods that we override from asynPortDriver */
 //   virtual asynStatus writeInt32(asynUser *pasynUser, epicsInt32 value);
//    virtual asynStatus writeFloat64(asynUser *pasynUser, epicsFloat64 value);
//    virtual asynStatus readFloat64Array(asynUser *pasynUser, epicsFloat64 *value, size_t nElements, size_t *nIn);

    /* These are the methods that are new to this class */
//    void simTask(void);
static void initializeHardware(const char *portName);


protected:

private:
    /** Values used for pasynUser->reason, and indexes into the parameter library. */
	int L_iscompatibleFirmware;
	
    int L_comment;
    int L_expectedFirmware;
    int L_installedFirmware;
    int L_ipAddress;
    int L_name; 
    int L_productName; 
    int L_serialNumber; 
	int L_angleUnits; 
	int L_humidityUnits; 
	int L_pressureUnits; 
	int L_temperatureUnits; 
	int L_xUnits;
	int L_yUnits;
	int L_zUnits;

	
	int L_horizontalAngle;
	int L_verticalAngle;
	int L_humidity;
	int L_pressure;
	int L_temperature;
	int L_x;
	int L_y;
	int L_z;
							

};
#define FIRST_lsAT403_PARAM comment
#define LAST_lsAT403_PARAM serialNumber
#define NUM_PARAMS (&LAST_lsAT403_PARAM - &FIRST_lsAT403_PARAM + 1) 
