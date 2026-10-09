#include <framework/seadFramework.h>
#include <framework/seadMethodTreeMgr.h>
#include <heap/seadExpHeap.h>
#include <heap/seadHeap.h>
#include <random/seadGlobalRandom.h>

namespace sead
{
Framework::CreateSystemTaskArg::CreateSystemTaskArg():
    hostio_parameter(nullptr), 
    heap(nullptr), 
    infloop_detection_span(0), 
    infloop_thread_stack_size(Thread::cDefaultStackSize)
{
}

Framework::InitializeArg::InitializeArg(): 
    heap_size(48 * 1024 * 1024), // 48 MB
    arena(nullptr)
{
}

Framework::RunArg::RunArg(): 
    prepare_stack_size(0), 
    prepare_priority(-1)
{
}


Framework::Framework(): 
    mReserveReset(false), 
    mResetParameter(NULL), 
    mResetEvent(), 
    mTaskMgr(NULL),
    mMethodTreeMgr(NULL), 
    mMethodTreeMgrHeap(NULL)
{
}

Framework::~Framework()
{
    if (mTaskMgr != NULL)
    {
        mTaskMgr->finalize();
        delete mTaskMgr;
        mTaskMgr = NULL;
    }

    if (mMethodTreeMgr != NULL)
    {
        delete mMethodTreeMgr;
        mMethodTreeMgr = NULL;
    }

    if (mMethodTreeMgrHeap != NULL)
        mMethodTreeMgrHeap->destroy();
}

void Framework::initialize(const InitializeArg& arg)
{
    if (arg.arena)
        HeapMgr::initialize(arg.arena);
    else
        HeapMgr::initialize(arg.heap_size);

    Heap* heap = HeapMgr::instance()->getRootHeap(0);

    {
        Heap* threadHeap = ExpHeap::create(0, "sead::ThreadMgr", heap);

        ThreadMgr::createInstance(threadHeap);
        ThreadMgr::instance()->initialize(threadHeap);

        threadHeap->adjust();
    }

    GlobalRandom::createInstance(heap);
}

void Framework::run(Heap* heap, const TaskBase::CreateArg& rootCreateArg, const RunArg& runArg)
{
    initRun_(heap);

    {
        Heap* methodTreeHeap = ExpHeap::create(0, "sead::MethodTreeMgr", heap);
        mMethodTreeMgr = createMethodTreeMgr_(methodTreeHeap);
        methodTreeHeap->adjust();

        mMethodTreeMgrHeap = methodTreeHeap;
    }

    TaskMgr::InitializeArg arg(rootCreateArg);

    if (runArg.prepare_stack_size != 0)
        arg.prepare_stack_size = runArg.prepare_stack_size;

    if (runArg.prepare_priority != -1)
        arg.prepare_priority = runArg.prepare_priority;

    arg.heap = heap;
    arg.parent_framework = this;

    mTaskMgr = TaskMgr::initialize(arg);

    runImpl_();
}

void Framework::createSystemTasks(TaskBase*, const CreateSystemTaskArg&)
{
}

void Framework::procReset_()
{
    if (mReserveReset)
    {
        mResetEvent.fire(mResetParameter);

        mTaskMgr->destroyAllAndCreateRoot();

        mReserveReset = false;
        mResetParameter = nullptr;
    }
}

}  // namespace sead

