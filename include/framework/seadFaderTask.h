#pragma once

#include <framework/seadMethodTree.h>
#include <framework/seadTaskBase.h>

namespace sead 
{
class FaderTaskBase : public TaskBase
{
public:
    enum Type
    {
        cCreate = 0,
        cTakeover,
        cTransit,
        cPush,
        cPop,
        cInvalid
    };

    enum FaderState
    {
        cWait = 0,
        cFadeout,
        cLoop,
        cFadein
    };

    enum FadeEvent
    {
        cFadeinStarted = 0,
        cFadeinEnded,
        cFadeoutStarted,
        cFadeoutEnded,
        cMax
    };

public:
    FaderTaskBase(const TaskConstructArg& arg, const char* name);

    void setFrames(s32 fadeout, s32 black, s32 fadein);

    virtual void pauseCalc(bool b);
    virtual void pauseCalcRec(bool b);
    virtual void pauseCalcChild(bool b);

    virtual void enter();
    virtual void attachCalcImpl();
    virtual void detachCalcImpl();
    virtual MethodTreeNode* getMethodTreeNode(s32 methodType);

protected:
    virtual void doCalc_();
    virtual void onFadeEvent_(const FadeEvent& event);

    void fadein_();
    bool startCreate_();
    void onCreateDone_(TaskBase* task);

public:
    virtual void calc();

protected:
    void calcCore_();

    void setFaderState_(FaderState s);

    bool startAsCreate_(const CreateArg& createArg);
    bool startAsTakeover_(TaskBase* destroyTask, const CreateArg& createArg);
    bool startAsTransit_(TaskBase* from, TaskBase* to);
    bool startAsPush_(TaskBase* sleepTask, const CreateArg& createArg);
    bool startAsPop_(TaskBase* sleepTask, TaskBase* toTask);

    friend class TaskMgr;

protected:
    Type mType;
    TaskBase* mSrcTask;
    CreateArg mCreateArg;
    TaskBase* mDstTask;
    DelegateEvent<TaskBase*>::Slot mCreateDoneSlot;
    MethodTreeNode mCalcNode;
    MethodTreeNode mCalcDestructionNode;
    s32 mFrame;
    s32 mFadeoutFrame;
    s32 mLoopFrame;
    s32 mFadeinFrame;
    f32 mBlackness;
    bool mFinishing;
    FaderState mFaderState;
};

class FaderTask : public FaderTaskBase
{
public:
    FaderTask(const TaskConstructArg& arg, const char* name);
    virtual ~FaderTask();
    virtual void pauseDraw(bool b);
    virtual void pauseDrawRec(bool b);
    virtual void pauseDrawChild(bool b);

    virtual void attachDrawImpl();
    virtual void detachDrawImpl();

    virtual const RuntimeTypeInfo::Interface* getCorrespondingMethodTreeMgrTypeInfo() const;
    virtual MethodTreeNode* getMethodTreeNode(s32 methodType);

    virtual void draw();

protected:
    MethodTreeNode mDrawNode;
};

class NullFaderTask : public FaderTaskBase
{
public:
    explicit NullFaderTask(const TaskConstructArg& arg);

    virtual void draw();

    virtual void pauseDraw(bool b);
    virtual void pauseDrawRec(bool b);
    virtual void pauseDrawChild(bool b);

    virtual void attachDrawImpl();
    virtual void detachDrawImpl();

    virtual const RuntimeTypeInfo::Interface* getCorrespondingMethodTreeMgrTypeInfo() const;
    virtual MethodTreeNode* getMethodTreeNode(s32 methodType);
};

} // namespace sead
