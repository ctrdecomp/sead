#pragma once

#include "framework/ctr_nw4c/seadDoubleCmdGameFrameworkCtrNw4c.h"

namespace sead
{
class UlcdDoubleCmdGameFrameworkCtrNw4c : public DoubleCmdGameFrameworkCtrNw4c
{
    SEAD_RTTI_OVERRIDE(UlcdDoubleCmdGameFrameworkCtrNw4c, DoubleCmdGameFrameworkCtrNw4c)

public:
    static void initialize(const Framework::InitializeArg& arg);
    void initializeGraphicsSystem(Heap* heap, const Vector2f& leftFbSize, const Vector2f& rightFbSize);

    explicit UlcdDoubleCmdGameFrameworkCtrNw4c(const CreateArg& arg);

    void setUlcdEnable(bool enable);

    virtual ~UlcdDoubleCmdGameFrameworkCtrNw4c();
    virtual FrameBuffer* getMethodFrameBuffer(s32 methodType) const;
    virtual MethodTreeMgr* createMethodTreeMgr_(Heap*);
protected:
    virtual void procDraw_();
    virtual void swapBuffer_();
    virtual void doScreenShotImpl_(const char* shot);
    virtual void presentLeft_();
    virtual void presentRight_();

    GLuint mDisplayBufferRight[3];
};
}
