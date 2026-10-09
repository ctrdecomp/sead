// Filename: seadThreadCtr.cpp
//
// Project: StandardEAD C++ Library for CTR

#include "thread/seadThread.h"
#include "basis/seadWarning.h"

namespace sead
{
Thread::Thread(const SafeString& name, Heap* heap, s32 priority, MessageQueue::BlockType block_type,MessageQueue::Element quit_msg, s32 stack_size, s32 message_queue_size):
    IDisposer(),
    INamable(name),
    mMessageQueue(),
    mStackSize(stack_size),
    mListNode(this),
    mCurrentHeap(nullptr),
    mFindContainHeapCache(),
    mBlockType(block_type), 
    mQuitMsg(quit_msg),
    mId(0),
    mState(cInitialized),
    mPriority(priority)
{
    mMessageQueue.allocate(message_queue_size, heap);
    mStackTop = new u8[stack_size];

    initStackCheck_();
    if (ThreadMgr::instance())
        ThreadMgr::instance()->addThread_(this);
    else
        SEAD_ASSERT_MSG(false, "ThreadMgr not initialized");
}

Thread::Thread(Heap* heap, nn::os::Thread* pThread, u32 thread_id): 
    IDisposer(),
    INamable("sead::MainThread"),
    mMessageQueue(),
    mStackSize(0),
    mListNode(this),
    mCurrentHeap(nullptr),
    mFindContainHeapCache(),
    mBlockType(MessageQueue::cNoBlock), 
    mQuitMsg(0),
    mId(thread_id),
    mState(cInitialized)
{
    pThread->GetPriority();
    mMessageQueue.allocate(32, heap);
}

Thread::~Thread()
{
    if (!ThreadMgr::instance())
    {
        SEAD_ASSERT_MSG(false, "ThreadMgr not initialized");
        return;
    }

    if (ThreadMgr::instance()->getMainThread() != this)
    {
        ThreadMgr::instance()->removeThread_(this);

        if (mState != cQuitting && mState != cTerminated)
        {
            if (mState == cRunning)
            {
                SEAD_ASSERT_MSG(false, "Thread is running. Do quit and waitDone");
                quitAndWaitDoneSingleThread(false);
            }
        }
        else
        {
            SEAD_ASSERT_MSG(false, "Thread is not done. Do waitDone");
            waitDone();
        }

        mThreadInner->Finalize();

        if(mThreadInner)
            delete(mThreadInner);

        if (mStackTop)
            delete[] static_cast<u8*>(mStackTop);
    }

    mMessageQueue.free();
}

bool Thread::start()
{
    if (mState)
    {
        SEAD_WARNING("Thread is running or done. Can not start.\n");
        return false;
    }

    mThreadInner->TryStart(ctrThreadFunc_, reinterpret_cast<uptr>(this), *this, mPriority);

    if (mState == cInitialized)
        mState = cRunning;

    return true;
}

void Thread::waitDone()
{
    if ((mState | cReleased) == cReleased)
        return;

    mThreadInner->Join();
    SEAD_ASSERT_MSG(mState == cTerminated, "Join failed?");
    mState = cReleased;
}

void Thread::setPriority(s32 prio)
{
    mPriority = prio;
    if(isActive())
    {
        Thread* thread = ThreadMgr::instance()->getCurrentThread();
        if(thread == this)
        {
            nn::os::Thread::ChangeCurrentPriority(prio);
        }
        else
        {
            mThreadInner->ChangePriority(prio);
        }
    }
}

s32 Thread::getPriority() const
{
    return mPriority;
}

void Thread::yield()
{
    nn::os::Thread* thread;
    thread->Yield();
}

void Thread::sleep(TickSpan howLong)
{
    nn::os::Thread* thread;
    nn::os::Tick tick(howLong.toS64());
    thread->Sleep(tick);
}

uintptr_t Thread::getStackCheckStartAddress_() const
{
    return uintptr_t(mStackTopForCheck);
}

void Thread::ctrThreadFunc_(uptr arg)
{
    sead::Thread* self = reinterpret_cast<sead::Thread*>(arg);

    ThreadMgr::instance()->mThreadPtrTLS.setValue(reinterpret_cast<uintptr_t>(self));

    const u32 id = self->mThreadInner->GetCurrentId();
    self->mState = cRunning;
    self->mId = id;
    self->run_();
    self->mState = cTerminated;
}

/* sead::ThreadMgr */

u32 ThreadMgr::getCurrentThreadID_()
{
    return u32(uintptr_t(nn::os::Thread::GetCurrentId()));
}

void ThreadMgr::initMainThread_(Heap* heap)
{
    nn::os::Thread& nn_thread = nn::os::Thread::GetMainThread();
    const u64 thread_id = nn::os::Thread::GetCurrentId();

    Thread* thread = new (heap) MainThread(heap, &nn_thread, thread_id);
    mMainThread = thread;
    mThreadPtrTLS.setValue(uintptr_t(thread));
}
}// namespace sead
