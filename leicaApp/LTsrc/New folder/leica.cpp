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

static const char *driverName = "leica";

// support utilities

#ifdef xxx
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


static void exitHandler(void* drvPvt) {

	//  printf("\nThis is an exit handler being hit . . . which invoked heavy magic to actually make the destructor get processed.\n"); 

	leica* pleica = (leica*)drvPvt;
	
	delete pleica;

}

#endif
 int leica::computeImage()
 {
	   int status = asynSuccess;
	   return(status);
	   
 }
 
#ifdef OLDER
/** Computes the new image data */
int leica::computeImage()
{
    int status = asynSuccess;
    NDDataType_t dataType;
    int itemp;
    int binX, binY, minX, minY, sizeX, sizeY, reverseX, reverseY;
    int xDim=0, yDim=1, colorDim=-1;
    int resetImage;
    int maxSizeX, maxSizeY;
    int colorMode;
    int ndims=0;
    NDDimension_t dimsOut[3];
    size_t dims[3];
    NDArrayInfo_t arrayInfo;
    NDArray *pImage;
    const char* functionName = "computeImage";

    /* NOTE: The caller of this function must have taken the mutex */

    status |= getIntegerParam(ADBinX,         &binX);
    status |= getIntegerParam(ADBinY,         &binY);
    status |= getIntegerParam(ADMinX,         &minX);
    status |= getIntegerParam(ADMinY,         &minY);
    status |= getIntegerParam(ADSizeX,        &sizeX);
    status |= getIntegerParam(ADSizeY,        &sizeY);
    status |= getIntegerParam(ADReverseX,     &reverseX);
    status |= getIntegerParam(ADReverseY,     &reverseY);
    status |= getIntegerParam(ADMaxSizeX,     &maxSizeX);
    status |= getIntegerParam(ADMaxSizeY,     &maxSizeY);
    status |= getIntegerParam(NDColorMode,    &colorMode);
    status |= getIntegerParam(NDDataType,     &itemp); dataType = (NDDataType_t)itemp;
   
    if (status) asynPrint(this->pasynUserSelf, ASYN_TRACE_ERROR,
                    "%s:%s: error getting parameters\n",
                    driverName, functionName);

    /* Make sure parameters are consistent, fix them if they are not */
    if (minX < 0) {
        minX = 0;
        status |= setIntegerParam(ADMinX, minX);
    }
    if (minY < 0) {
        minY = 0;
        status |= setIntegerParam(ADMinY, minY);
    }
    if (minX > maxSizeX-1) {
        minX = maxSizeX-1;
        status |= setIntegerParam(ADMinX, minX);
    }
    if (minY > maxSizeY-1) {
        minY = maxSizeY-1;
        status |= setIntegerParam(ADMinY, minY);
    }
    if (sizeX < 1) {
	sizeX = 1;
        status |= setIntegerParam(ADSizeX, sizeX);
    }
    if (sizeY < 1) {
	sizeY = 1;
        status |= setIntegerParam(ADSizeY, sizeY);
    }
    if (sizeX > maxSizeX-minX) {
        sizeX = maxSizeX-minX;
        status |= setIntegerParam(ADSizeX, sizeX);
    }
    if (sizeY > maxSizeY-minY) {
        sizeY = maxSizeY-minY;
        status |= setIntegerParam(ADSizeY, sizeY);
    }
    if (binX < 1) {
        binX = 1;
        status |= setIntegerParam(ADBinX, binX);
    }
    if (binY < 1) {
        binY = 1;
        status |= setIntegerParam(ADBinY, binY);
    }
    if (binX > sizeX) {
	binX = sizeX;
        status |= setIntegerParam(ADBinX, binX);
    }
    if (binY > sizeY) {
	binY = sizeY;
        status |= setIntegerParam(ADBinY, binY);
    }
    if (sizeX % binX != 0) {
	sizeX = (sizeX / binX) * binX;
        status |= setIntegerParam(ADSizeX, sizeX);
    }
    if (sizeY % binY != 0) {
	sizeY = (sizeY / binY) * binY;
        status |= setIntegerParam(ADSizeY, sizeY);
    }

    switch (colorMode) {
        case NDColorModeMono:
            ndims = 2;
            xDim = 0;
            yDim = 1;
            break;
        case NDColorModeRGB1:
            ndims = 3;
            colorDim = 0;
            xDim     = 1;
            yDim     = 2;
            break;
        case NDColorModeRGB2:
            ndims = 3;
            colorDim = 1;
            xDim     = 0;
            yDim     = 2;
            break;
        case NDColorModeRGB3:
            ndims = 3;
            colorDim = 2;
            xDim     = 0;
            yDim     = 1;
            break;
    }

    if (resetImage) {
        /* Free the previous buffers */
        if (pRaw_) pRaw_->release();
        if (pBackground_) pBackground_->release();
        if (pRamp_) pRamp_->release();
        if (pPeak_) pPeak_->release();
        /* Allocate the raw buffer we use to compute images. */
        dims[xDim] = maxSizeX;
        dims[yDim] = maxSizeY;
        if (ndims > 2) dims[colorDim] = 3;
        pRaw_        = this->pNDArrayPool->alloc(ndims, dims, dataType, 0, NULL);
        pBackground_ = this->pNDArrayPool->alloc(ndims, dims, dataType, 0, NULL);
        pRamp_       = this->pNDArrayPool->alloc(ndims, dims, dataType, 0, NULL);
        pPeak_       = this->pNDArrayPool->alloc(ndims, dims, dataType, 0, NULL);
        pRaw_->getInfo(&arrayInfo_);

        if (!pRaw_) {
            asynPrint(this->pasynUserSelf, ASYN_TRACE_ERROR,
                      "%s:%s: error allocating raw buffer\n",
                      driverName, functionName);
            return(status);
        }
    }

    switch (dataType) {
        case NDInt8:
            status |= computeArray<epicsInt8>(maxSizeX, maxSizeY);
            break;
        case NDUInt8:
            status |= computeArray<epicsUInt8>(maxSizeX, maxSizeY);
            break;
        case NDInt16:
            status |= computeArray<epicsInt16>(maxSizeX, maxSizeY);
            break;
        case NDUInt16:
            status |= computeArray<epicsUInt16>(maxSizeX, maxSizeY);
            break;
        case NDInt32:
            status |= computeArray<epicsInt32>(maxSizeX, maxSizeY);
            break;
        case NDUInt32:
            status |= computeArray<epicsUInt32>(maxSizeX, maxSizeY);
            break;
        case NDInt64:
            status |= computeArray<epicsInt64>(maxSizeX, maxSizeY);
            break;
        case NDUInt64:
            status |= computeArray<epicsUInt64>(maxSizeX, maxSizeY);
            break;
        case NDFloat32:
            status |= computeArray<epicsFloat32>(maxSizeX, maxSizeY);
            break;
        case NDFloat64:
            status |= computeArray<epicsFloat64>(maxSizeX, maxSizeY);
            break;
    }

    /* Extract the region of interest with binning.
     * If the entire image is being used (no ROI or binning) that's OK because
     * convertImage detects that case and is very efficient */
    pRaw_->initDimension(&dimsOut[xDim], sizeX);
    pRaw_->initDimension(&dimsOut[yDim], sizeY);
    if (ndims > 2) pRaw_->initDimension(&dimsOut[colorDim], 3);
    dimsOut[xDim].binning = binX;
    dimsOut[xDim].offset  = minX;
    dimsOut[xDim].reverse = reverseX;
    dimsOut[yDim].binning = binY;
    dimsOut[yDim].offset  = minY;
    dimsOut[yDim].reverse = reverseY;
    /* We save the most recent image buffer so it can be used in the read() function.
     * Now release it before getting a new version. */
    if (this->pArrays[0]) this->pArrays[0]->release();
    status = this->pNDArrayPool->convert(pRaw_,
                                         &this->pArrays[0],
                                         dataType,
                                         dimsOut);
    if (status) {
        asynPrint(this->pasynUserSelf, ASYN_TRACE_ERROR,
                    "%s:%s: error allocating buffer in convert()\n",
                    driverName, functionName);
        return(status);
    }
    pImage = this->pArrays[0];
    pImage->getInfo(&arrayInfo);
    status = asynSuccess;
    status |= setIntegerParam(NDArraySize,  (int)arrayInfo.totalBytes);
    status |= setIntegerParam(NDArraySizeX, (int)pImage->dims[xDim].size);
    status |= setIntegerParam(NDArraySizeY, (int)pImage->dims[yDim].size);
    status |= setIntegerParam(SimResetImage, 0);
    if (status) asynPrint(this->pasynUserSelf, ASYN_TRACE_ERROR,
                    "%s:%s: error setting parameters\n",
                    driverName, functionName);
    return(status);
}
#endif

