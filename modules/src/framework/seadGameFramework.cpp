#include <framework/seadGameFramework.h>

#include <basis/seadWarning.h>
#include <controller/seadControllerMgr.h>
#include <devenv/seadSeadMenuMgr.h>
#include <filedevice/seadFileDeviceMgr.h>
#include <framework/seadInfLoopChecker.h>
#include <framework/seadInfLoopCheckerThread.h>
#include <framework/seadMethodTreeMgr.h>
#include <framework/seadProcessMeter.h>
#include <framework/seadTaskBase.h>
#include <framework/seadTaskMgr.h>
#include <heap/seadExpHeap.h>
#include <heap/seadHeapMgr.h>
#include <hostio/seadHostIOFramework.h>
#include <hostio/seadHostIORoot.h>
#include <resource/seadResourceMgr.h>
#include <thread/seadThreadUtil.h>

#if defined(CTRSDK)
#include <nn/hio.h>
#endif

static void DefaultLockFunc(bool isLock)
{
    if (isLock)
    {
        sead::Graphics::instance()->lockDrawContext();
    }
    else
    {
        sead::Graphics::instance()->unlockDrawContext();
    }
}

namespace sead {

GameFramework::GameFramework(): 
    Framework(), 
    mDisplayState(cHide),
    mCalcMeter("calc", Color4f::cRed), 
    mDrawMeter("draw", Color4f::cGreen), 
    mGPUMeter("waitGPU", Color4f::cMagenta), 
    mCheckerThread(nullptr), 
    mFrameLockFunc(nullptr), 
    mProcDrawCallback(&DefaultLockFunc)
{
}

GameFramework::~GameFramework()
{
    if (mCheckerThread)
    {
        mCheckerThread->quitAndDestroySingleThread(false);
        delete mCheckerThread;
        mCheckerThread = nullptr;
    }
}

void GameFramework::initialize(const InitializeArg& arg)
{
    Framework::initialize(arg);

    Heap* root = HeapMgr::instance()->getRootHeap(0);

    ExpHeap* heap = ExpHeap::create(root->getMaxAllocatableSize(), "sead::SystemManagers", root);

    {
        ExpHeap* mgrHeap = ExpHeap::create(heap->getMaxAllocatableSize(), "sead::ResourceMgr", heap);

        ScopedCurrentHeapSetter chs(mgrHeap);
        ResourceMgr::createInstance(mgrHeap);

        mgrHeap->adjust();
    }

    {
        ExpHeap* mgrHeap = ExpHeap::create(heap->getMaxAllocatableSize(), "sead::FileDeviceMgr", heap);

        ScopedCurrentHeapSetter chs(mgrHeap);
        FileDeviceMgr::createInstance(mgrHeap);

        mgrHeap->adjust();
    }



#if defined(SEAD_DEBUG)
    {
        ExpHeap* hostioHeap = ExpHeap::create(heap->getMaxAllocatableSize(), "sead::HostIO", heap)

        CurrentHeapSetter chs(hostioHeap);

        nn::hio::CTR::Initialize(hostioHeap->alloc(0x14020, 4));

        hostioHeap->adjust();
    }
#endif // SEAD_DEBUG

    heap->adjust();
}

void GameFramework::startDisplay()
{
    if (mDisplayState == cHide)
        mDisplayState = cReady;
}

void GameFramework::lockFrameDrawContext()
{
    if (mFrameLockFunc)
        mFrameLockFunc(true);
}

void GameFramework::unlockFrameDrawContext()
{
    if (mFrameLockFunc)
        mFrameLockFunc(false);
}

void GameFramework::createSystemTasks(TaskBase* rootTask, const CreateSystemTaskArg& arg)
{
    Framework::createSystemTasks(rootTask, CreateSystemTaskArg());

    createControllerMgr(rootTask);
    createProcessMeter(rootTask);
    createSeadMenuMgr(rootTask);
    createHostIOMgr(rootTask, arg.hostio_parameter, arg.heap);
    createInfLoopChecker(rootTask, arg.infloop_detection_span, arg.infloop_thread_stack_size);
}

void GameFramework::createControllerMgr(TaskBase* rootTask)
{
    TaskBase::SystemMgrTaskArg arg(&TTaskFactory<ControllerMgr>);
    arg.parent = rootTask;

    getTaskMgr()->createSingletonTaskSync<ControllerMgr>(arg);
}

void GameFramework::createHostIOMgr(TaskBase* rootTask, HostIOMgr::Parameter* parameter, Heap* heap)
{
#if defined(SEAD_DEBUG)
    TaskBase::SystemMgrTaskArg arg(&TTaskFactory<HostIOMgr>);
    arg.parent = rootTask;
    arg.parameter = parameter;
    arg.heap_policies[arg.heap_policies.getPrimaryHeapIndex()].parent = heap;

    mTaskMgr->createSingletonTaskSync<HostIOMgr>(arg);

    HeapMgr::instance()->initHostIO();
    ThreadMgr::instance()->initHostIO();
    getTaskMgr()->initHostIO();
    initHostIO_();
    Graphics::instance()->initHostIO();
#endif // SEAD_DEBUG
}

void GameFramework::createProcessMeter(TaskBase* rootTask)
{
#if defined(SEAD_DEBUG)
    TaskBase::SystemMgrTaskArg arg(&TTaskFactory<ProcessMeter>);
    arg.parent = rootTask;

    mTaskMgr->createSingletonTaskSync<ProcessMeter>(arg);

    ProcessMeter::instance()->attachProcessMeterBar(&mCalcMeter);
    ProcessMeter::instance()->attachProcessMeterBar(&mDrawMeter);
    ProcessMeter::instance()->attachProcessMeterBar(&mGPUMeter);
#else
    // Nono processmeter, roll back to kitchen.
#endif // SEAD_DEBUG
}

void GameFramework::createSeadMenuMgr(TaskBase* rootTask)
{
#if defined(SEAD_DEBUG)
    TaskBase::SystemMgrTaskArg arg(&TTaskFactory<SeadMenuMgr>);
    arg.parent = rootTask;

    mTaskMgr->createSingletonTaskSync<SeadMenuMgr>(arg);
#endif // SEAD_DEBUG
}

void GameFramework::createInfLoopChecker(TaskBase* rootTask, const TickSpan& infLoopSpan, s32 infLoopThreadStackSize)
{
#if defined(SEAD_DEBUG)
    if (infLoopSpan.toS64() <= 0)
        return;

    TaskBase::SystemMgrTaskArg arg(&TTaskFactory<InfLoopChecker>);
    arg.parent = rootTask;

    mTaskMgr->createSingletonTaskSync<InfLoopChecker>(arg);

    InfLoopChecker* checker = InfLoopChecker::instance();
    checker->setThreshold(2);

    SEAD_ASSERT(!mCheckerThread);
    mCheckerThread = new(rootTask->getHeapArray().getPrimaryHeap()) InfLoopCheckerThread(infLoopSpan / 2.0f,
                                                                                         rootTask->getHeapArray().getPrimaryHeap(),
                                                                                         ThreadUtil::ConvertPrioritySeadToPlatform(8),
                                                                                         infLoopThreadStackSize);
    mCheckerThread->start();
#endif // SEAD_DEBUG
}

void GameFramework::waitStartDisplayLoop_()
{
    for(;;)
    {
        Graphics::instance()->lockDrawContext();
        {
            getTaskMgr()->beforeCalc();
            getTaskMgr()->afterCalc();
        }
        Graphics::instance()->unlockDrawContext();

        if (getTaskMgr()->getRootTask() || mDisplayState != cHide)
            break;

        Thread::sleep(TickSpan::makeFromMilliSeconds(10));
    }

    getMethodTreeMgr()->pauseAll(false);

    startDisplay();
}

} // namespace sead