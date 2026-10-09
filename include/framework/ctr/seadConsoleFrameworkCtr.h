#pragma once

// This is a copy of the windows port ctrified

#include "framework/seadFramework.h"

namespace sead
{
class ConsoleFrameworkCtr : public Framework
{
    SEAD_RTTI_OVERRIDE(ConsoleFrameworkCtr, Framework)
public:
    static void initialize(const InitializeArg& arg);

    ConsoleFrameworkCtr();
    virtual ~ConsoleFrameworkCtr();
    virtual FrameBuffer* getMethodFrameBuffer(s32) const{ return NULL; }
    virtual void runImpl();
protected:
    virtual MethodTreeMgr* createMethodTreeMgr_(Heap* heap);
};
}
