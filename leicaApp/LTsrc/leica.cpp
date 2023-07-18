/* leica.cpp
 *
 * This is a driver for a Laser Tracker area detector.
 * 
 * Just need to derive from this since the image info from the Laser Tarcker Camera is probably required.
 *
 * Author: Mark Engbretson
 *         Argonne National Laboratory
 *
 * Created:  July 11, 2023
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

static const char* driverName = "leica";

// support utilities

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

	//	  printf("\nThis is an exit handler being hit . . . which invoked heavy magic to actually make the destructor get processed.\n"); 

	leica* pleica = (leica*)drvPvt;
	delete pleica;

}


int leica::computeImage()
{
	int status = asynSuccess;
	return(status);

}

// Minimal, standalone image array processing -
#ifdef zzz
int leica::computeImage()
{
	NDDataType_t  imageDataType;
	size_t imageDims[2];
	NDArrayInfo arrayInfo;
	epicsTimeStamp currentTime;

	imageDims[0] = 2048;
	imageDims[1] = 2048;


	//				imageDataType = NDFloat32;
	//				setIntegerParam(NDDataType, NDFloat32);

	imageDataType = NDUInt8;
	setIntegerParam(NDDataType, NDUInt8);

	/* Update the image */
	/* First release the copy that we held onto last time */
	if (this->pArrays[0]) {
		this->pArrays[0]->release();
	}


		/* Allocate a new array */
		this->pArrays[0] = pNDArrayPool->alloc(2, imageDims, imageDataType, 0, NULL);
		if (this->pArrays[0] != NULL) {
			pImage = this->pArrays[0];
			pImage->getInfo(&arrayInfo);
			// Copy data from the input to the output, correct 

//					memcpy(pImage->pData, buffer, bufferSize);

			epicsTimeGetCurrent(&currentTime);
			pImage->timeStamp = currentTime.secPastEpoch + currentTime.nsec / 1.e9;

			pImage->uniqueId = rand() * 2048;

			updateTimeStamp(&pImage->epicsTS);

			/* Get attributes that have been defined for this driver */
			getAttributes(pImage->pAttributeList);

			doCallbacksGenericPointer(pImage, NDArrayData, 0);
		}


}

#endif


