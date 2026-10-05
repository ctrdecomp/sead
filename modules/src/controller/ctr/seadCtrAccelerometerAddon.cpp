// Filename: seadCtrAccelerometerAddon.cpp
//
// Project: StandardEAD C++ Library for CTR

#include <controller/ctr/seadCtrAccelerometerAddon.h>
#include <controller/ctr/seadCtrHidDeviceCtr.h>
#include <controller/seadControllerMgr.h>

namespace sead
{
CtrAccelerometerAddon::CtrAccelerometerAddon(Controller* controller):
    AccelerometerAddon(controller),
    mAccelerationStatus()
{
}

CtrAccelerometerAddon::~CtrAccelerometerAddon()
{
}

bool CtrAccelerometerAddon::calc()
{
    CtrHidDevice* ctrDevice = DynamicCast<CtrHidDevice>(static_cast<ControlDevice*>(getController()->getMgr()->getControlDevice(ControllerDefine::cDevice_CtrHid)));

    if (ctrDevice == nullptr)
    {
        mIsEnable = false;
        return false;
    }

    if (ctrDevice->mFlags & CtrHidDevice::cAccelerometer)
    {
        mAcceleration = Vector3f(ctrDevice->mAccelerometerFloat.x, ctrDevice->mAccelerometerFloat.y, ctrDevice->mAccelerometerFloat.z);
        mAccelerationStatus = ctrDevice->mAccelerometerStatus;
        mIsEnable = true;
    }
    else
    {
        mIsEnable = false;
    }

    return true;
}
}