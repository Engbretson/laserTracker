
Measurement Computing 1608GX-2A0 Driver 
Version 4 


/* drvUSB1608G_V2.cpp

* This version implements digital inputs and outputs, simple analog inputs and simple
analog outputs, with a poller thread* 

* 
Mark Rivers 
* April 14, 2012*/ 

#include <iocsh.h> 
#include <epicsExport.h>
#include <epicsThread.h>
#include <asynPortDriver.h>
#include "cbw.h" 

static const char *driverName = "USB1608G"; 

// Analog output parameters

#define analogOutValueString "ANALOG_OUT_VALUE" 

// Analog input parameters

#define analogInValueString "ANALOG_IN_VALUE" 
#define analogInRangeString "ANALOG_IN_RANGE" 

// Digital I/O parameters

#define digitalDirectionString "DIGITAL_DIRECTION"

#define digitalInputString "DIGITAL_INPUT" 

#define digitalOutputString "DIGITAL_OUTPUT" 

#define NUM_ANALOG_IN 16 // Number analog inputs on 1608G#define NUM_ANALOG_OUT 2 // Number of analog outputs on 1608G#define NUM_IO_BITS 8 // Number of digital I/O bits on 1608G#define MAX_SIGNALS NUM_ANALOG_IN 


#define DEFAULT_POLL_TIME 0.01 

/** Class definition for the USB1608G class*/

class USB1608G : public asynPortDriver {
public: 
USB1608G(
const char *portName, int boardNum); 

/* These are the methods that we override from asynPortDriver */

virtual asynStatus writeInt32(asynUser *pasynUser, epicsInt32 value);
virtual asynStatus readInt32(asynUser *pasynUser, epicsInt32 *value);
virtual asynStatus getBounds(asynUser *pasynUser, epicsInt32 *low,

virtual asynStatus writeUInt32Digital(asynUser *pasynUser, epicsUInt32 value,
epicsUInt32 mask);
virtual void report(FILE *fp, int details);

// These should be private but are called from C

virtual void pollerThread(void); 

protected: 

// Analog output parameters

int analogOutValue_;
#define FIRST_USB1608G_PARAM analogOutValue_ 

// Analog input parameters

int analogInValue_;
int analogInRange_; 

// Digital I/O parameters

int digitalDirection_;

int digitalInput_; 


int digitalOutput_;
#define LAST_USB1608G_PARAM digitalOutput_ 

private: 
int boardNum_;

double pollTime_;
int forceCallback_;

}; 

#define NUM_PARAMS (&LAST_USB1608G_PARAM - &FIRST_USB1608G_PARAM + 1) 

static void pollerThreadC(void * pPvt)

{ 
USB1608G *pUSB1608G = (USB1608G *)pPvt;
pUSB1608G->pollerThread();

} 


/** Constructor for the USB1608G class */

USB1608G::USB1608G(const char *portName, int boardNum)
: asynPortDriver(portName, MAX_SIGNALS, NUM_PARAMS,

// Interfaces that we implement

asynInt32Mask | asynUInt32DigitalMask | asynDrvUserMask,

// Interfaces that do callbacks

asynUInt32DigitalMask,
ASYN_MULTIDEVICE | ASYN_CANBLOCK, 1,

/* ASYN_CANBLOCK=1, ASYN_MULTIDEVICE=1, autoConnect=1 */
0, 0), 
/* Default priority and stack size */
boardNum_(boardNum),

pollTime_(DEFAULT_POLL_TIME),
forceCallback_(1)

{ 

// Analog output parameters

createParam(analogOutValueString, 
asynParamInt32, &analogOutValue_);

// Analog input parameters

createParam(analogInValueString, 
asynParamInt32, &analogInValue_);
createParam(analogInRangeString, 
asynParamInt32, &analogInRange_);

// Digital I/O parameters

createParam(digitalDirectionString, 
asynParamUInt32Digital, &digitalDirection_);

createParam(digitalInputString, 
asynParamUInt32Digital, &digitalInput_);

createParam(digitalOutputString, 
asynParamUInt32Digital, &digitalOutput_);

/* Start the thread to poll digital inputs and do callbacks to device support */

epicsThreadCreate(
"USB1608GPoller",
this);

} 


void USB1608G::pollerThread()
{ 

/* This function runs in a separate thread. It waits for the poll time */

static const char *functionName = "pollerThread";
epicsUInt32 newValue, changedBits, prevInput=0;unsigned short biVal;;
int i, status; 

while(1) {
lock();

// Read the digital inputs

status = cbDIn(boardNum_, AUXPORT, &biVal);
if (status)

asynPrint(pasynUserSelf, ASYN_TRACE_ERROR,
"%s:%s: ERROR calling cbDIn, status=%d\n",
driverName, functionName, status);

newValue = biVal;
changedBits = newValue ^ prevInput;
if (forceCallback_ || (changedBits != 0)) {

prevInput = newValue;
forceCallback_ = 0;
setUIntDigitalParam(digitalInput_, newValue, 0xFFFFFFFF);

}
for (i=0; i<MAX_SIGNALS; i++) {

callParamCallbacks(i);}
unlock();
epicsThreadSleep(pollTime_);

}} 


measCompBinaryIn.template 

record(bi, "$(P)$(R)")

{ 
field(DTYP, "asynUInt32Digital")
field(INP, 
"@asynMask($(PORT),$(ADDR),$(MASK))DIGITAL_INPUT")
field(ZNAM, "Low")
field(ONAM, "High")
field(SCAN, "I/O Intr")

} 