#ifdef OLDER
/** Computes the new image data */
int leica::computeImage()
{
	int status = asynSuccess;
	NDDataType_t dataType;
	int itemp;
	int binX, binY, minX, minY, sizeX, sizeY, reverseX, reverseY;
	int xDim = 0, yDim = 1, colorDim = -1;
	int resetImage;
	int maxSizeX, maxSizeY;
	int colorMode;
	int ndims = 0;
	NDDimension_t dimsOut[3];
	size_t dims[3];
	NDArrayInfo_t arrayInfo;
	NDArray* pImage;
	const char* functionName = "computeImage";

	/* NOTE: The caller of this function must have taken the mutex */

	status |= getIntegerParam(ADBinX, &binX);
	status |= getIntegerParam(ADBinY, &binY);
	status |= getIntegerParam(ADMinX, &minX);
	status |= getIntegerParam(ADMinY, &minY);
	status |= getIntegerParam(ADSizeX, &sizeX);
	status |= getIntegerParam(ADSizeY, &sizeY);
	status |= getIntegerParam(ADReverseX, &reverseX);
	status |= getIntegerParam(ADReverseY, &reverseY);
	status |= getIntegerParam(ADMaxSizeX, &maxSizeX);
	status |= getIntegerParam(ADMaxSizeY, &maxSizeY);
	status |= getIntegerParam(NDColorMode, &colorMode);
	status |= getIntegerParam(NDDataType, &itemp); dataType = (NDDataType_t)itemp;

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
	if (minX > maxSizeX - 1) {
		minX = maxSizeX - 1;
		status |= setIntegerParam(ADMinX, minX);
	}
	if (minY > maxSizeY - 1) {
		minY = maxSizeY - 1;
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
	if (sizeX > maxSizeX - minX) {
		sizeX = maxSizeX - minX;
		status |= setIntegerParam(ADSizeX, sizeX);
	}
	if (sizeY > maxSizeY - minY) {
		sizeY = maxSizeY - minY;
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
		xDim = 1;
		yDim = 2;
		break;
	case NDColorModeRGB2:
		ndims = 3;
		colorDim = 1;
		xDim = 0;
		yDim = 2;
		break;
	case NDColorModeRGB3:
		ndims = 3;
		colorDim = 2;
		xDim = 0;
		yDim = 1;
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
		pRaw_ = this->pNDArrayPool->alloc(ndims, dims, dataType, 0, NULL);
		pBackground_ = this->pNDArrayPool->alloc(ndims, dims, dataType, 0, NULL);
		pRamp_ = this->pNDArrayPool->alloc(ndims, dims, dataType, 0, NULL);
		pPeak_ = this->pNDArrayPool->alloc(ndims, dims, dataType, 0, NULL);
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
	dimsOut[xDim].offset = minX;
	dimsOut[xDim].reverse = reverseX;
	dimsOut[yDim].binning = binY;
	dimsOut[yDim].offset = minY;
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
	status |= setIntegerParam(NDArraySize, (int)arrayInfo.totalBytes);
	status |= setIntegerParam(NDArraySizeX, (int)pImage->dims[xDim].size);
	status |= setIntegerParam(NDArraySizeY, (int)pImage->dims[yDim].size);
	status |= setIntegerParam(SimResetImage, 0);
	if (status) asynPrint(this->pasynUserSelf, ASYN_TRACE_ERROR,
		"%s:%s: error setting parameters\n",
		driverName, functionName);
	return(status);
}
#endif

static void simTaskC(void* drvPvt)
{
	leica* pPvt = (leica*)drvPvt;

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
	int acquire = 0;
	NDArray* pImage;
	double acquireTime, acquirePeriod, delay;
	epicsTimeStamp startTime, endTime;
	double elapsedTime;
	const char* functionName = "simTask";

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
			}
			else {
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
					}
					else {
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

#ifdef FROMSIMDET
asynStatus leica::writeInt32(asynUser* pasynUser, epicsInt32 value)
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
			}
			else {
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
	}
	else if ((function == NDDataType) ||
		(function == NDColorMode)) {  // This assumes order in leica.h!
//	status = setIntegerParam(SimResetImage, 1);
	}
	else {
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
asynStatus leica::writeFloat64(asynUser* pasynUser, epicsFloat64 value)
{
	int function = pasynUser->reason;
	asynStatus status = asynSuccess;

	/* Set the parameter and readback in the parameter library.  This may be overwritten when we read back the
	 * status at the end, but that's OK */
	status = setDoubleParam(function, value);

	/* Changing any of the simulation parameters requires recomputing the base image */
	if ((function == ADGain) || (function >= FIRST_LEICA_PARAM)) {
		//       status = setIntegerParam(SimResetImage, 1);
	}
	else {
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
#endif

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

	printf("in writeInt32 . . . function %d  %s value %d\n", function, whoami, value);

	if (function == L_gotoXY) {
		int tempx, tempy;
		getIntegerParam(L_XX, &tempx);
		getIntegerParam(L_YY, &tempy);
///		std::cout << "goto x " << tempx << " y " << tempy << std::endl;
		GlobalObjects::LMFTracker->OverviewCamera->MoveToPixel(tempx, tempy, 640, 480);
		GlobalObjects::LMFTracker->OverviewCamera->StartAsync();
	}
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
	// }
	if (function > FIRST_LEICA_PARAM )
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

	fprintf(fp, "Leica detector %s\n", this->portName);
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

	createParam(L_iscompatibleFirmwareString, asynParamInt32, &L_iscompatibleFirmware);
	createParam(L_faceString, asynParamInt32, &L_face);
	createParam(L_face_commandString, asynParamInt32, &L_face_command);

	createParam(L_islaseronString, asynParamInt32, &L_islaseron);
	createParam(L_islaserwarmString, asynParamInt32, &L_islaserwarm);
	createParam(L_laseronoff_commandString, asynParamInt32, &L_laseronoff_command);

	createParam(L_measonoff_commandString, asynParamInt32, &L_measonoff_command);
	createParam(L_meas_in_progString, asynParamInt32, &L_meas_in_prog);

	createParam(L_targetsString, asynParamInt32, &L_targets);

	createParam(L_commentString, asynParamOctet, &L_comment);
	createParam(L_expectedFirmwareString, asynParamOctet, &L_expectedFirmware);
	createParam(L_installedFirmwareString, asynParamOctet, &L_installedFirmware);
	createParam(L_ipAddressString, asynParamOctet, &L_ipAddress);
	createParam(L_nameString, asynParamOctet, &L_name);
	createParam(L_productNameString, asynParamOctet, &L_productName);
	createParam(L_serialNumberString, asynParamOctet, &L_serialNumber);

	createParam(L_gotoXYString, asynParamOctet, &L_gotoXY);


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

	createParam(L_XXString, asynParamInt32, &L_XX);
	createParam(L_YYString, asynParamInt32, &L_YY);

	createParam(L_X1String, asynParamFloat64, &L_X1);
	createParam(L_Y1String, asynParamFloat64, &L_Y1);
	createParam(L_X2String, asynParamFloat64, &L_X2);
	createParam(L_Y2String, asynParamFloat64, &L_Y2);
	createParam(L_X3String, asynParamFloat64, &L_X3);
	createParam(L_Y3String, asynParamFloat64, &L_Y3);
	createParam(L_X4String, asynParamFloat64, &L_X4);
	createParam(L_Y4String, asynParamFloat64, &L_Y4);
	createParam(L_X5String, asynParamFloat64, &L_X5);
	createParam(L_Y5String, asynParamFloat64, &L_Y5);




	// lets do some test commands 

	initializeHardware(portName);
	


    epicsSnprintf(versionString, sizeof(versionString), "%d.%d.%d",
                  DRIVER_VERSION, DRIVER_REVISION, DRIVER_MODIFICATION);
    setStringParam(NDDriverVersion, versionString);
    setStringParam(ADSDKVersion, "1.9.1.11");
 
	setIntegerParam(ADMaxSizeX, 2560);
	setIntegerParam(ADMaxSizeY, 1920);

	printf("\n***********************************\n");
	printf("\nConnected to Laser Tracker, checking default parameters . . . \n");
	
	setStringParam (ADManufacturer, "Leica");
	
	String^ Comment = GlobalObjects::LMFTracker->Comment;
	cout << "Comment: " << (decode)(Comment) << "\n";
	setStringParam(L_comment, (decode)(Comment));

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
	cout << "Is Compatible With Installed Firmware: " << CompatFirmware << "\n";
	setIntegerParam(L_iscompatibleFirmware, CompatFirmware);

	String^ Name = GlobalObjects::LMFTracker->Name;
	cout << "Name: " << (decode)(Name) << "\n";
	setStringParam(L_name, (decode)(Name));

	String^ ProductName = GlobalObjects::LMFTracker->ProductName;
	cout << "ProductName: " << (decode)(ProductName) << "\n";
	setStringParam(L_productName, (decode)(ProductName));
	setStringParam(ADModel, (decode)(ProductName));

	String^ SerialNumber = GlobalObjects::LMFTracker->SerialNumber;
	cout << "Serial: " << (decode)(SerialNumber) << "\n";
	setStringParam(L_serialNumber, (decode)(SerialNumber));
	setStringParam(ADSerialNumber, (decode)(SerialNumber));

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

	// if you don't pick mediuam, you get a mix of mostlu medium res images and maybe 2 fps of the larger or smaller ones.



//	GlobalObjects::LMFTracker->OverviewCamera->ImageArrived += gcnew LMF::Tracker::OVC::OverviewCamera::ImageArrivedHandler(&OnImageArrived);
	GlobalObjects::LMFTracker->OverviewCamera->WPFBitmapImageArrived += gcnew LMF::Tracker::OVC::OverviewCamera::WPFBitmapImageArrivedHandler(&OnWPFBitmapImageArrived);

	GlobalObjects::LMFTracker->OverviewCamera->GetStillImage(LMF::Tracker::Enums::EStillImageMode::High);
	//	GlobalObjects::LMFTracker->OverviewCamera->GetStillImage(LMF::Tracker::Enums::EStillImageMode::Medium);
	//	GlobalObjects::LMFTracker->OverviewCamera->GetStillImage(LMF::Tracker::Enums::EStillImageMode::Low);

		GlobalObjects::LMFTracker->OverviewCamera->StartAsync();
		Sleep(1000);
		GlobalObjects::LMFTracker->OverviewCamera->Stop();

	GlobalObjects::LMFTracker->Measurement->MeasurementArrived += gcnew LMF::Tracker::Measurements::MeasurementSettings::MeasurementArrivedHandler(&OnMeasurementArrived);
	// also not very helpful
//	GlobalObjects::LMFTracker->Measurement->Status->Changed += gcnew LMF::Tracker::MeasurementStatus::MeasurementStatusValue::ChangedEventHandler(&OnChanged);
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



	callParamCallbacks();

#ifdef FROMSIMDET

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
	status = setStringParam(ADManufacturer, "Simulated detector");
	status |= setStringParam(ADModel, "Basic simulator");
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
	status |= setDoubleParam(ADAcquireTime, .001);
	status |= setDoubleParam(ADAcquirePeriod, .005);
	status |= setIntegerParam(ADNumImages, 100);

	if (status) {
		printf("%s: unable to set camera parameters\n", functionName);
		return;
	}
#endif



	// Test setting the device connected to false.  
	// Real drivers should do this if they fail to connect to the hardware
	//this->deviceIsConnected = false;
	//this->disconnect(pasynUserSelf);

#ifdef FROMSIMDET
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
#endif

	epicsAtExit(exitHandler, this);
	GlobalObjects::LMFTracker->OverviewCamera->StartAsync();
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


leica::~leica(void)
{

	GlobalObjects::LMFTracker->OverviewCamera->Stop();

	// code cleanup should happen here like hitting all the lasrer Tracker destructors, as required.
	//    std::cout << "lecia destructor being hit . . . " << std::endl;


	/*
	//
	// Something in the laser tracking code that seems to be related to using Net 4.5.0 (which is older than dirt and
	// reached EOL back in 2017 doesn't play well with the pvaServer - which never shuts down.
	//
	// This effectively performs a stopPVAServer() in the console, which makes this happy, or else  I have to brute force
	// kill this code to get it to stop.
	//
	*/
	iocshCmd("stopPVAServer()");

	/*
		 DWORD me = GetCurrentProcessId();
		std::cout << me << std::endl;
		const auto explorer = OpenProcess(PROCESS_TERMINATE, false, me);
		TerminateProcess(explorer, 1);
		CloseHandle(explorer);
	*/
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


void leica::OnLaserChanged(LMF::Tracker::BasicTypes::BoolValue::ReadOnlyBoolValue^ sender, bool paramNewValue)
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

	std::cout << "Callback OnMeasurementStatusChanged: ";

	//	if (paramNewValue == EMeasurementStatus::ReadyToMeasure) { std::cout << "Ready To Measure . . . \n"; }
	//	if (paramNewValue == EMeasurementStatus::MeasurementInProgress) { std::cout << "Measurement in Progress . . . \n"; }
	//	if (paramNewValue == EMeasurementStatus::NotReady) { std::cout << "Not Ready . . . \n"; }
	//	if (paramNewValue == EMeasurementStatus::Invalid) { std::cout << "Measurement Status Invalid . . . \n"; }

	std::cout << EMeasurementStatusStrings[(int)paramNewValue] << std::endl;

	leica_->setIntegerParam(leica_->L_meas_in_prog, (int)paramNewValue);
	leica_->callParamCallbacks();
}


void leica::OnChanged(LMF::Tracker::BasicTypes::BoolValue::ReadOnlyBoolValue^ sender, bool paramNewValue)
{
	//	throw gcnew System::NotImplementedException();
	std::cout << "Callback OnBool Value changed: " << paramNewValue;
	std::cout << " Label: " << (decode)(sender->Label) <<
		" Value: " << TFS[(int)sender->Value] << " (" << (int)sender->Value << ")" <<
		std::endl;
}


void leica::OnMeasChanged(LMF::Tracker::BasicTypes::BoolValue::ReadOnlyBoolValue^ sender, bool paramNewValue)
{
	

	//	throw gcnew System::NotImplementedException();
	std::cout << "Callback OnMeasChanged Bool Value changed: " << paramNewValue;
	std::cout << " Label: " << (decode)(sender->Label) <<
		" Value: " << TFS[(int)sender->Value] << " (" << (int)sender->Value << ")" <<
		std::endl;

	leica_->setIntegerParam(leica_->L_meas_in_prog, (int)sender->Value);
	leica_->callParamCallbacks();
}

int filenamenumber = 0;

void leica::OnWPFBitmapImageArrived(LMF::Tracker::OVC::OverviewCamera^ sender, System::Windows::Media::Imaging::BitmapImage^ image, LMF::Tracker::OVC::ATRCoordinateCollection^ atrCoordinates)
{
//		filenamenumber++;



		// throw gcnew System::NotImplementedException();
		// 
		// 
	//	std::cout << blue << on_white;
//	std::cout << "Callback OnWPFBitmapImageArrived . . . ";
//	std::cout << atrCoordinates->Count << " Targets seen in Image.";
//			<< std::endl;
	//	std::cout << reset;
	//std::cout << ".";

	leica_->setIntegerParam(leica_->L_targets, atrCoordinates->Count);


	LMF::Tracker::OVC::ATRCoordinateCollection^ gettargetdirections = atrCoordinates;

// this will get over written by real values, if any . . . 

	leica_->setDoubleParam(leica_->L_X1, 0.0);
	leica_->setDoubleParam(leica_->L_Y1, 0.0);
	leica_->setDoubleParam(leica_->L_X2, 0.0);
	leica_->setDoubleParam(leica_->L_Y2, 0.0);
	leica_->setDoubleParam(leica_->L_X3, 0.0);
	leica_->setDoubleParam(leica_->L_Y3, 0.0);
	leica_->setDoubleParam(leica_->L_X4, 0.0);
	leica_->setDoubleParam(leica_->L_Y4, 0.0);
	leica_->setDoubleParam(leica_->L_X5, 0.0);
	leica_->setDoubleParam(leica_->L_Y5, 0.0);

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
			leica_->setDoubleParam(leica_->L_X1, thing->PixelX->Value);
			leica_->setDoubleParam(leica_->L_Y1, thing->PixelY->Value);
		}
		if (i == 1)
		{
			leica_->setDoubleParam(leica_->L_X2, thing->PixelX->Value);
			leica_->setDoubleParam(leica_->L_Y2, thing->PixelY->Value);
		}
		if (i == 2)
		{
			leica_->setDoubleParam(leica_->L_X3, thing->PixelX->Value);
			leica_->setDoubleParam(leica_->L_Y3, thing->PixelY->Value);
		}
		if (i == 3)
		{
			leica_->setDoubleParam(leica_->L_X4, thing->PixelX->Value);
			leica_->setDoubleParam(leica_->L_Y4, thing->PixelY->Value);
		}
		if (i == 4)
		{
			leica_->setDoubleParam(leica_->L_X5, thing->PixelX->Value);
			leica_->setDoubleParam(leica_->L_Y5, thing->PixelY->Value);
		}


		//		std::cout << std::endl;
	}


	// Some sort of image should be in Image.
//	std::cout << "Height: " << image->Height <<
//		" Width: " << image->Width <<
//		std::endl;

	// Some sort of image should be in Image.
//	std::cout << "Pixel Height: " << image->PixelHeight <<
//		" Pixel Width: " << image->PixelWidth <<
//		std::endl;
//		//SaveBitmap(image, "test.png");

// This prints out the image after converting it to png.
/*
		String^ temp = filenamenumber.ToString();
		FileStream^ fileStream = gcnew FileStream("test" + filenamenumber.ToString() + ".png", FileMode::OpenOrCreate);
		PngBitmapEncoder^ encoder = gcnew PngBitmapEncoder();
		encoder->Frames->Add(BitmapFrame::Create(image));
		encoder->Save(fileStream);
		fileStream->Close();
		std::cout << "png" << std::endl;

// This prints out the image after converting it to tiff, since we have tiff file readers that might help.

		String^ temp2 = filenamenumber.ToString();
		FileStream^ fileStream2 = gcnew FileStream("test" + filenamenumber.ToString() + ".tif", FileMode::OpenOrCreate);
		TiffBitmapEncoder^ encoder2 = gcnew TiffBitmapEncoder();
		encoder2->Frames->Add(BitmapFrame::Create(image));
		encoder2->Save(fileStream2);
		fileStream2->Close();
		std::cout << "tiff" << std::endl;

*/

// this is an attempt to get the raw bitmap values and get it into a byte stream that Area Detector expects
// which is a pain in the . . . 

//	const char* nullTerminatedData;

//	try
//	{
		BitmapImage^ bitmapImage = image;
		WriteableBitmap^ writeableBitmap = gcnew WriteableBitmap(bitmapImage);

		cli::array<unsigned char>^ pixelData = gcnew cli::array<unsigned char>(writeableBitmap->PixelWidth * writeableBitmap->PixelHeight * 4);
		writeableBitmap->CopyPixels(pixelData, writeableBitmap->BackBufferStride, 0);

		// This starts to flip this to greyscale - not sure if I can leave it as-is if I still want color

		std::vector<uint8_t> grayscaleData(writeableBitmap->PixelWidth * writeableBitmap->PixelHeight);
		

	for (int i = 0; i < pixelData->Length; i += 4) {
			float red = static_cast<float>(pixelData[i]);
			float green = static_cast<float>(pixelData[i + 1]);
			float blue = static_cast<float>(pixelData[i + 2]);

			grayscaleData[i / 4] = static_cast<uint8_t>(0.299 * red + 0.587 * green + 0.114 * blue);
		}
		const char* 	nullTerminatedData = reinterpret_cast<const char*>(grayscaleData.data());

		size_t imageDims[2]; 


//////
		NDDataType_t  imageDataType;
//		size_t imageDims[3]; // or 2 for BW
		NDArrayInfo arrayInfo;
		epicsTimeStamp currentTime;

		imageDims[0] = image->PixelWidth;
		imageDims[1] = image->PixelHeight;

//		imageDims[2] = 0;
	
//		imageDataType = NDFloat32;
//		leica_->setIntegerParam(leica_->NDDataType, NDFloat32);

		imageDataType = NDUInt8;
		leica_->setIntegerParam(leica_->NDDataType, NDUInt8);



		/* Update the image */
		/* First release the copy that we held onto last time */
		if (leica_->pArrays[0]) {
			leica_->pArrays[0]->release();
		}

//		std::cout << grayscaleData.size() << " " << 640 * 480 << std::endl;


		/* Allocate a new array */
		leica_->pArrays[0] = leica_->pNDArrayPool->alloc(2, imageDims, imageDataType, 0, NULL);
//		leica_->pArrays[0] = leica_->pNDArrayPool->alloc(3, imageDims, imageDataType, 0, NULL);
		if (leica_->pArrays[0] != NULL) {
			leica_->pImage = leica_->pArrays[0];
			leica_->pImage->getInfo(&arrayInfo);
			// Copy data from the input to the output, correct 

			memcpy(leica_->pImage->pData, nullTerminatedData, grayscaleData.size());
//			memcpy(leica_->pImage->pData, nullTerminatedData, 640 *480 );

			epicsTimeGetCurrent(&currentTime);
			leica_->pImage->timeStamp = currentTime.secPastEpoch + currentTime.nsec / 1.e9;

			leica_->pImage->uniqueId = filenamenumber++;

			leica_->updateTimeStamp(&leica_->pImage->epicsTS);

			/* Get attributes that have been defined for this driver */
			leica_->getAttributes(leica_->pImage->pAttributeList);

			leica_->doCallbacksGenericPointer(leica_->pImage, leica_->NDArrayData, 0);
		}
		leica_->callParamCallbacks();
}


void leica::OnMeasurementArrived(LMF::Tracker::Measurements::MeasurementSettings^ sender, LMF::Tracker::MeasurementResults::MeasurementCollection^ paramMeasurements, LMF::Tracker::ErrorHandling::LmfException^ paramException)
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
				leica_->setDoubleParam(leica_->L_humidity, LastMeasurement->Humidity->Value);
				leica_->setDoubleParam(leica_->L_pressure, LastMeasurement->Pressure->Value);
				leica_->setDoubleParam(leica_->L_temperature, LastMeasurement->Temperature->Value);


				if (StationaryMeasurement3D^ stationaryMeas3D = dynamic_cast<StationaryMeasurement3D^>(LastMeasurement))
				{
					//					std::cout << "I am a stationary3d measurement \n";

					//					std::cout << " X = " << stationaryMeas3D->Position->Coordinate1->Value << " " << (decode)(stationaryMeas3D->Position->Coordinate1->UnitString);
					//					std::cout << " Y = " << stationaryMeas3D->Position->Coordinate2->Value << " " << (decode)(stationaryMeas3D->Position->Coordinate2->UnitString);
					//					std::cout << " Z = " << stationaryMeas3D->Position->Coordinate3->Value << " " << (decode)(stationaryMeas3D->Position->Coordinate3->UnitString) << std::endl;



					leica_->setDoubleParam(leica_->L_x, stationaryMeas3D->Position->Coordinate1->Value);
					leica_->setDoubleParam(leica_->L_y, stationaryMeas3D->Position->Coordinate2->Value);
					leica_->setDoubleParam(leica_->L_z, stationaryMeas3D->Position->Coordinate3->Value);

				}
				else if (StationaryMeasurement6D^ stationaryMeas6D = dynamic_cast<StationaryMeasurement6D^>(LastMeasurement))
				{
					//					std::cout << "I am a stationary6d measurement \n";

					//					std::cout << " X = " << stationaryMeas6D->Position->Coordinate1->Value << " " << (decode)(stationaryMeas6D->Position->Coordinate1->UnitString);
					//					std::cout << " Y = " << stationaryMeas6D->Position->Coordinate2->Value << " " << (decode)(stationaryMeas6D->Position->Coordinate2->UnitString);
					//					std::cout << " Z = " << stationaryMeas6D->Position->Coordinate3->Value << " " << (decode)(stationaryMeas6D->Position->Coordinate3->UnitString) << std::endl;

					leica_->setDoubleParam(leica_->L_x, stationaryMeas3D->Position->Coordinate1->Value);
					leica_->setDoubleParam(leica_->L_y, stationaryMeas3D->Position->Coordinate2->Value);
					leica_->setDoubleParam(leica_->L_z, stationaryMeas3D->Position->Coordinate3->Value);


				}
				else if (SingleShotMeasurement3D^ singleshot3dD = dynamic_cast<SingleShotMeasurement3D^>(LastMeasurement))
				{
					//					std::cout << "I am a singleshot 3d measurement \n";

					//					std::cout << " X = " << singleshot3dD->Position->Coordinate1->Value << " " << (decode)(singleshot3dD->Position->Coordinate1->UnitString);
					//					std::cout << " Y = " << singleshot3dD->Position->Coordinate2->Value << " " << (decode)(singleshot3dD->Position->Coordinate2->UnitString);
					//					std::cout << " Z = " << singleshot3dD->Position->Coordinate3->Value << " " << (decode)(singleshot3dD->Position->Coordinate3->UnitString) << std::endl;
					leica_->setDoubleParam(leica_->L_x, stationaryMeas3D->Position->Coordinate1->Value);
					leica_->setDoubleParam(leica_->L_y, stationaryMeas3D->Position->Coordinate2->Value);
					leica_->setDoubleParam(leica_->L_z, stationaryMeas3D->Position->Coordinate3->Value);


				}
				else if (SingleShotMeasurement6D^ singleshot6dD = dynamic_cast<SingleShotMeasurement6D^>(LastMeasurement))
				{
					//					std::cout << "I am a singleshot 6d measurement \n";

					//					std::cout << " X = " << singleshot6dD->Position->Coordinate1->Value << " " << (decode)(singleshot6dD->Position->Coordinate1->UnitString);
					//					std::cout << " Y = " << singleshot6dD->Position->Coordinate2->Value << " " << (decode)(singleshot6dD->Position->Coordinate2->UnitString);
					//					std::cout << " Z = " << singleshot6dD->Position->Coordinate3->Value << " " << (decode)(singleshot6dD->Position->Coordinate3->UnitString) << std::endl;
					leica_->setDoubleParam(leica_->L_x, stationaryMeas3D->Position->Coordinate1->Value);
					leica_->setDoubleParam(leica_->L_y, stationaryMeas3D->Position->Coordinate2->Value);
					leica_->setDoubleParam(leica_->L_z, stationaryMeas3D->Position->Coordinate3->Value);

				}
			}
		}
	}
	leica_->callParamCallbacks();

	// not sure why This callback has to be restrted while onEnviromnetalValuesChanged does not
// *AND* should probably check if it is even leagal to restart
// *AND* having to put this here means that I also can't actually stop it. Unless I check the epics status to see if it shuld be resytarted or not


//	GlobalObjects::LMFTracker->Measurement->StartMeasurement();

	//	std::cout << reset;
}





