#pragma once

#include "framework/seadGameFramework.h"
#include "gfx/ctr/seadGfxMemoryMgrCtr.h"
#include "gfx/ctr/seadFrameBufferCtr.h"

#include <nn/gr.h>
#include <nn/gx.h>

namespace sead
{
class FileHandle;

class GameFrameworkCtrNw4c : public GameFramework
{
    SEAD_RTTI_OVERRIDE(GameFrameworkCtrNw4c, GameFramework)
public:
    struct ScreenShotBuffer
    {
        s32 mScreenShotNo;
        const char* mScreenshotPath;
    };
    struct CreateArg
    {
        ~CreateArg()
        {
        }
        
        s32 widthTop;
        s32 heightTop;
        s32 widthBtm;
        s32 heightBtm;
        f32 physH_Top;
        f32 physW_Top;
        f32 physH_Btm;
        f32 physW_Btm;
        u32 wait_vblank;
        Color4f clearColor;
        u32 cmdBufSize;
        u32 cmdRequestCount;
        DefaultGfxMemoryMgrCtr* memoryMgrCtr;
        u32 cmdMemSize;
        GLenum format;
        void* screenshot_buffer;
        s32 vsync_buf;
    };

public:
    static const int cMaxScreenShotPathLength = 256;

    static void initialize(const Framework::InitializeArg& arg);
    void initializeGraphicsSystem(Heap* heap, const Vector2f& topFbSize, const Vector2f& btmFbSize);

    explicit GameFrameworkCtrNw4c(const CreateArg& arg);

    virtual ~GameFrameworkCtrNw4c()
    {
    }
    virtual FrameBuffer* getMethodFrameBuffer(s32 methodType) const;
    virtual void initRun_(Heap*);
    virtual void runImpl_();
    virtual MethodTreeMgr* createMethodTreeMgr_(Heap*);
    virtual float calcFps();
    virtual void saveScreenShot(const SafeString& filename);
    virtual bool isScreenShotBusy() const{ return mScreenshotBuf != nullptr; }
    virtual void setCaption(SafeString const& caption);

    static GLvoid* allocate(GLenum area, GLenum alignment, GLuint size, GLsizei heap);
    static void deallocate(GLenum area, GLenum alignment, GLuint size, GLvoid* ptr);

    static GfxMemoryMgrCtr* sMemoryMgr;
protected:
    virtual void mainLoop_();
    virtual void procFrame_();
    virtual void procDraw_();
    virtual void procCalc_();
    virtual void presentTop_();
    virtual void presentBtm_();
    virtual void swapBuffer_();
    virtual void waitForVBlank_();
    virtual void clearFrameBuffers_(s32 buffer);
    virtual void doScreenShot_(const char* shot);
    virtual void doScreenShotImpl_(const char* shot);

    GLsizei createCmdlist_(GLsizei bufsize, GLsizei requestcount);
    u32 createDisplayBuffers_(u32* dst, u32 num, u32 disp, GLenum format, s32 width, s32 height, u32 buffer);
    void createFramebuffer_(nn::gr::CTR::FrameBuffer* buffer, s32 width, s32 height, u32 vram_a, PicaDataColor color_area, u32 vram_b, PicaDataDepth depth_area);
    void initNngx_(GfxMemoryMgrCtr* memMgr);
    void saveScreenShotToFileHandle_(FileHandle* handle, void*, int width, int height, u32);
    void requestTransferRenderImage_(u32 displayBuffer, nn::gr::CTR::FrameBuffer* frameBuffer, s32 x, s32 y, f32 scaleX, f32 scaleY);
protected:
// this struct is BIG, so i label offsets
    CreateArg mGameArg;
    u32 mVblinkBuf;
    TickSpan mLastUpdateTime;
    TickTime mFrameNow;
    TickTime mLastDiffTime;
    GLuint mBufferSizeTop[2];
    GLuint mBufferSizeBtm[2];
    nn::gr::CTR::FrameBuffer mBuffer;
    FrameBufferCtr* mTopFrameBuffer;
    FrameBufferCtr* mBtmFrameBuffer;
    u32 mFrameBufferNo[1];
    const char* mScreenshotBuf;
    void* mCurrentScreenshot;
#ifdef SEAD_DRAW_EXCEPTION
    ExceptionScreenCtr* mExceptionScreen;
#endif
    GLint* mGLDispParam;
};

inline float GameFrameworkCtrNw4c::calcFps()
{
    TickSpan sec_span = TickSpan::makeFromSeconds(1);
    return static_cast<f32>(sec_span.toS64()) / static_cast<f32>(mLastUpdateTime.toS64());
}

inline void GameFrameworkCtrNw4c::saveScreenShot(const SafeString& filename)
{
    SEAD_ASSERT(filename.calcLength() <= cMaxScreenShotPathLength);
    mScreenshotBuf = filename.cstr();
}

inline void GameFrameworkCtrNw4c::setCaption(SafeString const& caption)
{
    SEAD_PRINT("%s\n", caption.cstr());
}
}
