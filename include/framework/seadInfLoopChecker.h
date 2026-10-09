#pragma once

#include "basis/seadTypes.h"
#include "framework/seadCalculateTask.h"
#include "framework/seadTaskMgr.h"
#include "thread/seadAtomic.h"

namespace sead
{
class InfLoopChecker : public CalculateTask
{
    SEAD_RTTI_OVERRIDE(InfLoopChecker, CalculateTask);
    SEAD_TASK_SINGLETON_DISPOSER(InfLoopChecker);

public:
    struct InfLoopParam
    {
    };

    typedef DelegateEvent<const InfLoopParam&> InfLoopEvent;

    explicit InfLoopChecker(const TaskConstructArg& arg);
    virtual ~InfLoopChecker();
    virtual void prepare();
    virtual void calc();

    void countUp();
    
    u32 getCounter() const
    {
        return mLoopCounter;
    }

    void resetCounter()
    {
        mLoopCounter = 0;
    }

    u32 getThreshold() const
    {
        return mLoopThreshold;
    }

    void setThreshold(u32 threashold)
    {
        SEAD_ASSERT(threashold > 1);
        mLoopThreshold = threashold;
    }

    void setDisableCounter(u32 counter)
    {
        mDisableCounter.setValue(counter);
    }

    void setEnable(bool enable)
    {
        mEnable = enable;
    }

    bool isEnable() const
    {
        return mEnable;
    }

    //? Unofficial name but probably exists
    void registerCallback(InfLoopEvent::Slot& slot)
    {
        mInfLoopEvent.connect(slot);
    }

    //? Unofficial name but probably exists
    void unregisterCallback(InfLoopEvent::Slot& slot)
    {
        mInfLoopEvent.disconnect(slot);
    }
protected:
    void onInfLoop_();

private:
    u32 mLoopCounter;
    u32 mLoopThreshold;
    bool mEnable;
    InfLoopEvent mInfLoopEvent;
    sead::Atomic<u32> mDisableCounter;
};
}  // namespace sead