static void simTaskC(void *drvPvt)
{
    leica *pPvt = (leica *)drvPvt;

    pPvt->simTask();
}

/** This thread calls computeImage to compute new image data and does the callbacks to send it to higher layers.
  * It implements the logic for single, multiple or continuous acquisition. */
void leica::simTask()
{
    int status = asynSuccess;
    int imageCounter;
    int numImages, numImagesCounter;
    int imageMode;
    int arrayCallbacks;
    int acquire=0;
    NDArray *pImage;
    double acquireTime, acquirePeriod, delay;
    epicsTimeStamp startTime, endTime;
    double elapsedTime;
    const char *functionName = "simTask";

    this->lock();
    /* Loop forever */
    while (1) {
        /* If we are not acquiring then wait for a semaphore that is given when acquisition is started */
        if (!acquire) {
          /* Release the lock while we wait for an event that says acquire has started, then lock again */
            asynPrint(this->pasynUserSelf, ASYN_TRACE_FLOW,
                "%s:%s: waiting for acquire to start\n", driverName, functionName);
            this->unlock();
            status = epicsEventWait(startEventId_);
            this->lock();
            acquire = 1;
            setStringParam(ADStatusMessage, "Acquiring data");
            setIntegerParam(ADNumImagesCounter, 0);
        }

        /* We are acquiring. */
        /* Get the current time */
        epicsTimeGetCurrent(&startTime);
        getIntegerParam(ADImageMode, &imageMode);

        /* Get the exposure parameters */
        getDoubleParam(ADAcquireTime, &acquireTime);
        getDoubleParam(ADAcquirePeriod, &acquirePeriod);

        setIntegerParam(ADStatus, ADStatusAcquire);

        /* Open the shutter */
        setShutter(ADShutterOpen);

        /* Call the callbacks to update any changes */
        callParamCallbacks();

        /* Update the image */
        status = computeImage();
        if (status) continue;

        /* Simulate being busy during the exposure time.  Use epicsEventWaitWithTimeout so that
         * manually stopping the acquisition will work */
        epicsTimeGetCurrent(&endTime);
        elapsedTime = epicsTimeDiffInSeconds(&endTime, &startTime);
        delay = acquireTime - elapsedTime;
        asynPrint(this->pasynUserSelf, ASYN_TRACE_FLOW,
                  "%s:%s: delay=%f\n",
                  driverName, functionName, delay);
        if (delay <= 0.0) delay = MIN_DELAY;
        this->unlock();
        status = epicsEventWaitWithTimeout(stopEventId_, delay);
        this->lock();
        if (status == epicsEventWaitOK) {
            acquire = 0;
            if (imageMode == ADImageContinuous) {
              setIntegerParam(ADStatus, ADStatusIdle);
            } else {
              setIntegerParam(ADStatus, ADStatusAborted);
            }
            callParamCallbacks();
        }

        /* Close the shutter */
        setShutter(ADShutterClosed);

        if (!acquire) continue;

        setIntegerParam(ADStatus, ADStatusReadout);
        /* Call the callbacks to update any changes */
        callParamCallbacks();

        pImage = this->pArrays[0];

        /* Get the current parameters */
        getIntegerParam(NDArrayCounter, &imageCounter);
        getIntegerParam(ADNumImages, &numImages);
        getIntegerParam(ADNumImagesCounter, &numImagesCounter);
        getIntegerParam(NDArrayCallbacks, &arrayCallbacks);
        imageCounter++;
        numImagesCounter++;
        setIntegerParam(NDArrayCounter, imageCounter);
        setIntegerParam(ADNumImagesCounter, numImagesCounter);

        /* Put the frame number and time stamp into the buffer */
        pImage->uniqueId = imageCounter;
        pImage->timeStamp = startTime.secPastEpoch + startTime.nsec / 1.e9;
        updateTimeStamp(&pImage->epicsTS);

        /* Get any attributes that have been defined for this driver */
        this->getAttributes(pImage->pAttributeList);

        if (arrayCallbacks) {
            /* Call the NDArray callback */
            asynPrint(this->pasynUserSelf, ASYN_TRACE_FLOW,
                      "%s:%s: calling imageData callback\n", driverName, functionName);
            doCallbacksGenericPointer(pImage, NDArrayData, 0);
        }

        /* See if acquisition is done */
        if ((imageMode == ADImageSingle) ||
            ((imageMode == ADImageMultiple) &&
             (numImagesCounter >= numImages))) {

            /* First do callback on ADStatus. */
            setStringParam(ADStatusMessage, "Waiting for acquisition");
            setIntegerParam(ADStatus, ADStatusIdle);
            callParamCallbacks();

            acquire = 0;
            setIntegerParam(ADAcquire, acquire);
            asynPrint(this->pasynUserSelf, ASYN_TRACE_FLOW,
                      "%s:%s: acquisition completed\n", driverName, functionName);
        }

        /* Call the callbacks to update any changes */
        callParamCallbacks();

        /* If we are acquiring then sleep for the acquire period minus elapsed time. */
        if (acquire) {
            epicsTimeGetCurrent(&endTime);
            elapsedTime = epicsTimeDiffInSeconds(&endTime, &startTime);
            delay = acquirePeriod - elapsedTime;
            asynPrint(this->pasynUserSelf, ASYN_TRACE_FLOW,
                      "%s:%s: delay=%f\n",
                      driverName, functionName, delay);
            if (delay >= 0.0) {
                /* We set the status to waiting to indicate we are in the period delay */
                setIntegerParam(ADStatus, ADStatusWaiting);
                callParamCallbacks();
                this->unlock();
                status = epicsEventWaitWithTimeout(stopEventId_, delay);
                this->lock();
                if (status == epicsEventWaitOK) {
                  acquire = 0;
                  if (imageMode == ADImageContinuous) {
                    setIntegerParam(ADStatus, ADStatusIdle);
                  } else {
                    setIntegerParam(ADStatus, ADStatusAborted);
                  }
                  callParamCallbacks();
                }
            }
        }
    }
}


