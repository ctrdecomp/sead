#pragma once

#include "framework/ctr_nw4c/seadGameFrameworkCtrNw4c.h"

namespace sead
{
class UlcdGameFrameworkCtrNw4c : public GameFrameworkCtrNw4c
{
    SEAD_RTTI_OVERRIDE(UlcdGameFrameworkCtrNw4c, GameFrameworkCtrNw4c)

public:
    static void initialize(const Framework::InitializeArg& arg);
    void initializeGraphicsSystem(Heap* heap, const Vector2f& leftFbSize, const Vector2f& rightFbSize);

    explicit UlcdGameFrameworkCtrNw4c(const CreateArg& arg);

    void setUlcdEnable(bool enable);

    virtual ~UlcdGameFrameworkCtrNw4c();
    virtual FrameBuffer* getMethodFrameBuffer(s32 methodType) const;
    virtual MethodTreeMgr* createMethodTreeMgr_(Heap*);
protected:
    virtual void procDraw_();
    virtual void doScreenShotImpl_(const char* shot);
    virtual void presentLeft_();
    virtual void presentRight_();

    GLuint mDisplayBufferRight[3];
};
}
