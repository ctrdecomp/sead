#pragma once

#include <basis/seadAssert.h>
#include <container/seadObjList.h>
#include <framework/seadMethodTree.h>
#include <framework/seadTaskBase.h>
#include <hostio/seadHostIONode.h>
#include <prim/seadDelegateEventSlot.h>
#include <thread/seadCriticalSection.h>
#include <thread/seadMessageQueue.h>

namespace sead
{
class DelegateThread;
class Framework;
class Heap;
class NullFaderTask;

class TaskCreateContext
{
public:
    TaskCreateContext();

private:
    TaskBase* mCreatedTask;
    TaskBase::CreateArg mArg;
    DelegateEvent<TaskBase*> mEventOnCreate;

    friend class TaskMgr;
};

class TaskMgr : public sead::hostio::Node
{
public:
    struct InitializeArg
    {
    public:
        InitializeArg(const TaskBase::CreateArg& roottask_arg):
            create_queue_size(0x20),
            prepare_stack_size(0x8000),
            prepare_priority(-1),
            roottask_create_arg(roottask_arg),
            heap(nullptr),
            parent_framework(nullptr)
        {
        }

        u32 create_queue_size;
        u32 prepare_stack_size;
        s32 prepare_priority;
        const TaskBase::CreateArg& roottask_create_arg;
        Heap* heap;
        Framework* parent_framework;
    };

    class TaskCreateContextMgr : public ObjList<TaskCreateContext>
    {
    public:
        TaskCreateContextMgr(s32 size, Heap* heap);

        bool isCreateEmpty() const;
        TaskCreateContext* birthForCreate();
        TaskCreateContext* getCreateFront();
        TaskCreateContext* find(TaskBase* task);
        void kill(TaskCreateContext* cc);
    };

public:
    TaskMgr(const InitializeArg& arg);

    static TaskMgr* initialize(const InitializeArg& arg);
    void finalize();

    void initHostIO();

#if defined(SEAD_DEBUG)
    virtual void listenPropertyEvent(const hostio::PropertyEvent* ev);
    virtual void genMessage(hostio::Context* context);
#endif // SEAD_DEBUG

    TaskBase* createTaskSync(const TaskBase::CreateArg& arg);

    Framework* getFramework() const
    {
        return mParentFramework;
    }

    TaskBase* getRootTask() const
    {
        return mRootTask;
    }

    template <typename T>
    T* createSingletonTaskSync(const TaskBase::CreateArg& arg)
    {
        TaskBase::CreateArg arg_ = arg;
        arg_.instance_cb = &T::setInstance_;

        TaskBase* task = createTaskSync(arg_);

        T* derived = DynamicCast<T>(task);
        SEAD_ASSERT(derived != nullptr);

        return T::instance();
    }

    bool requestCreateTask(const TaskBase::CreateArg& arg);
    TaskBase* createTaskSync(const TaskBase::CreateArg& arg);
    void destroyTaskSync(TaskBase* task);
    void requestDestroyTask(TaskBase* task, FaderTaskBase* fader);

    bool requestTakeover(const TaskBase::TakeoverArg& arg);
    bool requestTransition(TaskBase* from, TaskBase* to, FaderTaskBase* fader);

    bool requestPush(const TaskBase::PushArg& arg);
    TaskBase* pushSync(const TaskBase::PushArg& arg);
    bool requestPop(TaskBase* popTask, FaderTaskBase* fader);
    bool requestPop(TaskBase* popTask, TaskBase* toTask, FaderTaskBase* fader);
    bool popSync(TaskBase* popTask);

    void beforeCalc();
    void afterCalc();

    TaskBase* findTask(const TaskClassID& classID);

    void destroyAllAndCreateRoot();
private:
    void doInit_();

    void appendToList_(TaskBase::List& ls, TaskBase* task);
    void createHeap_(HeapArray* ha, const TaskBase::CreateArg& arg);