/** Called when asyn clients call pasynInt32->write().
  * This function performs actions for some parameters, including ADAcquire, ADColorMode, etc.
  * For all parameters it sets the value in the parameter library and calls any registered callbacks..
  * \param[in] pasynUser pasynUser structure that encodes the reason and address.
  * \param[in] value Value to write. */
asynStatus leica::writeInt32(asynUser *pasynUser, epicsInt32 value)
{
    int function = pasynUser->reason;
    int adstatus;
    int acquiring;
    int imageMode;
    asynStatus status = asynSuccess;

    /* Ensure that ADStatus is set correctly before we set ADAcquire.*/
    getIntegerParam(ADStatus, &adstatus);
    getIntegerParam(ADAcquire, &acquiring);
    getIntegerParam(ADImageMode, &imageMode);
    if (function == ADAcquire) {
      if (value && !acquiring) {
        setStringParam(ADStatusMessage, "Acquiring data");
      }
      if (!value && acquiring) {
        setStringParam(ADStatusMessage, "Acquisition stopped");
        if (imageMode == ADImageContinuous) {
          setIntegerParam(ADStatus, ADStatusIdle);
        } else {
          setIntegerParam(ADStatus, ADStatusAborted);
        }
        setIntegerParam(ADStatus, ADStatusAcquire);
      }
    }
    callParamCallbacks();

    /* Set the parameter and readback in the parameter library.  This may be overwritten when we read back the
     * status at the end, but that's OK */
    status = setIntegerParam(function, value);

    /* For a real detector this is where the parameter is sent to the hardware */
    if (function == ADAcquire) {
        if (value && !acquiring) {
            /* Send an event to wake up the simulation task.
             * It won't actually start generating new images until we release the lock below */
            epicsEventSignal(startEventId_);
        }
        if (!value && acquiring) {
            /* This was a command to stop acquisition */
            /* Send the stop event */
            epicsEventSignal(stopEventId_);
        }
    } else if ((function == NDDataType) ||
               (function == NDColorMode)) {  // This assumes order in leica.h!
//	status = setIntegerParam(SimResetImage, 1);
    } else {
        /* If this parameter belongs to a base class call its method */
        if (function < FIRST_LEICA_PARAM) status = ADDriver::writeInt32(pasynUser, value);
    }

    /* Do callbacks so higher layers see any changes */
    callParamCallbacks();

    if (status)
        asynPrint(pasynUser, ASYN_TRACE_ERROR,
              "%s:writeInt32 error, status=%d function=%d, value=%d\n",
              driverName, status, function, value);
    else
        asynPrint(pasynUser, ASYN_TRACEIO_DRIVER,
              "%s:writeInt32: function=%d, value=%d\n",
              driverName, function, value);
    return status;
}


