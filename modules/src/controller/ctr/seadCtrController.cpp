// Filename: seadCtrController.cpp
//
// Project: StandardEAD C++ Library for CTR
//
// TODO: CHECK THIS

#include <controller/ctr/seadCtrController.h>
#include <controller/seadControllerMgr.h>

namespace sead
{
CtrController::CtrController(ControllerMgr* mgr):
    Controller(mgr)
{
    mId = ControllerDefine::cController_Ctr;
    mLeftStickCrossStartBit  = -1;
    mRightStickCrossStartBit = -1;
}

void CtrController::calcImpl_()
{
    CtrHidDevice* ctrDevice = DynamicCast<CtrHidDevice>(static_cast<ControlDevice*>(getMgr()->getControlDevice(ControllerDefine::cDevice_CtrHid)));

    if (ctrDevice == nullptr)
        return;

    const u32 buttons = ctrDevice->mPadStatus.hold;

    mPadHold.makeAllZero();

    if (buttons & nn::hid::CTR::BUTTON_A)
        mPadHold.set(0x00000001);
    else
        mPadHold.reset(0x00000001);

    // Button B
    if (buttons & nn::hid::CTR::BUTTON_B)
        mPadHold.set(0x00000002);
    else
        mPadHold.reset(0x00000002);

    // Select
    if (buttons & nn::hid::CTR::BUTTON_SELECT)
        mPadHold.set(0x00001000);
    else
        mPadHold.reset(0x00001000);

    // Start
    if (buttons & nn::hid::CTR::BUTTON_START)
        mPadHold.set(0x00000800);
    else
        mPadHold.reset(0x00000800);

    // Right
    if (buttons & nn::hid::CTR::BUTTON_RIGHT)
        mPadHold.set(0x00040000);
    else
        mPadHold.reset(0x00040000);

    // Left
    if (buttons & nn::hid::CTR::BUTTON_LEFT)
        mPadHold.set(0x00080000);
    else
        mPadHold.reset(0x00080000);

    // Up
    if (buttons & nn::hid::CTR::BUTTON_UP)
        mPadHold.set(0x00010000);
    else
        mPadHold.reset(0x00010000);

    // Down
    if (buttons & nn::hid::CTR::BUTTON_DOWN)
        mPadHold.set(0x00020000);
    else
        mPadHold.reset(0x00020000);

    // R
    if (buttons & nn::hid::CTR::BUTTON_R)
        mPadHold.set(0x00004000);
    else
        mPadHold.reset(0x00004000);

    // L
    if (buttons & nn::hid::CTR::BUTTON_L)
        mPadHold.set(0x00002000);
    else
        mPadHold.reset(0x00002000);

    // X
    if (buttons & nn::hid::CTR::BUTTON_X)
        mPadHold.set(0x00000008);
    else
        mPadHold.reset(0x00000008);

    // Y
    if (buttons & nn::hid::CTR::BUTTON_Y)
        mPadHold.set(0x00000010);
    else
        mPadHold.reset(0x00000010);

    // Debug
    if (buttons & nn::hid::CTR::BUTTON_DEBUG)
        mPadHold.set(0x00000080);
    else
        mPadHold.reset(0x00000080);

    // Reserved
    if (buttons & nn::hid::CTR::BUTTON_RESERVED)
        mPadHold.set(0x00001000);
    else
        mPadHold.reset(0x00001000);

    // Emulation Right
    if (buttons & nn::hid::CTR::BUTTON_EMULATION_RIGHT)
        mPadHold.set(0x00800000);
    else
        mPadHold.reset(0x00800000);

    // Emulation Left
    if (buttons & nn::hid::CTR::BUTTON_EMULATION_LEFT)
        mPadHold.set(0x00400000);
    else
        mPadHold.reset(0x00400000);

    // Emulation Up
    if (buttons & nn::hid::CTR::BUTTON_EMULATION_UP)
        mPadHold.set(0x00100000);
    else
        mPadHold.reset(0x00100000);

    // Emulation Down
    if (buttons & nn::hid::CTR::BUTTON_EMULATION_DOWN)
        mPadHold.set(0x00200000);
    else
        mPadHold.reset(0x00200000);

    ctrDevice->mPadReaderPtr->NormalizeStickWithScale(mLeftStick, mRightStick, ctrDevice->mPadStatus.stick.x, ctrDevice->mPadStatus.stick.y);

    if (ctrDevice->mFlags & CtrHidDevice::cTouchPanel)
    {
        const u16 touchX = ctrDevice->mTouchPanelStatus.x;
        const u16 touchY = ctrDevice->mTouchPanelStatus.y;

        const u8 touch = ctrDevice->mTouchPanelStatus.touch;

        setPointerWithBound_(true, touch == 1, Vector2f(static_cast<f32>(touchX), static_cast<f32>(touchY)));
    }
}

void CtrController::setStickClampMode(PadReader::StickClampMode mode)
{
    CtrHidDevice* ctrDevice = DynamicCast<CtrHidDevice>(static_cast<ControlDevice*>(getMgr()->getControlDevice(ControllerDefine::cDevice_CtrHid)));
    if(ctrDevice != NULL)
    {
        ctrDevice->mPadReaderPtr->SetStickClampMode(mode);
    }
}
}