    TaskBase* doCreateTask_(const TaskBase::CreateArg& arg, HeapArray* ha);
    bool doRequestCreateTask_(const TaskBase::CreateArg& arg, DelegateEvent<TaskBase*>::Slot* systemSlot);
    void doDestroyTask_(TaskBase* task);

    bool changeTaskState_(TaskBase* task, TaskBase::State state);

    void calcCreation_();
    void calcDestruction_();

    void prepare_(Thread* thread, MessageQueue::Element msg);

    void beginCreateRootTask_();
    bool destroyable_(TaskBase* task);

    CriticalSection mCriticalSection;
    Framework* mParentFramework;
    DelegateThread* mPrepareThread;
    NullFaderTask* mNullFaderTask;
    TaskBase::List mPrepareList;
    TaskBase::List mPrepareDoneList;
    TaskBase::List mActiveList;
    TaskBase::List mStaticList;
    TaskBase::List mDyingList;
    TaskBase::List mDestroyableList;
    HeapArray mHeapArray;
    TaskCreateContextMgr* mTaskCreateContextMgr;
    u32 mMaxCreateQueueSize;
    TaskBase* mRootTask;
    TaskBase::CreateArg mRootTaskCreateArg;
    TaskMgr::InitializeArg mInitializeArg;
    MethodTreeNode mCalcDestructionTreeNode;
    u32 useless1;
    u32 useless2;

    friend class TaskBase;
    friend class FaderTaskBase;
};

}  // namespace sead

#define SEAD_TASK_SINGLETON_DISPOSER(CLASS)                 \
protected:                                                  \
    class SingletonDisposer_                                \
    {                                                       \
    public:                                                 \
        SingletonDisposer_()                                \
            : mIsSetAsSingleton_(false)                     \
        {                                                   \
        }                                                   \
                                                            \
        ~SingletonDisposer_();                              \
                                                            \
        bool mIsSetAsSingleton_;                            \
    };                                                      \
                                                            \
public:                                                     \
    static CLASS* instance() { return sInstance; }          \
    static void setInstance_(sead::TaskBase* instance);     \
    static void deleteInstance();                           \
                                                            \
    SEAD_NO_COPY(CLASS);                                    \
                                                            \
protected:                                                  \
    SingletonDisposer_ mSingletonDisposer_;                 \
                                                            \
    static CLASS* sInstance;                                \
                                                            \
    friend class SingletonDisposer_

#define SEAD_TASK_SET_SINGLETON_INSTANCE(CLASS)                                                         \
    void CLASS::setInstance_(sead::TaskBase* instance)                                                  \
    {                                                                                                   \
        if (!sInstance)                                                                                 \
        {                                                                                               \
            sInstance = static_cast<CLASS*>(instance);                                                  \
            sInstance->mSingletonDisposer_.mIsSetAsSingleton_ = true;                                   \
        }                                                                                               \
        else                                                                                            \
        {                                                                                               \
            SEAD_ASSERT_MSG(false, "Create Singleton Twice (%s) : addr 0x%p", #CLASS, sInstance);       \
        }                                                                                               \
    }

#define SEAD_TASK_DELETE_SINGLETON_INSTANCE(CLASS)                      \
    void CLASS::deleteInstance()                                        \
    {                                                                   \
        if (sInstance)                                                  \
        {                                                               \
            sInstance->getTaskMgr()->destroyTaskSync(sInstance);        \
            sInstance = nullptr;                                        \
        }                                                               \
    }

#define SEAD_TASK_SINGLETON_DISPOSER_IMPL(CLASS)                \
    CLASS* CLASS::sInstance = nullptr;                          \
                                                                \
    SEAD_TASK_SET_SINGLETON_INSTANCE(CLASS)                     \
    SEAD_TASK_DELETE_SINGLETON_INSTANCE(CLASS)                  \
                                                                \
    CLASS::SingletonDisposer_::~SingletonDisposer_()            \
    {                                                           \
        if (mIsSetAsSingleton_)                                 \
            CLASS::sInstance = nullptr;                         \
    }