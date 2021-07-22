Measurement Computing 1608GX-2A0 Driver 
Version 3 


/* drvUSB1608G_V3.cpp* 

* 
This version implements digital outputs, simple analog inputs and simple analog
outputs* 

* 
Mark Rivers 
* April 14, 2012*/
#include <iocsh.h> 
#include <epicsExport.h>
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
#define digitalOutputString "DIGITAL_OUTPUT" 

#define NUM_ANALOG_IN 16 // Number analog inputs on 1608G#define NUM_ANALOG_OUT 2 // Number of analog outputs on 1608G

#define NUM_IO_BITS 8 // Number of digital I/O bits on 1608G

#define MAX_SIGNALS NUM_ANALOG_IN 


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

protected: 

// Analog output parameters

int analogOutValue_;
#define FIRST_USB1608G_PARAM analogOutValue_

// Analog input parameters

int analogInValue_;
int analogInRange_;

// Digital I/O parameters

int digitalDirection_;
int digitalOutput_;

#define LAST_USB1608G_PARAM digitalOutput_ 

private: 
int boardNum_;
}; 


/** Constructor for the USB1608G class*/

USB1608G::USB1608G(const char *portName, int boardNum)
: asynPortDriver(portName, MAX_SIGNALS, NUM_PARAMS,

// Interfaces that we implement

asynInt32Mask | 
asynUInt32DigitalMask | asynDrvUserMask,

// Interfaces that do callbacks

asynUInt32DigitalMask,

ASYN_MULTIDEVICE | ASYN_CANBLOCK, 1,

/* ASYN_CANBLOCK=1, ASYN_MULTIDEVICE=1, autoConnect=1 */
0, 0), 
/* Default priority and stack size */
boardNum_(boardNum)
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
createParam(digitalOutputString, 
asynParamUInt32Digital, &digitalOutput_);

} 


asynStatus USB1608G::writeUInt32Digital(asynUser *pasynUser, epicsUInt32 value,
epicsUInt32 mask)
{ 

int function = pasynUser->reason;
int status=0;
int i;
epicsUInt32 outValue=0, outMask, direction=0;static const char *functionName = "writeUInt32Digital"; 

setUIntDigitalParam(function, value, mask);

if (function == digitalDirection_) {
outValue = (value == 0) ? DIGITALIN : DIGITALOUT;
for (i=0; i<NUM_IO_BITS; i++) {

if ((mask & (1<<i)) != 0) {
status = cbDConfigBit(boardNum_, AUXPORT, i, outValue);}}} 

else if (function == digitalOutput_) {
getUIntDigitalParam(digitalDirection_, &direction, 0xFFFFFFFF);
for (i=0, outMask=1; i<NUM_IO_BITS; i++, outMask = (outMask<<1)) {

// Only write the value if the mask has this bit set and the direction// for that bit is output (1)

outValue = ((value &outMask) == 0) ? 0 : 1;
if ((mask & outMask & direction) != 0) {
status = cbDBitOut(boardNum_, AUXPORT, i, outValue);}} 


 } 

callParamCallbacks();if (status == 0) {

asynPrint(pasynUser, ASYN_TRACEIO_DRIVER,
"%s:%s, port %s, wrote outValue=0x%x, value=0x%x, mask=0x%x, direction=0x%x\n",
driverName, functionName, 
this->portName, outValue, value, mask, direction);

} 
else {
asynPrint(pasynUser, ASYN_TRACE_ERROR,

"%s:%s, port %s, ERROR writing outValue=0x%x, value=0x%x, mask=0x%x,
direction=0x%x, status=%d\n"
,
driverName, functionName, 
this->portName, outValue, value, mask, direction,
status);

}return (status==0) ? asynSuccess : asynError;
} 

