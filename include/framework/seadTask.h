#pragma once

#include <framework/seadMethodTree.h>
#include <framework/seadTaskBase.h>
#include <prim/seadRuntimeTypeInfo.h>

namespace sead
{
class Task : public TaskBase
{
    SEAD_RTTI_OVERRIDE(Task, TaskBase);

public:
    Task()
    {
    }
    
    explicit Task(const TaskConstructArg& arg);
    Task(const TaskConstructArg& arg, const char* name);
    virtual ~Task();
    virtual void pauseCalc(bool b);
    virtual void pauseDraw(bool b);
    virtual void pauseCalcRec(bool b);
    virtual void pauseDrawRec(bool b);
    virtual void pauseCalcChild(bool b);
    virtual void pauseDrawChild(bool b);
    virtual void attachCalcImpl();
    virtual void attachDrawImpl();
    virtual void detachCalcImpl();
    virtual void detachDrawImpl();
    virtual const RuntimeTypeInfo::Interface* getCorrespondingMethodTreeMgrTypeInfo() const;
    virtual MethodTreeNode* getMethodTreeNode(s32 method_type);
    virtual void calc();
    virtual void draw();

protected:
    MethodTreeNode mCalcNode;
    MethodTreeNode mDrawNode;
};
}  // namespace sead

