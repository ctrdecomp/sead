// Filename: seadCtrHidDeviceCtr.cpp
//
// Project: StandardEAD C++ Library for CTR

#include <controller/ctr/seadCtrHidDeviceCtr.h>

namespace sead
{
/*
Pad to align for Line nos for assert


*/
CtrHidDevice::CtrHidDevice(ControllerMgr* mgr):
    ControlDevice(mgr),
    mPadReaderPtr(NULL),
    mTouchPanelReaderPtr(NULL),
    mAccelerometerReaderPtr(NULL),
    mGyroscopeReaderPtr(NULL),
#ifdef SEAD_DEBUG
    mDebugPadReaderPtr(NULL),
#endif
    mFlags(0)
{
    nn::Result result = nn::hid::CTR::Initialize();

    SEAD_ASSERT(result.IsSuccess() || result.GetDescription() == nn::Result::DESCRIPTION_ALREADY_INITIALIZED);
    mPadReaderPtr = new (&mMainPad) nn::hid::CTR::PadReader(nn::hid::CTR::GetPad());

    mTouchPanelReaderPtr = new(&mMainTouchPanel) nn::hid::CTR::TouchPanelReader(nn::hid::CTR::GetTouchPanel());
}

void CtrHidDevice::calc()
{
    if (mPadReaderPtr->ReadLatest(&mPadStatus) != false)
    {
        mFlags |= cPadReader;
    }
    else
    {
        mFlags &= ~cPadReader;
    }

    if (mTouchPanelReaderPtr->ReadLatest(&mTouchPanelStatus) != false)
    {
        mFlags |= cTouchPanel;
    }
    else
    {
        mFlags &= ~cTouchPanel;
    }

    if (mAccelerometerReaderPtr != nullptr)
    {
        if (mAccelerometerReaderPtr->ReadLatest(&mAccelerometerStatus) != false)
        {
            mFlags |= cAccelerometer;

            mAccelerometerReaderPtr->ConvertToAcceleration(&mAccelerometerFloat, 1, &mAccelerometerStatus);
        }
        else
        {
            mFlags &= ~cAccelerometer;
        }
    }

    if (mGyroscopeReaderPtr != nullptr)
    {
        if (mGyroscopeReaderPtr->ReadLatest(&mGyroscopeStatus) != false)
        {
            mFlags |= cGyroscope;
        }
        else
        {
            mFlags &= ~cGyroscope;
        }
    }
#ifdef SEAD_DEBUG
    if (mDebugPadReaderPtr != nullptr)
    {
        if (mDebugPadReaderPtr->ReadLatest(&mDebugPadStatus) != false)
        {
            mFlags |= cDebugPadReader;
        }
        else
        {
            mFlags &= ~cDebugPadReader;
        }
    }
#endif
}

}