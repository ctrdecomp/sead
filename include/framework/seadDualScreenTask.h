#pragma once

#include "framework/seadTaskBase.h"
#include "framework/seadMethodTree.h"

namespace sead
{
struct TaskConstructArg;
class DualScreenTask : public TaskBase
{
    SEAD_RTTI_OVERRIDE(DualScreenTask, TaskBase)
public:
    explicit DualScreenTask(const struct TaskConstructArg& arg);
    DualScreenTask(const struct TaskConstructArg& arg, const char* name);

    virtual ~DualScreenTask();
    virtual void pauseCalc(bool b);
    virtual void pauseDraw(bool b);
    virtual void pauseCalcRec(bool b);
    virtual void pauseDrawRec(bool b);

    virtual void attachCalcImpl();
    virtual void attachDrawImpl();
    virtual void detachCalcImpl();
    virtual void detachDrawImpl();
    virtual const RuntimeTypeInfo::Interface* getCorrespondingMethodTreeMgrTypeInfo() const;
    virtual MethodTreeNode* getMethodTreeNode(s32 methodType);
    virtual void calc(){ }
    virtual void drawTop(){ }
    virtual void drawBtm(){ }

    MethodTreeNode mCalcNode;
    MethodTreeNode mTopNode;
    MethodTreeNode mBtmNode;
};
}
