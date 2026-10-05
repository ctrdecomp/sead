// Filename: seadUlcdGameFrameworkCtrNw4c.cpp
//
// Project: StandardEAD C++ Library for CTR

#include "framework/ctr_nw4c/seadUlcdGameFrameworkCtrNw4c.h"
#include "basis/seadWarning.h"
#include "gfx/ctr/seadGraphicsCtr.h"
#include "filedevice/seadFileDevice.h"
#include "filedevice/seadFileDeviceMgr.h"
#include "framework/seadProcessMeter.h"
#include "framework/seadUlcdMethodTreeMgr.h"
#include "heap/seadHeap.h"
#include "thread/seadThread.h"

namespace sead
{

UlcdGameFrameworkCtrNw4c::UlcdGameFrameworkCtrNw4c(const CreateArg& arg):
    GameFrameworkCtrNw4c(arg)
{
}

UlcdGameFrameworkCtrNw4c::~UlcdGameFrameworkCtrNw4c()
{
}

void UlcdGameFrameworkCtrNw4c::initialize(const Framework::InitializeArg& arg)
{
    GameFrameworkCtrNw4c::initialize(arg);
}

void UlcdGameFrameworkCtrNw4c::initializeGraphicsSystem(Heap* heap, const Vector2f& leftFbSize, const Vector2f& rightFbSize)
{
    ScopedCurrentHeapSetter chs(heap);
    GameFrameworkCtrNw4c::initializeGraphicsSystem(heap, leftFbSize, rightFbSize);
    createDisplayBuffers_(mDisplayBufferRight, 2, NN_GX_DISPLAY1, mGameArg.format, mGameArg.widthTop, mGameArg.heightTop, NN_GX_MEM_FCRAM);
    setUlcdEnable(true);
}

MethodTreeMgr* UlcdGameFrameworkCtrNw4c::createMethodTreeMgr_(Heap* heap)
{
    return new(heap) UlcdMethodTreeMgr();
}

void UlcdGameFrameworkCtrNw4c::procDraw_()
{
    mDrawMeter.measureBegin();
    {
        UlcdMethodTreeMgr* method = DynamicCast<UlcdMethodTreeMgr>(getMethodTreeMgr());

        // Left Screen
        clearFrameBuffers_(12);
        mTopFrameBuffer->bind();
        method->drawLeft();
        presentLeft_();

        // Eight Screen
        clearFrameBuffers_(15);
        mBtmFrameBuffer->bind();
        method->drawRight();
        presentRight_();

        mTopFrameBuffer->bind();
    }
    mDrawMeter.measureEnd();
}

void UlcdGameFrameworkCtrNw4c::doScreenShotImpl_(char const* shot)
{
    FileDeviceMgr* fMgr = FileDeviceMgr::instance();
    FixedSafeString<264> str;

    {
        str.copy(shot);
        str.append("_right.bmp");
        FileHandle rightHandle;
        fMgr->open(&rightHandle, str, FileDevice::cFileOpenFlag_WriteOnly, 0);
        if(!rightHandle.isOpened())
        {
            SEAD_WARNING("Can't open file handle(%s). Can't save screen-shot.\n", shot);
        }
        nngxBindDisplaybuffer(mDisplayBufferRight[0]);
        GLint param;
        nngxGetDisplaybufferParameteri(NN_GX_DISPLAYBUFFER_ADDRESS, &param);
        saveScreenShotToFileHandle_(&rightHandle, &param, mGameArg.widthTop, mGameArg.heightTop, mGameArg.format);
    }
}

void UlcdGameFrameworkCtrNw4c::presentLeft_()
{
    presentTop_();
}

void UlcdGameFrameworkCtrNw4c::presentRight_()
{
    requestTransferRenderImage_(mFrameBufferNo[mDisplayBufferRight[0]], &mBuffer, mGameArg.widthBtm, mGameArg.heightBtm, NN_GX_ANTIALIASE_NOT_USED, 0);
}

void UlcdGameFrameworkCtrNw4c::setUlcdEnable(bool enable)
{
    if(enable)
    {
        nngxSetDisplayMode(NN_GX_DISPLAYMODE_STEREO);
    }
    else
    {
        nngxSetDisplayMode(NN_GX_DISPLAYMODE_NORMAL);
    }
}

FrameBuffer* UlcdGameFrameworkCtrNw4c::getMethodFrameBuffer(s32 methodType) const
{
    switch (methodType) 
    { 
    case 11: 
    case 12: 
    case 13: 
    case 14: 
    case 15: 
    case 16:
    {
        return mTopFrameBuffer; 
    }
    default: 
    {
        return GameFrameworkCtrNw4c::getMethodFrameBuffer(methodType); 
    } 
    }
}

} // namespace sead