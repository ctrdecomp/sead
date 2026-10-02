// Filename: seadDoubleGameFrameworkCtrNw4c.cpp
//
// Project: StandardEAD C++ Library for CTR

#include "framework/ctr_nw4c/seadDoubleCmdGameFrameworkCtrNw4c.h"
#include "basis/seadWarning.h"
#include "gfx/ctr/seadGraphicsCtr.h"
#include "filedevice/seadFileDevice.h"
#include "filedevice/seadFileDeviceMgr.h"
#include "framework/seadProcessMeter.h"
#include "heap/seadExpHeap.h"
#include "thread/seadThread.h"

namespace sead
{

DoubleCmdGameFrameworkCtrNw4c* DoubleCmdGameFrameworkCtrNw4c::sInstance = NULL;

DoubleCmdGameFrameworkCtrNw4c::DoubleCmdGameFrameworkCtrNw4c(const CreateArg& arg):
    GameFrameworkCtrNw4c(arg),
    mDoubleDispBufList(0),
    mDoubleDispBufState(cHide),
    mLastDoubleTick(),
    mProcessMeterBar(0),
    mWaitCmdlistDone(false),
    mLastCmdlistDone(false),
    mWaitForVBlink(false)
{
    SEAD_ASSERT(!sInstance);
    sInstance = this;
}

DoubleCmdGameFrameworkCtrNw4c::~DoubleCmdGameFrameworkCtrNw4c()
{
    if(sInstance)
    {
        sInstance = nullptr;
    }
}

void DoubleCmdGameFrameworkCtrNw4c::initialize(const Framework::InitializeArg& arg)
{
    GameFrameworkCtrNw4c::initialize(arg);
}

// FIX ME
void DoubleCmdGameFrameworkCtrNw4c::initializeGraphicsSystem(Heap* heap, const Vector2f& topFbSize, const Vector2f& btmFbSize)
{
    if(mGameArg.mMemoryMgrCtr == NULL)
    {
        {
            ExpHeap* gfxHeap = ExpHeap::create(mGameArg.cmdMemSize, "sead::DefaultGfxMemoryMgrCtr", heap);

            GfxMemoryMgrCtr* mem = new(gfxHeap) DefaultGfxMemoryMgrCtr();
            mem->setInitialize(true);
            initNngx_(mem);
            mem->setInitialize(false);
        }            
    }
    else
    {
        mGameArg.mMemoryMgrCtr->setInitialize(true);
        initNngx_(mGameArg.mMemoryMgrCtr);
        mGameArg.mMemoryMgrCtr->setInitialize(false);
    }

    for(s32 disp = 0; disp < 2; disp++)
    {
        mDoubleBuf[disp] = createCmdlist_(mGameArg.cmdBufSize, mGameArg.cmdBufRequest);
    }

    for(s32 disp = 0; disp < 2; disp++)
    {
        nngxBindDisplaybuffer(mDoubleBuf[disp]);
        nngxSetCmdlistCallback(cmdlistCallback);
        nngxEnableCmdlistCallback(-1);
    }

    nngxBindCmdlist(mDoubleBuf[0]);
    createDisplayBuffers_(mDoubleBufferTop, 2, NN_GX_DISPLAY0, mGameArg.format, mGameArg.widthTop, mGameArg.heightTop, NN_GX_MEM_FCRAM);
    createDisplayBuffers_(mDoubleBufferBtm, 2, NN_GX_DISPLAY1, mGameArg.format, mGameArg.widthBtm, mGameArg.heightBtm, NN_GX_MEM_FCRAM);
    nngxActiveDisplay(NN_GX_DISPLAY1);

    for(int disp = 0; disp < 2; disp++)
    {
        nngxBindDisplaybuffer(mDoubleBufferBtm[disp]);
        GLint p;
        nngxGetDisplaybufferParameteri(NN_GX_DISPLAYBUFFER_ADDRESS, &p);
        mDoubleCmdParam[disp] = p;
    }

    {
        Vector2i bufSize;
        createFramebuffer_(&mBuffer, MathCalcCommon<int>::max(mGameArg.widthTop, mGameArg.widthBtm), MathCalcCommon<int>::max(mGameArg.heightTop, mGameArg.heightBtm),
                            NN_GX_MEM_VRAMA, PICA_DATA_COLOR_RGBA8_OES, NN_GX_MEM_VRAMB, PICA_DATA_DEPTH24_STENCIL8_EXT);

        mTopFrameBuffer = new(heap) FrameBufferCtr(nn::gr::CTR::FrameBuffer(mBuffer), Vector2f(topFbSize.x, topFbSize.y), topFbSize.x, topFbSize.y, mGameArg.physH_Top, mGameArg.physW_Top);
        SEAD_ASSERT(mTopFrameBuffer);
        mTopFrameBuffer->bind();

        mBtmFrameBuffer = new(heap) FrameBufferCtr(nn::gr::CTR::FrameBuffer(mBuffer), Vector2f(btmFbSize.x, btmFbSize.y), btmFbSize.x, btmFbSize.y, mGameArg.physH_Btm, mGameArg.physW_Btm);
        SEAD_ASSERT(mBtmFrameBuffer);
        mTopFrameBuffer->bind();
    }

    Graphics::setInstance(new (heap) GraphicsCtr());
    Graphics::instance()->initialize(heap);

    SEAD_ASSERT_GL();
}

void DoubleCmdGameFrameworkCtrNw4c::mainLoop_()
{
    mVblinkBuf = nngxCheckVSync(NN_GX_DISPLAY_BOTH);
    mFrameNow.setNow();
    mLastDiffTime.setNow();
    ProcessMeter* pInst = ProcessMeter::instance();
    if(pInst)
    {
        pInst->measureBeginFrame();
    }

    for(;;)
    {
        procFrame_();

        if (mWaitCmdlistDone != mLastCmdlistDone)
        {
            continue;
        }
        
        if (mWaitCmdlistDone != 0)
        {
            nngxWaitCmdlistDone();
        }

        mLastCmdlistDone = mWaitCmdlistDone;
    }
}

void DoubleCmdGameFrameworkCtrNw4c::procFrame_()
{ 
    ProcessMeter* pInst = ProcessMeter::instance(); 
    Graphics* pGraphics = Graphics::instance(); 
    
    pGraphics->lockDrawContext();
    {
        procCalc_();
        procDraw_();

        if (mWaitCmdlistDone != 0) 
        { 
            if (mProcessMeterBar == 0) 
            { 
                mGPUMeter.measureBegin(); 
                mProcessMeterBar = 1; 
            } 
            nngxRunCmdlist(); 
            mDoubleDispBufList = 1 - mDoubleDispBufList; 
        }

        presentTop_();
    }
    pGraphics->unlockDrawContext();

    if (mProcessMeterBar != 0) 
    { 
        mGPUMeter.measureEnd(mLastDoubleTick); 
        mProcessMeterBar = 0; 
    }

    if (pInst)
    {
        pInst->measureEndFrame();
    }

    mFrameNow.diffToNow();
    mFrameNow = mLastDiffTime;
    mFrameNow.setNow();

    presentBtm_();

    if (pInst)
    {
        pInst->measureBeginFrame();
    }

    pGraphics->lockDrawContext();
    {
        if (mWaitCmdlistDone == 0)
        { 
            if (mProcessMeterBar == 0) 
            { 
                mGPUMeter.measureBegin(); 
                mProcessMeterBar = 1; 
            } 
            nngxBindCmdlist(mDoubleDispBufList[mDoubleBuf]); 
            nngxRunCmdlist(); 
        }

        mWaitCmdlistDone = 1 - mWaitCmdlistDone; 
        nngxBindCmdlist(mDoubleDispBufList[mDoubleBuf]); 
        mDoubleDispBufList = (mDoubleDispBufList + 1) % 3;
    }
    pGraphics->unlockDrawContext();

    if (getDisplayState() == cReady) 
    { 
        if (mDoubleDispBufState == cShow) 
        { 
            nngxStartLcdDisplay(); 
            mDoubleDispBufState = cShow; 
        } 
    }
    
    if (mDoubleDispBufState <= cShow)
    {
        ++mDoubleDispBufState;
    }
}

void DoubleCmdGameFrameworkCtrNw4c::presentTop_()
{
    requestTransferRenderImage_(mFrameBufferNo[mDoubleBufferTop[0]], &mBuffer, mGameArg.widthTop, mGameArg.heightTop, NN_GX_ANTIALIASE_NOT_USED, 0);
}

void DoubleCmdGameFrameworkCtrNw4c::presentBtm_()
{
    requestTransferRenderImage_(mFrameBufferNo[mDoubleBufferBtm[0]], &mBuffer, mGameArg.widthBtm, mGameArg.heightBtm, NN_GX_ANTIALIASE_NOT_USED, 0);
}

void DoubleCmdGameFrameworkCtrNw4c::swapBuffer_()
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

