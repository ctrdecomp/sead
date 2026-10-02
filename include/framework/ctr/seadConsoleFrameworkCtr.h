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
protected:
    virtual void runImpl_();
    virtual MethodTreeMgr* createMethodTreeMgr_(Heap* heap);
};
}