/** Called when asyn clients call pasynFloat64->write().
  * This function performs actions for some parameters, including ADAcquireTime, ADGain, etc.
  * For all parameters it sets the value in the parameter library and calls any registered callbacks..
  * \param[in] pasynUser pasynUser structure that encodes the reason and address.
  * \param[in] value Value to write. */
asynStatus leica::writeFloat64(asynUser *pasynUser, epicsFloat64 value)
{
    int function = pasynUser->reason;
    asynStatus status = asynSuccess;

    /* Set the parameter and readback in the parameter library.  This may be overwritten when we read back the
     * status at the end, but that's OK */
    status = setDoubleParam(function, value);

    /* Changing any of the simulation parameters requires recomputing the base image */
    if ((function == ADGain) || (function >= FIRST_LEICA_PARAM)) {
 //       status = setIntegerParam(SimResetImage, 1);
    } else {
        /* This parameter belongs to a base class call its method */
        status = ADDriver::writeFloat64(pasynUser, value);
    }

    /* Do callbacks so higher layers see any changes */
    callParamCallbacks();
    if (status)
        asynPrint(pasynUser, ASYN_TRACE_ERROR,
              "%s:writeFloat64 error, status=%d function=%d, value=%f\n",
              driverName, status, function, value);
    else
        asynPrint(pasynUser, ASYN_TRACEIO_DRIVER,
              "%s:writeFloat64: function=%d, value=%f\n",
              driverName, function, value);
    return status;
}