    mFrameBufferNo[0] = (mFrameBufferNo[0] + 2) % 3;

    nngxActiveDisplay(NN_GX_DISPLAY0);
    nngxBindDisplaybuffer(mDoubleBufferTop[0]);

    nngxActiveDisplay(NN_GX_DISPLAY1);
    nngxBindDisplaybuffer(mDoubleBufferBtm[0]);

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

void DoubleCmdGameFrameworkCtrNw4c::waitForVBlank_()
{
    if (mWaitForVBlink)
    {
        Graphics::instance()->waitForVBlank();
    }

    TickSpan span;
    span.setMilliSeconds(10);

    if (mDoubleDispBufState > cHide)
    {
        for(;;)
        {
            u32 vsync = nngxCheckVSync(NN_GX_DISPLAY_BOTH);

            s32 diff;

            if (vsync > mVblinkBuf)
                diff = (s32)(vsync - mVblinkBuf);
            else
                diff = (s32)(vsync + 0x7FFFFFFF - mVblinkBuf);

            if (mGameArg.wait_vblank <= (u32)diff)
            {
                Graphics::instance()->lockDrawContext();
                {
                    nngxSwapBuffers(NN_GX_DISPLAY_BOTH);
                }
                Graphics::instance()->unlockDrawContext();

                mVblinkBuf = vsync;
                break;
            }

            if (mGameArg.wait_vblank == (u32)(diff + 1))
            {
                Graphics::instance()->lockDrawContext();
                {
                    nngxSwapBuffers(NN_GX_DISPLAY_BOTH);
                }
                Graphics::instance()->unlockDrawContext();

                Graphics::instance()->waitForVBlank();
                break;
            }

            Thread::sleep(span);
        }
    }
    else
    {
        u32 vsync = nngxCheckVSync(NN_GX_DISPLAY_BOTH);

        s32 diff;

        if (vsync > mVblinkBuf)
            diff = (s32)(vsync - mVblinkBuf);
        else
            diff = (s32)(vsync + 0x7FFFFFFF - mVblinkBuf);

        u32 count = (u32)diff;

        while (mGameArg.wait_vblank > count)
        {
            Graphics::instance()->waitForVBlank();
            ++count;
        }

        mVblinkBuf = nngxCheckVSync(NN_GX_DISPLAY_BOTH);
    }

    mLastDoubleTick.setNow();
}

void DoubleCmdGameFrameworkCtrNw4c::doScreenShotImpl_(char const* shot)
{
    FileDeviceMgr* fMgr = FileDeviceMgr::instance();
    FixedSafeString<264> str;

    {
        str.copy(shot);
        str.append("_top.bmp");
        FileHandle topHandle;
        fMgr->open(&topHandle, str, FileDevice::cFileOpenFlag_WriteOnly, 0);
        if(!topHandle.isOpened())
        {
            SEAD_WARNING("Can't open file handle(%s). Can't save screen-shot.\n", shot);
        }
        nngxBindDisplaybuffer(mDoubleBufferTop[0]);
        GLint param;
        nngxGetDisplaybufferParameteri(NN_GX_DISPLAYBUFFER_ADDRESS, &param);
        saveScreenShotToFileHandle_(&topHandle, &param, mGameArg.widthTop, mGameArg.heightTop, mGameArg.format);
    }

    {
        str.copy(shot);
        str.append("_btm.bmp");
        FileHandle btmHandle;
        fMgr->open(&btmHandle, str, FileDevice::cFileOpenFlag_WriteOnly, 0);
        if(!btmHandle.isOpened())
        {
            SEAD_WARNING("Can't open file handle(%s). Can't save screen-shot.\n", shot);
        }
        nngxBindDisplaybuffer(mDoubleBufferBtm[0]);
        GLint param;
        nngxGetDisplaybufferParameteri(NN_GX_DISPLAYBUFFER_ADDRESS, &param);
        saveScreenShotToFileHandle_(&btmHandle, &param, mGameArg.widthBtm, mGameArg.heightBtm, mGameArg.format);
    }
}

void DoubleCmdGameFrameworkCtrNw4c::cmdlistCallbackImpl_(GLint list)
{
    return mLastDoubleTick.setNow();
}

void DoubleCmdGameFrameworkCtrNw4c::cmdlistCallback(GLint list)
{
    sInstance->cmdlistCallbackImpl_(list);
}
}