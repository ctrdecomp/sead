#ifndef SEAD_TASKBASE_H_
#define SEAD_TASKBASE_H_

#include <container/seadTList.h>
#include <container/seadTreeNode.h>
#include <framework/seadHeapPolicies.h>
#include <framework/seadTaskID.h>
#include <framework/seadMethodTreeMgr.h>
#include <heap/seadDisposer.h>
#include <prim/seadBitFlag.h>
#include <prim/seadDelegateEventSlot.h>
#include <prim/seadNamable.h>
#include <prim/seadRuntimeTypeInfo.h>
#include <thread/seadDelegateThread.h>

namespace sead
{
class Framework;
class FaderTaskBase;
class MethodTreeNode;
class MethodTreeMgr;
class TaskEvent;
class TaskMgr;
class TaskParameter;

class TaskBase : public TTreeNode<TaskBase*>, public IDisposer, public INamable
{
    SEAD_RTTI_BASE(TaskBase)

public:
    typedef TListNode<TaskBase*> ListNode;
    typedef TList<TaskBase*> List;

public:
    enum State
    {
        cCreated = 0,
        cPrepare = 1,
        cPrepareDone = 2,
        cSleep = 3,
        cRunning = 4,
        cDying = 5,
        cDestroyable = 6,
        cDead = 7
    };

    enum Tag
    {
        cSystem = 0,
        cApp = 1
    };

    struct CreateArg
    {
        CreateArg();
        CreateArg(const TaskClassID& factory);

        typedef void (*SingletonFunc)(TaskBase*);

        TaskClassID factory;
        HeapPolicies heap_policies;
        TaskBase* parent;
        TaskParameter* parameter;
        FaderTaskBase* fader;
        TaskBase* src_task;
        TaskBase** created_task;
        DelegateEvent<TaskBase*>::Slot* create_callback;
        TaskUserID user_id;
        Tag tag;
        SingletonFunc instance_cb;
    };

    struct TakeoverArg : public CreateArg
    {
        TakeoverArg(TaskBase* src, const TaskClassID& dst, FaderTaskBase* fader);
        TakeoverArg(const TaskClassID& dst, FaderTaskBase* fader);
    };

    struct PushArg : public CreateArg
    {
        PushArg(TaskBase* src, const TaskClassID& dst, FaderTaskBase* fader);
        PushArg(const TaskClassID& dst, FaderTaskBase* fader);
    };

    struct MgrTaskArg : public CreateArg
    {
        explicit MgrTaskArg(const TaskClassID& classID);
    };

    struct SystemMgrTaskArg : public MgrTaskArg
    {
        explicit SystemMgrTaskArg(const TaskClassID& classID);
    };

public:
    TaskBase()
    {
    }
    
    explicit TaskBase(const TaskConstructArg& arg);
    TaskBase(const TaskConstructArg& arg, const char* name);
    virtual ~TaskBase();
    virtual void pauseCalc(bool b) = 0;
    virtual void pauseDraw(bool b) = 0;
    virtual void pauseCalcRec(bool b) = 0;
    virtual void pauseDrawRec(bool b) = 0;
    virtual void pauseCalcChild(bool b){ }
    virtual void pauseDrawChild(bool b){ }

#if defined(SEAD_DEBUG)
    virtual void taskListenPropertyEvent(const hostio::PropertyEvent* event);
    virtual void taskGenMessage(hostio::Context* ctx);
    virtual void taskDoGenMessage(hostio::Context* ctx);
    virtual void taskDoListenPropertyEvent(const hostio::PropertyEvent* event);
#endif // SEAD_DEBUG
    virtual void prepare();
    virtual void enterCommon();
    virtual void enter();
    virtual void exit();
    virtual void onEvent(const TaskEvent&);
    virtual void attachCalcImpl() = 0;
    virtual void attachDrawImpl() = 0;
    virtual void detachCalcImpl() = 0;
    virtual void detachDrawImpl() = 0;
    virtual const RuntimeTypeInfo::Interface* getCorrespondingMethodTreeMgrTypeInfo() const = 0;
    virtual MethodTreeNode* getMethodTreeNode(s32 method_type) = 0;
    virtual void onDestroy();

    void attachCalc();
    void attachCalcDraw();
    void attachDraw();

    void doneDestroy();

    bool isConnectable(TaskBase* child) const;
    bool isDescendantOf(TaskBase* ancestor) const;

    void attachMethodWithCheck(s32 methodType, MethodTreeNode* node);

    bool requestCreateTask(const CreateArg& arg);
    TaskBase* createTaskSync(const CreateArg& arg);
    TaskBase* createChildTaskSync(CreateArg& arg);

    bool requestTakeover(const TakeoverArg& arg);
    bool requestTransition(TaskBase* next, FaderTaskBase* fader);


    bool requestPush(const PushArg& arg);
    bool requestPop();
    TaskBase* pushSync(const PushArg& arg);

    void adjustHeap(s32 index);
    void adjustHeapAll();
    void adjustHeapWithSlack(s32 index, size_t size);
    void adjustHeapWithSlackWithoutLock_(s32 index, size_t size);

    TaskBase* getParentTask() const
    {
        return parent() ? parent()->val() : nullptr;
    }

    const HeapArray& getHeapArray() const
    {
        return mHeapArray;
    }

    TaskMgr* getTaskMgr() const
    {
        return mTaskMgr;
    }

    template <typename T>
    T* getParameter() const
    {
        T* param = DynamicCast<T>(mParameter);
        return param;
    }

    Tag getTag() const
    {
        return mTag;
    }

    void detachCalc()
    { 
        detachCalcImpl(); 
    }

    void detachDraw()
    { 
        detachDrawImpl(); 
    }

    void detachCalcDraw()
    {
        detachCalc();
        detachDraw();
    }

    Framework* getFramework() const;
    MethodTreeMgr* getMethodTreeMgr() const;

public:
    void setFlag_(u32 f)
    {
        mInternalFlag.set(f);
    }

    void resetFlag_(u32 f)
    {
        mInternalFlag.reset(f);
    }

    void clearFlag_()
    {
        mInternalFlag.makeAllZero();
    }

    bool checkFlag_(u32 f) const
    {
        return mInternalFlag.isOn(f);
    }

    TaskParameter* mParameter;
    BitFlag32 mInternalFlag;
    ListNode mTaskListNode;
    HeapArray mHeapArray;
    TaskMgr* mTaskMgr;
    State mState;
    Tag mTag;
    TaskClassID mClassID;
};

}  // namespace sead

#endif  // SEAD_TASKBASE_H_