/** Report status of the driver.
  * Prints details about the driver if details>0.
  * It then calls the ADDriver::report() method.
  * \param[in] fp File pointed passed by caller where the output is written to.
  * \param[in] details If >0 then driver details are printed.
  */
void leica::report(FILE *fp, int details)
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
leica::leica(const char *portName, int maxSizeX, int maxSizeY, NDDataType_t dataType,
                         int maxBuffers, size_t maxMemory, int priority, int stackSize)

    : ADDriver(portName, 1, 0, maxBuffers, maxMemory,
               0, 0, /* No interfaces beyond those set in ADDriver.cpp */
               0, 1, /* ASYN_CANBLOCK=0, ASYN_MULTIDEVICE=0, autoConnect=1 */
               priority, stackSize)

{
    int status = asynSuccess;
    char versionString[20];
    const char *functionName = "leica";
	
//	leica_ = this;

    /* Create the epicsEvents for signaling to the simulate task when acquisition starts and stops */
    startEventId_ = epicsEventCreate(epicsEventEmpty);
    if (!startEventId_) {
        printf("%s:%s epicsEventCreate failure for start event\n",
            driverName, functionName);
        return;
    }
    stopEventId_ = epicsEventCreate(epicsEventEmpty);
    if (!stopEventId_) {
        printf("%s:%s epicsEventCreate failure for stop event\n",
            driverName, functionName);
        return;
    }

 
    /* Set some default values for parameters */
    status =  setStringParam (ADManufacturer, "Simulated detector");
    status |= setStringParam (ADModel, "Basic simulator");
    epicsSnprintf(versionString, sizeof(versionString), "%d.%d.%d",
                  DRIVER_VERSION, DRIVER_REVISION, DRIVER_MODIFICATION);
    setStringParam(NDDriverVersion, versionString);
    setStringParam(ADSDKVersion, versionString);
    setStringParam(ADSerialNumber, "No serial number");
    setStringParam(ADFirmwareVersion, "No firmware");

    status |= setIntegerParam(ADMaxSizeX, maxSizeX);
    status |= setIntegerParam(ADMaxSizeY, maxSizeY);
    status |= setIntegerParam(ADMinX, 0);
    status |= setIntegerParam(ADMinY, 0);
    status |= setIntegerParam(ADBinX, 1);
    status |= setIntegerParam(ADBinY, 1);
    status |= setIntegerParam(ADReverseX, 0);
    status |= setIntegerParam(ADReverseY, 0);
    status |= setIntegerParam(ADSizeX, maxSizeX);
    status |= setIntegerParam(ADSizeY, maxSizeY);
    status |= setIntegerParam(NDArraySizeX, maxSizeX);
    status |= setIntegerParam(NDArraySizeY, maxSizeY);
    status |= setIntegerParam(NDArraySize, 0);
    status |= setIntegerParam(NDDataType, dataType);
    status |= setIntegerParam(ADImageMode, ADImageContinuous);
    status |= setDoubleParam (ADAcquireTime, .001);
    status |= setDoubleParam (ADAcquirePeriod, .005);
    status |= setIntegerParam(ADNumImages, 100);

    if (status) {
        printf("%s: unable to set camera parameters\n", functionName);
        return;
    }

 //   initializeHardware(portName);

    // Test setting the device connected to false.  
    // Real drivers should do this if they fail to connect to the hardware
    //this->deviceIsConnected = false;
    //this->disconnect(pasynUserSelf);

    /* Create the thread that updates the images */
    status = (epicsThreadCreate("SimDetTask",
                                epicsThreadPriorityMedium,
                                epicsThreadGetStackSize(epicsThreadStackMedium),
                                (EPICSTHREADFUNC)simTaskC,
                                this) == NULL);
    if (status) {
        printf("%s:%s epicsThreadCreate failure for image task\n",
            driverName, functionName);
        return;
    }
}
#ifdef xxx
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

	}
	
