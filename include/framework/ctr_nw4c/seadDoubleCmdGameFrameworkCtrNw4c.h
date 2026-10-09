#pragma once

#include "framework/ctr_nw4c/seadGameFrameworkCtrNw4c.h"

namespace sead
{
class DoubleCmdGameFrameworkCtrNw4c : public GameFrameworkCtrNw4c
{
    SEAD_RTTI_OVERRIDE(DoubleCmdGameFrameworkCtrNw4c, GameFrameworkCtrNw4c)

public:
    static void initialize(const Framework::InitializeArg& arg);
    void initializeGraphicsSystem(Heap* heap, const Vector2f& topFbSize, const Vector2f& btmFbSize);

    explicit DoubleCmdGameFrameworkCtrNw4c(const CreateArg& arg);

    virtual ~DoubleCmdGameFrameworkCtrNw4c();

    static void cmdlistCallback(GLint list);
protected:
    virtual void mainLoop_();
    virtual void procFrame_();
    virtual void presentTop_();
    virtual void presentBtm_();
    virtual void swapBuffer_();
    virtual void waitForVBlank_();
    virtual void doScreenShotImpl_(const char* shot);
    virtual void cmdlistCallbackImpl_(s32 list);

    static DoubleCmdGameFrameworkCtrNw4c* sInstance;
protected:
    s32 _2068;
    u32 mDoubleBuf[2];
    GLuint mDoubleBufferTop[3];
    GLuint mDoubleBufferBtm[3];
    u32 mDoubleDispBufList;
    u32 mDoubleDispBufState;
    u32 mDoubleDispBufFrameBuffer;
    TickTime mLastDoubleTick;
    u8 mProcessMeterBar;
    bool mWaitCmdlistDone;
    bool mLastCmdlistDone;
    bool mWaitForVBlink;
    GLint mDoubleCmdParam[3];
};

}
