#pragma once

#include <controller/seadController.h>
#include <controller/SeadControlDevice.h>
#include <nn/hid.h>

namespace sead
{
class CtrHidDevice : public ControlDevice
{
    SEAD_RTTI_OVERRIDE(CtrHidDevice, ControlDevice)
public:
    enum CtrControllerFlag
    {
        cNull           = (0 << 1),
        cPadReader      = (1 << 0),
        cTouchPanel     = (1 << 1),
        cAccelerometer  = (1 << 2),
        cGyroscope      = (1 << 3),
        cDebugPadReader = (1 << 4)
    };

    CtrHidDevice(ControllerMgr* mgr);
    virtual ~CtrHidDevice() { }
    virtual void calc();

    PadReader mMainPad;
    TouchPanelReader mMainTouchPanel;
    GyroscopeReader mMainGyroscope;

    PadReader* mPadReaderPtr;
    PadStatus mPadStatus;
    TouchPanelReader* mTouchPanelReaderPtr;
    TouchPanelStatus mTouchPanelStatus;
    AccelerometerReader* mAccelerometerReaderPtr;
    AccelerometerStatus mAccelerometerStatus;
    AccelerationFloat mAccelerometerFloat;
    GyroscopeReader* mGyroscopeReaderPtr;
    GyroscopeStatus mGyroscopeStatus;
#ifdef SEAD_DEBUG
    DebugPadReader* mDebugPadReaderPtr;
    DebugPadStatus mDebugPadStatus;
#endif
    u32 mFlags;
};
}