#endif	
leica::~leica(void) 
{
}

/** Configuration command, called directly or from iocsh */
extern "C" int leicaConfig(const char *portName, int maxSizeX, int maxSizeY, int dataType,
                                 int maxBuffers, int maxMemory, int priority, int stackSize)
{
    new leica(portName, maxSizeX, maxSizeY, (NDDataType_t)dataType,
                    (maxBuffers < 0) ? 0 : maxBuffers,
                    (maxMemory < 0) ? 0 : maxMemory,
                    priority, stackSize);
    return(asynSuccess);
}

/** Code for iocsh registration */
static const iocshArg leicaConfigArg0 = {"Port name", iocshArgString};
static const iocshArg leicaConfigArg1 = {"Max X size", iocshArgInt};
static const iocshArg leicaConfigArg2 = {"Max Y size", iocshArgInt};
static const iocshArg leicaConfigArg3 = {"Data type", iocshArgInt};
static const iocshArg leicaConfigArg4 = {"maxBuffers", iocshArgInt};
static const iocshArg leicaConfigArg5 = {"maxMemory", iocshArgInt};
static const iocshArg leicaConfigArg6 = {"priority", iocshArgInt};
static const iocshArg leicaConfigArg7 = {"stackSize", iocshArgInt};
static const iocshArg * const leicaConfigArgs[] =  {&leicaConfigArg0,
                                                          &leicaConfigArg1,
                                                          &leicaConfigArg2,
                                                          &leicaConfigArg3,
                                                          &leicaConfigArg4,
                                                          &leicaConfigArg5,
                                                          &leicaConfigArg6,
                                                          &leicaConfigArg7};
static const iocshFuncDef configleica = {"leicaConfig", 8, leicaConfigArgs};
static void configleicaCallFunc(const iocshArgBuf *args)
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

#ifdef xxx
 void OnLaserChanged(LMF::Tracker::BasicTypes::BoolValue::ReadOnlyBoolValue^ sender, bool paramNewValue)
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
		leica_->setIntegerParam(leica_->L_islaseron, (int)sender->Value);
		leica_->callParamCallbacks();
	}
	else if (str2 == (decode)(sender->Label)) {
		leica_->setIntegerParam(leica_->L_islaserwarm, (int)sender->Value);
		leica_->callParamCallbacks();
	}

}
void OnErrorArrived(LMF::Tracker::Tracker^ sender, LMF::Tracker::ErrorHandling::LmfError^ error)
{
	//	throw gcnew System::NotImplementedException();
	std::cout << "Callback OnErrorArrived:" <<
		" Number: " << (int)(error->Number) <<
		" Title : " << (decode)(error->Title) <<
		" Description: " << (decode)(error->Description) <<
		" Solution: " << (decode)(error->Solution) <<
		std::endl;
}


void OnInformationArrived(LMF::Tracker::Tracker^ sender, LMF::Tracker::ErrorHandling::LmfInformation^ paramInfo)
{
	//	throw gcnew System::NotImplementedException();
	std::cout << "Callback OnInformationArrived: " <<
		" Number: " << (int)(paramInfo->Number) <<
		" Title : " << (decode)(paramInfo->Title) <<
		" Description: " << (decode)(paramInfo->Description) <<
		" Solution: " << (decode)(paramInfo->Solution) <<
		std::endl;
}


void OnWarningArrived(LMF::Tracker::Tracker^ sender, LMF::Tracker::ErrorHandling::LmfWarning^ warning)
{
	//	throw gcnew System::NotImplementedException();
	std::cout << "Callback OnWarningArrived:" <<
		" Number: " << (int)(warning->Number) <<
		" Title : " << (decode)(warning->Title) <<
		" Description: " << (decode)(warning->Description) <<
		" Solution: " << (decode)(warning->Solution) <<
		std::endl;
}
#endif