// Filename: seadDebugCtrControllerCtr.cpp
//
// Project: StandardEAD C++ Library for CTR
//
// TODO: CHECK THIS

#include <controller/ctr/seadCtrDebugControllerCtr.h>
#include <controller/seadControllerMgr.h>

namespace sead
{
#ifdef SEAD_DEBUG
CtrDebugController::CtrDebugController(ControllerMgr* mgr):
    Controller(mgr)
{
    mId = ControllerDefine::cController_CtrDebug;
    mLeftStickCrossStartBit  = -1;
    mRightStickCrossStartBit = -1;
}

void CtrDebugController::calcImpl_()
{
    #if defined(SEAD_CTRCONTROLLER_CALCIMPL_STATIC)
        static CtrHidDevice* ctrDevice = DynamicCast<CtrHidDevice>(static_cast<ControlDevice*>(getMgr()->getControlDevice(ControllerDefine::cDevice_CtrHid)));
    #else
        CtrHidDevice* ctrDevice = DynamicCast<CtrHidDevice>(static_cast<ControlDevice*>(getMgr()->getControlDevice(ControllerDefine::cDevice_CtrHid)));
    #endif

    if (ctrDevice == nullptr)
        return;

    if ((ctrDevice->mFlags & CtrHidDevice::cDebugPadReader) == 0)
        return;

    const u16 buttons = ctrDevice->mDebugPadStatus.hold;

    mPadHold.reset(0xFFFFFFFF);

    if (buttons & nn::hid::CTR::DEBUG_PAD_BUTTON_UP)
        mPadHold.set(0x00000001);

    if (buttons & nn::hid::CTR::DEBUG_PAD_BUTTON_LEFT)
        mPadHold.set(0x00000002);

    if (buttons & nn::hid::CTR::DEBUG_PAD_TRIGGER_ZR)
        mPadHold.set(0x00000008);

    if (buttons & nn::hid::CTR::DEBUG_PAD_BUTTON_X)
        mPadHold.set(0x00000010);

    if (buttons & nn::hid::CTR::DEBUG_PAD_BUTTON_A)
        mPadHold.set(0x00004000);

    if (buttons & nn::hid::CTR::DEBUG_PAD_BUTTON_Y)
        mPadHold.set(0x00002000);

    if (buttons & nn::hid::CTR::DEBUG_PAD_BUTTON_B)
        mPadHold.set(0x00000020);

    if (buttons & nn::hid::CTR::DEBUG_PAD_TRIGGER_ZL)
        mPadHold.set(0x00000004);

    if (buttons & nn::hid::CTR::DEBUG_PAD_RESERVED)
        mPadHold.set(0x00000C00);

    if (buttons & nn::hid::CTR::DEBUG_PAD_TRIGGER_R)
        mPadHold.set(0x00001200);

    if (buttons & nn::hid::CTR::DEBUG_PAD_BUTTON_PLUS)
        mPadHold.set(0x00000100);

    if (buttons & nn::hid::CTR::DEBUG_PAD_BUTTON_HOME)
        mPadHold.set(0x00080000);

    if (buttons & nn::hid::CTR::DEBUG_PAD_BUTTON_MINUS)
        mPadHold.set(0x00040000);

    if (buttons & nn::hid::CTR::DEBUG_PAD_TRIGGER_L)
        mPadHold.set(0x00010000);

    if (buttons & nn::hid::CTR::DEBUG_PAD_BUTTON_DOWN)
        mPadHold.set(0x00040000);

    if (buttons & nn::hid::CTR::DEBUG_PAD_BUTTON_RIGHT)
        mPadHold.set(0x00010000);

    mLeftStick = Vector2f(ctrDevice->mDebugPadStatus.leftStickX, ctrDevice->mDebugPadStatus.leftStickY);
    mRightStick = Vector2f(ctrDevice->mDebugPadStatus.rightStickX, ctrDevice->mDebugPadStatus.rightStickY);
    mLeftAnalogTrigger = 0.0f;
    mRightAnalogTrigger = 0.0f;
}

bool CtrDebugController::isConnected() const
{
    #if defined(SEAD_CTRCONTROLLER_STATIC)
        static CtrHidDevice* ctrDevice = DynamicCast<CtrHidDevice>(getMgr()->getControlDevice(ControllerDefine::cDevice_CtrHid));
    #else
        CtrHidDevice* ctrDevice = DynamicCast<CtrHidDevice>(getMgr()->getControlDevice(ControllerDefine::cDevice_CtrHid));
    #endif

    if (ctrDevice != nullptr)
    {
        return (ctrDevice->mFlags & CtrHidDevice::cDebugPadReader) != 0;
    }

    SEAD_ASSERT_MSG(false, "cannot get device");
    return false;
}
#endif
}
