#pragma once

#include "framework/seadDualScreenTask.h"

namespace sead
{
class UlcdTask : public DualScreenTask
{
    SEAD_RTTI_OVERRIDE(UlcdTask, DualScreenTask);
public:
    explicit UlcdTask(const struct TaskConstructArg& arg);
    UlcdTask(const struct TaskConstructArg& arg, const char* name);

    virtual ~UlcdTask();
    virtual void pauseDraw(bool b);
    virtual void pauseDrawRec(bool b);

    virtual void attachDrawImpl();
    virtual void detachDrawImpl();
    virtual const RuntimeTypeInfo::Interface* getCorrespondingMethodTreeMgrTypeInfo() const;
    virtual MethodTreeNode* getMethodTreeNode(s32 methodType);

    virtual void drawTop(){ drawLeft(); }
    virtual void drawLeft(){ }
    virtual void drawRight(){ }

    MethodTreeNode mRightNode;
};
}
