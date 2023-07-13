#include <epicsEvent.h>
#include "ADDriver.h"

#define DRIVER_VERSION      2
#define DRIVER_REVISION     9
#define DRIVER_MODIFICATION 0

#define MIN_DELAY 1e-5

/** Simulation detector driver; demonstrates most of the features that areaDetector drivers can support. */
class epicsShareClass leica : public ADDriver {
public:
    leica(const char *portName, int maxSizeX, int maxSizeY, NDDataType_t dataType,
                int maxBuffers, size_t maxMemory,
                int priority, int stackSize);

    /* These are the methods that we override from ADDriver */
    virtual asynStatus writeInt32(asynUser *pasynUser, epicsInt32 value);
    virtual asynStatus writeFloat64(asynUser *pasynUser, epicsFloat64 value);

    virtual void report(FILE *fp, int details);
    void simTask(); /**< Should be private, but gets called from C, so must be public */

protected:
    int dummyfirst;
    #define FIRST_SIM_DETECTOR_PARAM dummyfirst
    int dummylast; 

private:
    /* These are the methods that are new to this class */
    int computeImage();

    /* Our data */
    epicsEventId startEventId_;
    epicsEventId stopEventId_;
    NDArray *pRaw_;
    NDArray *pBackground_;
    bool useBackground_;
    NDArray *pRamp_;
    NDArray *pPeak_;
    NDArrayInfo arrayInfo_;
};

