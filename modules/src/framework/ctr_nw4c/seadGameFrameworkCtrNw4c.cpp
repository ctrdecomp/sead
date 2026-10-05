// Filename: seadGameFrameworkCtrNw4c.cpp
//
// Project: StandardEAD C++ Library for CTR

#include "framework/ctr_nw4c/seadGameFrameworkCtrNw4c.h"
#include "basis/seadWarning.h"
#include "devenv/ctr/seadExceptionScreenCtr.h"
#include "framework/seadDualScreenMethodTreeMgr.h"
#include "framework/seadInfLoopChecker.h"
#include "framework/seadProcessMeter.h"
#include "filedevice/seadFileDevice.h"
#include "filedevice/seadFileDeviceMgr.h"
#include "gfx/ctr/seadGraphicsCtr.h"
#include "heap/seadExpHeap.h"
#include "stream/seadFileDeviceStream.h"
#include "tentative/seadBitmapBuilder.h"

namespace
{
static u32 getPicaDataColorSize_(PicaDataColor color)
{
    switch(color)
    {
    case PICA_DATA_COLOR_RGBA8_OES:
    {
        return 4;
    }
    case PICA_DATA_COLOR_RGB5_A1:
    case PICA_DATA_COLOR_RGB565:
    case PICA_DATA_COLOR_RGBA4:
    {
        return 2;
    }
    default:
    {
        SEAD_ASSERT_MSG(false, "Undefined PicaDataColor(%d)", color);
        return 4;
    }
    }
}

u32 getPicaDataDepthSize_(PicaDataDepth depth)
{
    switch (depth)
    {
    case PICA_DATA_DEPTH_COMPONENT16:
    {
        return 2;
    }
    case PICA_DATA_DEPTH_COMPONENT24_OES:
    {
        return 3;
    }
    case PICA_DATA_DEPTH24_STENCIL8_EXT:
    {
        return 4;
    }
    default:
    {
        SEAD_ASSERT_MSG(false, "Undefined PicaDataColor(%d)", depth);
        return 4;
    }
    }
}

GLenum picaDataColor2GLenum_(PicaDataColor color)
{
    switch (color)
    {
    case PICA_DATA_COLOR_RGBA8_OES:
        return GL_RGBA8_OES;
    case PICA_DATA_COLOR_RGB5_A1:
        return GL_RGB5_A1;
    case PICA_DATA_COLOR_RGB565:
        return GL_RGB565;
    case PICA_DATA_COLOR_RGBA4:
        return GL_RGBA4;
    default:
        SEAD_ASSERT_MSG(false, "Undefined PicaDataColor(%d)", color);
        return 0;
    }
}
}
namespace sead
{
GfxMemoryMgrCtr* GameFrameworkCtrNw4c::sMemoryMgr = NULL;

GameFrameworkCtrNw4c::GameFrameworkCtrNw4c(CreateArg const& arg):
    GameFramework(),
    mGameArg(arg),
    mVblinkBuf(0),
    mLastUpdateTime(),
    mFrameNow(),
    mLastDiffTime(),
    mBuffer(),
    mTopFrameBuffer(nullptr),
    mBtmFrameBuffer(nullptr),
    mFrameBufferNo(),
    mScreenshotBuf(nullptr),
    mCurrentScreenshot(mGameArg.mScreenShotBuff),

#ifdef SEAD_DEBUG
    mExceptionScreen(nullptr),
#endif
    mGLDispParam(nullptr)
{
}

void GameFrameworkCtrNw4c::initialize(const Framework::InitializeArg& arg)
{
    GameFramework::initialize(arg);
}

// FIX ME
void GameFrameworkCtrNw4c::initializeGraphicsSystem(Heap* heap, const Vector2f& topFbSize, const Vector2f& btmFbSize)
{
    if(mGameArg.mMemoryMgrCtr == NULL)
    {
        {
            ExpHeap* gfxHeap = ExpHeap::create(mGameArg.cmdMemSize, "sead::DefaultGfxMemoryMgrCtr", heap);

            GfxMemoryMgrCtr* mem = new(gfxHeap) DefaultGfxMemoryMgrCtr(gfxHeap);
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
    mGameArg.cmdBufSize = createCmdlist_(mGameArg.cmdBufSize, mGameArg.cmdBufRequest);

    createDisplayBuffers_(mBufferSizeTop, 2, NN_GX_DISPLAY0, mGameArg.format, mGameArg.widthTop, mGameArg.heightTop, NN_GX_MEM_FCRAM);
    createDisplayBuffers_(mBufferSizeBtm, 2, NN_GX_DISPLAY1, mGameArg.format, mGameArg.widthTop, mGameArg.heightTop, NN_GX_MEM_FCRAM);
    nngxActiveDisplay(NN_GX_DISPLAY1);

    for(int disp = 0; disp < 2; disp++)
    {
        nngxBindDisplaybuffer(mBufferSizeBtm[disp]);
        GLint p;
        nngxGetDisplaybufferParameteri(NN_GX_DISPLAYBUFFER_ADDRESS, &p);
        mGLDispParam[disp] = p;
    }
    
    {
        Vector2i bufSize;
        createFramebuffer_(&mBuffer, MathCalcCommon<int>::max(mGameArg.widthTop, mGameArg.widthTop), MathCalcCommon<int>::max(mGameArg.heightTop, mGameArg.heightTop),
                            NN_GX_MEM_VRAMA, PICA_DATA_COLOR_RGBA8_OES, NN_GX_MEM_VRAMB, PICA_DATA_DEPTH24_STENCIL8_EXT);


        mTopFrameBuffer = new(heap) FrameBufferCtr(nn::gr::CTR::FrameBuffer(mBuffer), Vector2f(topFbSize.x, topFbSize.y), topFbSize.x, topFbSize.y, mGameArg.physH_Top, mGameArg.physW_Top);
        SEAD_ASSERT(mTopFrameBuffer);
        mTopFrameBuffer->bind();
        
        mBtmFrameBuffer = new(heap) FrameBufferCtr(nn::gr::CTR::FrameBuffer(mBuffer), Vector2f(btmFbSize.x, btmFbSize.y), btmFbSize.x, btmFbSize.y, mGameArg.physH_Btm, mGameArg.physW_Btm);
        SEAD_ASSERT(mBtmFrameBuffer);
        mTopFrameBuffer->bind();
    }

    GraphicsCtr::setInstance(new(heap) GraphicsCtr());
    Graphics::instance()->initialize(heap);

    SEAD_ASSERT_GL();
}

void GameFrameworkCtrNw4c::runImpl_()
{
    waitStartDisplayLoop_();
#ifdef SEAD_DEBUG
    if(mExceptionScreen != nullptr)
    {
        mExceptionScreen->enableAssertCallback();
    }
#endif
    mainLoop_();
}

void GameFrameworkCtrNw4c::initNngx_(GfxMemoryMgrCtr* mem)
{
    SEAD_ASSERT(mem);

    sMemoryMgr = mem;
    SEAD_ASSERT_MSG(nngxInitialize(allocate, deallocate), "nngxInitialize failed(%d).", 0);
}

GLsizei GameFrameworkCtrNw4c::createCmdlist_(GLsizei bufsize, GLsizei requestcount)
{
    GLuint list;
    nngxGenCmdlists(1, &list);
    nngxBindCmdlist(list);
    nngxCmdlistStorage(bufsize, requestcount);
    SEAD_ASSERT_GL();
    return list;
}

u32 GameFrameworkCtrNw4c::createDisplayBuffers_(u32* dst, u32 num, u32 disp, GLenum format, s32 width, s32 height, u32 buffer)
{
    SEAD_ASSERT(dst);
    SEAD_ASSERT(num > 0);
    SEAD_ASSERT(( width % 8 ) == 0);
    SEAD_ASSERT(( height % 8 ) == 0);
    nngxGenDisplaybuffers(num, dst);
    nngxActiveDisplay(disp);
    SEAD_ASSERT_GL();
    for(int i = 0; i < num; i++)
    {
        nngxBindDisplaybuffer(dst[i]);
        nngxDisplaybufferStorage(format, width, height, buffer);
        SEAD_ASSERT_GL();
    }
    return num;
}

void GameFrameworkCtrNw4c::createFramebuffer_(nn::gr::CTR::FrameBuffer* buffer, s32 width, s32 height, u32 vram_a, 
        PicaDataColor color_area, u32 vram_b, PicaDataDepth depth_area)
{
    SEAD_ASSERT(( width % 8 ) == 0);
    SEAD_ASSERT(( height % 8 ) == 0);
    SEAD_ASSERT(( color_area == NN_GX_MEM_VRAMA ) || ( color_area == NN_GX_MEM_VRAMB ));
    SEAD_ASSERT(( depth_area == NN_GX_MEM_VRAMA ) || ( depth_area == NN_GX_MEM_VRAMB ));
    buffer->colorBuffer.virtualAddr = sMemoryMgr->allocate(vram_a, NN_GX_MEM_RENDERBUFFER, 0, reinterpret_cast<Heap*>(getPicaDataColorSize_(color_area) * width * height));
    buffer->colorBuffer.format = color_area;
    buffer->colorBuffer.blockSize = nn::gr::CTR::FrameBuffer::BLOCK_SIZE8;
    buffer->colorBuffer.width = width;
    buffer->colorBuffer.height = height;

    buffer->depthStencilBuffer.virtualAddr = sMemoryMgr->allocate(vram_b, NN_GX_MEM_RENDERBUFFER, 0, reinterpret_cast<Heap*>(getPicaDataDepthSize_(depth_area) * width * height));
    buffer->depthStencilBuffer.format = depth_area;
    buffer->depthStencilBuffer.width = width;
    buffer->depthStencilBuffer.height = height;

    buffer->width = width;
    buffer->height = height;
}

void GameFrameworkCtrNw4c::initRun_(Heap* heap)
{
#ifdef SEAD_DEBUG
    if(mGameArg.mScreenShotBuff != NULL)
    {
        {
            ExpHeap* ssHeap = ExpHeap::create(0, "sead::ScreenShotBuffer", heap);

            mScreenshotBuf = new (ssHeap) char[mGameArg.mScreenShotBuff];
            ssHeap->adjust();
        }
    }
    if(mExceptionScreen == NULL)
    {
        {
            ExpHeap* exceptionHeap = ExpHeap::create(0, "sead::ExceptionScreenCtr", heap);

            mExceptionScreen = new (exceptionHeap) ExceptionScreenCtr();
            ssHeap->adjust();
        }
    }
    if(mExceptionScreen != NULL)
    {
        mExceptionScreen->getDirectPrint()->changeDisplayBuffer(NULL, mGameArg.format, Vector2i(mGameArg.widthB, mGameArg.heightB), BoundBox2<int>(Vector2i(0, 0), Vector2i(0, mGameArg.heightB + NN_GX_DISPLAY1_HEIGHT * mGameArg.heightB)));
    }
#endif
}

void GameFrameworkCtrNw4c::mainLoop_()
{
    mVblinkBuf = nngxCheckVSync(NN_GX_DISPLAY_BOTH);
    mFrameNow.setNow();
    mLastDiffTime.setNow();

    Graphics::instance()->lockDrawContext();
    {
        procCalc_();
    }
    Graphics::instance()->unlockDrawContext();

    for(;;)
    {
        procFrame_();
    }
}

void GameFrameworkCtrNw4c::saveScreenShotToFileHandle_(FileHandle* handle, void* buffer, int width, int height, u32 format)
{
    if (!handle->isOpened())
    {
        SEAD_WARNING("Can't open file handle. Can't save screen-shot.");
    }

    if (buffer == nullptr)
    {
        SEAD_WARNING(mScreenshotBuf, "No temp-buffer availavle.\n");
    }

    FileDeviceWriteStream fileStream(handle, false);
    BufferWriteStream bufferStream(static_cast<WriteStream*>(&fileStream), mCurrentScreenshot, reinterpret_cast<u32>(mGameArg.mScreenShotBuff));

    BitmapBuilder bitmap(&bufferStream, width, height);

    bitmap.writeFileHeader();
    bitmap.writeInfoHeader();

    const u8* image = static_cast<const u8*>(buffer);
    
    switch (format)
    {
    case GL_RGBA8_OES:
    {
        for (int y = height - 1; y >= 0; --y)
        {
            bitmap.writeImageLineABGR8(static_cast<u8*>(buffer) + y * width * 4, width);
        }
        break;
    }

    case GL_RGB8_OES:
    {
        for (int y = height - 1; y >= 0; --y)
        {
            bitmap.writeImageLineBGR8(static_cast<u8*>(buffer) + y * width * 3, width);
        }
        break;
    }

    case GL_RGBA4:
    {
        for (int y = height - 1; y >= 0; --y)
        {
            bitmap.writeImageLineABGR4(static_cast<u8*>(buffer) + y * width * 2, width);
        }
        break;
    }

    case GL_RGB565:
    {
        for (int y = height - 1; y >= 0; --y)
        {
            bitmap.writeImageLineBGR565(static_cast<u8*>(buffer) + y * width * 2,width);
        }
        break;
    }

    case GL_RGB5_A1:
    {
        for (int y = height - 1; y >= 0; --y)
        {
            bitmap.writeImageLineA1BGR5(static_cast<u8*>(buffer) + y * width * 2, width);
        }
        break;
    }

    default:
        SEAD_ASSERT_MSG(false, "Undefined format.");
        break;
    }
}

MethodTreeMgr* GameFrameworkCtrNw4c::createMethodTreeMgr_(Heap* heap)
{
    return new(heap) DualScreenMethodTreeMgr();
}

GLvoid* GameFrameworkCtrNw4c::allocate(GLenum area, GLenum alignment, GLuint size, GLsizei heap)
{
    sMemoryMgr->allocate(area, alignment, size, reinterpret_cast<Heap*>(heap));
}

void GameFrameworkCtrNw4c::deallocate(GLenum area, GLenum alignment, GLuint size, GLvoid* ptr)
{
    sMemoryMgr->deallocate(area, alignment, size, ptr);
}

void GameFrameworkCtrNw4c::procFrame_()
{
    ProcessMeter* proc = ProcessMeter::instance();
    if (proc)
        proc->measureBeginFrame();

    Graphics::instance()->lockDrawContext();
    {
        nngxRunCmdlist();
        procDraw_();
        procCalc_();
        procReset_();
        swapBuffer_();
    }
    Graphics::instance()->unlockDrawContext();

    if (proc)
        proc->measureEndFrame();

    mLastUpdateTime = mFrameNow.diffToNow();
    mFrameNow.setNow();
    waitForVBlank_();

    if(getDisplayState() == cReady)
    {
        nngxStartLcdDisplay();

        mDisplayState = cShow;
    }
}

void GameFrameworkCtrNw4c::procDraw_()
{
    mDrawMeter.measureBegin();
    {
        DualScreenMethodTreeMgr* method = DynamicCast<DualScreenMethodTreeMgr>(getMethodTreeMgr());
        clearFrameBuffers_(6);
        mTopFrameBuffer->bind();
        method->drawTop();
        presentTop_();

        clearFrameBuffers_(9);
        mBtmFrameBuffer->bind();
        method->drawBtm();
        presentBtm_();

        mTopFrameBuffer->bind();
    }
    mDrawMeter.measureEnd();
}

void GameFrameworkCtrNw4c::procCalc_()
{
    mCalcMeter.measureBegin();
    {
        mTaskMgr->beforeCalc();
        DynamicCast<DualScreenMethodTreeMgr>(getMethodTreeMgr())->calc();
        mTaskMgr->afterCalc();
    }
    mCalcMeter.measureEnd();
}

void GameFrameworkCtrNw4c::presentTop_()
{
    nngxActiveDisplay(NN_GX_DISPLAY0);
    requestTransferRenderImage_(mFrameBufferNo[mBufferSizeTop[0]], &mBuffer, 0, 0, 0, NN_GX_ANTIALIASE_NOT_USED);
    nngxActiveDisplay(NN_GX_DISPLAY0);
    nngxBindDisplaybuffer(mFrameBufferNo[mBufferSizeTop[0]]);
}

void GameFrameworkCtrNw4c::presentBtm_()
{
    nngxActiveDisplay(NN_GX_DISPLAY1);
    requestTransferRenderImage_(mFrameBufferNo[mBufferSizeBtm[0]], &mBuffer, 0, 0, 0, NN_GX_ANTIALIASE_NOT_USED);
    nngxActiveDisplay(NN_GX_DISPLAY1);
    nngxBindDisplaybuffer(mFrameBufferNo[mBufferSizeBtm[0]]);
}

void GameFrameworkCtrNw4c::swapBuffer_()
{
    nngxRunCmdlist();
    mGPUMeter.measureBegin();
    GLint listParam;
    nngxGetCmdlistParameteri(NN_GX_CMDLIST_IS_RUNNING, &listParam);
    if(listParam)
        nngxWaitCmdlistDone();
    mGPUMeter.measureEnd();

    nngxStopCmdlist();
    nngxClearCmdlist();
    mFrameBufferNo[0] = -mFrameBufferNo[0];
#ifdef SEAD_DEBUG
    if(mExceptionScreen)
    {
        mExceptionScreen->getDirectPrint()->changeDisplaybuffer();
    }
    if(mScreenshotBuf)
    {
        doScreenShot_(mScreenshotBuf);
        mScreenshotBuf = nullptr;
    }
#endif
}

void GameFrameworkCtrNw4c::waitForVBlank_()
{
    Graphics::instance()->waitForVBlank();

    TickSpan waitSpan;
    waitSpan.setMilliSeconds(10);

    for(;;)
    {
        const u32 vblank = nngxCheckVSync(NN_GX_DISPLAY_BOTH);

        const u32 lastVblank = mGameArg.wait_vblank;

        u32 vblankDiff;
        if (lastVblank > vblank)
            vblankDiff = vblank + (0x7FFFFFFF * 2 + 1) - lastVblank;
        else
            vblankDiff = vblank - lastVblank;

        if (vblankDiff >= mVblinkBuf)
        {
            Graphics::instance()->lockDrawContext();
            {
                nngxSwapBuffers(NN_GX_DISPLAY_BOTH);
            }
            Graphics::instance()->unlockDrawContext();

            mGameArg.wait_vblank = vblank;
            

            break;
        }

        if (vblankDiff + 1 == mVblinkBuf)
        {
            Graphics::instance()->lockDrawContext();
            {
                nngxSwapBuffers(NN_GX_DISPLAY_BOTH);
            }
            Graphics::instance()->unlockDrawContext();

            Graphics::instance()->waitForVBlank();
            break;
        }
        else
        {
            Thread::sleep(waitSpan);
        }
    }

    mFrameNow.setNow();
}

void GameFrameworkCtrNw4c::clearFrameBuffers_(s32 method)
{
    getMethodFrameBuffer(method)->bind();
    Graphics::instance()->clear(FrameBuffer::cAll, mGameArg.clearColor, 1.0f, 0);
}

void GameFrameworkCtrNw4c::doScreenShot_(char const* shot)
{
    bool isEnable = false;
    if(InfLoopChecker::instance())
    {
        isEnable = InfLoopChecker::instance()->isEnable();
        InfLoopChecker::instance()->setEnable(false);
    }
    doScreenShotImpl_(shot);
    if(InfLoopChecker::instance())
    {
        InfLoopChecker::instance()->setEnable(isEnable);
    }
}

void GameFrameworkCtrNw4c::requestTransferRenderImage_(u32 displayBuffer, nn::gr::CTR::FrameBuffer* frameBuffer,
    s32 x, s32 y, f32 scaleX, f32 scaleY)
{
    nngxBindDisplaybuffer(displayBuffer);

    s32 addr;
    s32 form;
    s32 nnWidth;
    s32 nnHeight;

    nngxGetDisplaybufferParameteri(NN_GX_DISPLAYBUFFER_ADDRESS, &addr);
    nngxGetDisplaybufferParameteri(NN_GX_DISPLAYBUFFER_FORMAT, &form);
    nngxGetDisplaybufferParameteri(NN_GX_DISPLAYBUFFER_WIDTH, &nnWidth);
    nngxGetDisplaybufferParameteri(NN_GX_DISPLAYBUFFER_HEIGHT, &nnHeight);

    PicaDataColor colorFormat = frameBuffer->colorBuffer.format;

    u32 colorSize = getPicaDataColorSize_(colorFormat);

    const u32 transferAddress = frameBuffer->colorBuffer.virtualAddr + frameBuffer->colorBuffer.width * 
    (frameBuffer->colorBuffer.height - nnWidth - y) * colorSize + x;

    nngxFlush3DCommand();

    nn::gr::CTR::FrameBuffer::BlockSize blockSize = frameBuffer->colorBuffer.blockSize;

    u32 blockSizeBytes = blockSize == nn::gr::CTR::FrameBuffer::BLOCK_SIZE8 ? 8 : 32;

    u32 glFormat = picaDataColor2GLenum_(colorFormat);

    nngxAddB2LTransferCommand(
        reinterpret_cast<const GLvoid*>(transferAddress),
        frameBuffer->colorBuffer.width,
        frameBuffer->colorBuffer.height,
        glFormat,
        reinterpret_cast<GLvoid*>(addr),
        form,
        nnWidth,
        nnHeight,
        blockSize,
        scaleX,
        scaleY
    );
}

void GameFrameworkCtrNw4c::doScreenShotImpl_(char const* shot)
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
        nngxBindDisplaybuffer(mFrameBufferNo[mBufferSizeTop[0]]);
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
        nngxBindDisplaybuffer(mFrameBufferNo[mBufferSizeBtm[0]]);
        GLint param;
        nngxGetDisplaybufferParameteri(NN_GX_DISPLAYBUFFER_ADDRESS, &param);
        saveScreenShotToFileHandle_(&btmHandle, &param, mGameArg.widthBtm, mGameArg.heightBtm, mGameArg.format);
    }
}

FrameBuffer* GameFrameworkCtrNw4c::getMethodFrameBuffer(s32 methodType) const
{
    switch (methodType)
    {
    case 2:
    {
        DualScreenMethodTreeMgr* mgr = DynamicCast<DualScreenMethodTreeMgr>(static_cast<MethodTreeMgr*>(getMethodTreeMgr()));

        if (mgr->getSysDrawScreen() == false)
        {
            return mTopFrameBuffer;
        }
        return mBtmFrameBuffer;
    }
    case 3:
    {
        return mTopFrameBuffer;
    }

    case 4:
    {
        DualScreenMethodTreeMgr* mgr = DynamicCast<DualScreenMethodTreeMgr>(static_cast<MethodTreeMgr*>(getMethodTreeMgr()));

        if (mgr->getAppDrawScreen() == false)
        {
            return mTopFrameBuffer;
        }
        return mBtmFrameBuffer;
    }

    case 5:
    case 6:
    case 7:
    {
        return mBtmFrameBuffer;
    }
    default:
    {
        SEAD_ASSERT_MSG(false, "Undefined MethodType(%d).", methodType);
    }

    }
}
}