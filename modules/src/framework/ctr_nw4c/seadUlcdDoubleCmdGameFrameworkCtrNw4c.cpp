// Filename: seadUlcdDoubleGameFrameworkCtrNw4c.cpp
//
// Project: StandardEAD C++ Library for CTR

#include "framework/ctr_nw4c/seadUlcdDoubleCmdGameFrameworkCtrNw4c.h"
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

UlcdDoubleCmdGameFrameworkCtrNw4c::UlcdDoubleCmdGameFrameworkCtrNw4c(const CreateArg& arg):
    DoubleCmdGameFrameworkCtrNw4c(arg)
{
}

UlcdDoubleCmdGameFrameworkCtrNw4c::~UlcdDoubleCmdGameFrameworkCtrNw4c()
{
}

void UlcdDoubleCmdGameFrameworkCtrNw4c::initialize(const Framework::InitializeArg& arg)
{
    DoubleCmdGameFrameworkCtrNw4c::initialize(arg);
}

void UlcdDoubleCmdGameFrameworkCtrNw4c::initializeGraphicsSystem(Heap* heap, const Vector2f& leftFbSize, const Vector2f& rightFbSize)
{
    ScopedCurrentHeapSetter chs(heap);
    DoubleCmdGameFrameworkCtrNw4c::initializeGraphicsSystem(heap, leftFbSize, rightFbSize);
    createDisplayBuffers_(mDisplayBufferRight, 2, NN_GX_DISPLAY1, mGameArg.format, mGameArg.widthTop, mGameArg.heightTop, NN_GX_MEM_FCRAM);
    setUlcdEnable(true);
}

MethodTreeMgr* UlcdDoubleCmdGameFrameworkCtrNw4c::createMethodTreeMgr_(Heap* heap)
{
    return new(heap) UlcdMethodTreeMgr();
}

void UlcdDoubleCmdGameFrameworkCtrNw4c::procDraw_()
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

void UlcdDoubleCmdGameFrameworkCtrNw4c::swapBuffer_()
{
    if (mDoubleDispBufState <= 0)
        return;

    mDoubleDispBufList = 1 - mDoubleDispBufList;

    nngxBindCmdlist(mDoubleBuf[mDoubleDispBufList]);

    s32 param;
    nngxGetCmdlistParameteri(NN_GX_CMDLIST_IS_RUNNING, &param);

    if (param)
        nngxWaitCmdlistDone();

    nngxStopCmdlist();
    nngxClearCmdlist();

    s32 buffer = (mFrameBufferNo[0] + 2) % 3;

    nngxActiveDisplay(NN_GX_DISPLAY0);
    nngxBindDisplaybuffer(mDoubleBufferTop[buffer]);

    nngxActiveDisplay(NN_GX_DISPLAY0_RIGHT);
    nngxBindDisplaybuffer(mDisplayBufferRight[buffer]);

    nngxActiveDisplay(NN_GX_DISPLAY1);
    nngxBindDisplaybuffer(mDoubleBufferBtm[buffer]);

#ifdef SEAD_DEBUG
    if(mException != nullptr)
    {
        mExcpetion->getDirectPrint()->changeDisplaybuffer(mDoubleCmdParam[mFrameBufferNo]);
    }

    if(mScreenshotBuf != nullptr)
    {
        doScreenShot_(mScreenshotBuf);
        mScreenshotBuf = nullptr;
    }

    if (!mLastCmdlistDone)
    {
        nngxBindCmdlist(mDoubleBufTop[mDoubleDispBufList]);
    }
#endif
}

void UlcdDoubleCmdGameFrameworkCtrNw4c::doScreenShotImpl_(char const* shot)
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

void UlcdDoubleCmdGameFrameworkCtrNw4c::presentLeft_()
{
    presentTop_();
}

void UlcdDoubleCmdGameFrameworkCtrNw4c::presentRight_()
{
    requestTransferRenderImage_(mFrameBufferNo[mDisplayBufferRight[0]], &mBuffer, mGameArg.widthBtm, mGameArg.heightBtm, NN_GX_ANTIALIASE_NOT_USED, 0);
}

void UlcdDoubleCmdGameFrameworkCtrNw4c::setUlcdEnable(bool enable)
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

FrameBuffer* UlcdDoubleCmdGameFrameworkCtrNw4c::getMethodFrameBuffer(s32 methodType) const
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

}