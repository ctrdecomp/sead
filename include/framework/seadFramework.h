#ifndef SEAD_FRAMEWORK_H_
#define SEAD_FRAMEWORK_H_

#include <framework/seadMethodTreeMgr.h>
#include <framework/seadTaskBase.h>
#include <framework/seadTaskMgr.h>
#include <gfx/seadFrameBuffer.h>
#include <heap/seadArena.h>
#include <heap/seadHeap.h>
#include <hostio/seadHostIOMgr.h>
#include <prim/seadDelegateEventSlot.h>
#include <prim/seadRuntimeTypeInfo.h>
#include <time/seadTickSpan.h>

namespace sead
{
class Arena;

class FrameBuffer;
class Heap;
class LogicalFrameBuffer;
class MethodTreeMgr;
class TaskMgr;

class Framework : public hostio::Node
{
    SEAD_RTTI_BASE(Framework)

public:
    struct CreateSystemTaskArg
    {
        CreateSystemTaskArg();

        HostIOMgr::Parameter* hostio_parameter;
        Heap* heap;
        TickSpan infloop_detection_span;
        int infloop_thread_stack_size;
    };

    struct InitializeArg
    {
        InitializeArg();

        u64 heap_size;
        Arena* arena;
    };

    struct RunArg
    {
        RunArg();

        u32 prepare_stack_size;
        s32 prepare_priority;
    };

    enum ProcessPriority
    {
        cProcessPriority_Idle = 0,
        cProcessPriority_Normal = 1,
        cProcessPriority_High = 2,
        cProcessPriority_RealTime = 3
    };

public:
    typedef DelegateEvent<void*> ResetEvent;
    
    static void initialize(const InitializeArg&);

    Framework();
    virtual ~Framework();

    virtual void run(Heap*, const TaskBase::CreateArg&, const RunArg&);
    virtual void createSystemTasks(TaskBase*, const CreateSystemTaskArg&);
    virtual FrameBuffer* getMethodFrameBuffer(s32) const = 0;
    virtual const LogicalFrameBuffer* getMethodLogicalFrameBuffer(s32 methodType) const{ { return getMethodFrameBuffer(methodType); } }

    virtual bool setProcessPriority(ProcessPriority priority)
    {
        SEAD_UNUSED(priority);
        return false;
    }

    virtual void reserveReset(void* param)
    {
        mReserveReset = true;
        mResetParameter = param;
    }

    TaskMgr* getTaskMgr()
    {
        return mTaskMgr;
    }

    MethodTreeMgr* getMethodTreeMgr() const
    {
        return mMethodTreeMgr;
    }

    void registerResetEvent(ResetEvent::Slot& slot)
    {
        mResetEvent.connect(slot);
    }

    void unregisterResetEvent(ResetEvent::Slot& slot)
    {
        mResetEvent.disconnect(slot);
    }

protected:
    virtual void initRun_(Heap*){ }
    virtual void runImpl_(){ }
    virtual MethodTreeMgr* createMethodTreeMgr_(Heap*) = 0;
    virtual void procReset_();

    bool mReserveReset;
    void* mResetParameter;
    ResetEvent mResetEvent;
    TaskMgr* mTaskMgr;
    MethodTreeMgr* mMethodTreeMgr;
    Heap* mMethodTreeMgrHeap;
};

}  // namespace sead

#endif  // SEAD_FRAMEWORK_H_
