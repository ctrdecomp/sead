#include "thread/seadThread.h"
#include "basis/seadAssert.h"
#include "basis/seadWarning.h"
#include "prim/seadBitUtil.h"
#include "prim/seadPtrUtil.h"
#include "prim/seadScopedLock.h"
#include "hostio/seadHostIOFramework.h"
#include "hostio/seadHostIOMgr.h"
#include "thread/seadThreadUtil.h"

namespace sead
{
const s32 Thread::cDefaultPriority = 0x10;

bool Thread::sendMessage(MessageQueue::Element msg, MessageQueue::BlockType block_type)
{
    if (msg == MessageQueue::cNullElement){
        SEAD_ASSERT_MSG(false, "Can not send cNullElement(==%ld)", MessageQueue::cNullElement);
        return false;
    }

    if (isDone()){
        SEAD_ASSERT_MSG(false, "Thread is done. Reject message: %ld", msg);
        return false;
    }

    if (mQuitMsg == msg){
        SEAD_ASSERT_MSG(false, "use quit()");
        return false;
    }

    return mMessageQueue.push(msg, block_type);
}

MessageQueue::Element Thread::recvMessage(MessageQueue::BlockType block_type)
{
    if (mState == cQuitting)
        return 0;
    return mMessageQueue.pop(block_type);
}

void Thread::quit(bool is_jam)
{
    if (isDone()){
        SEAD_WARNING("Thread is done. Can not quit.");
        return;
    }

    if (mState == cRunning)
        mState = cQuitting;

    if (is_jam)
        mMessageQueue.jam(mQuitMsg, MessageQueue::cBlock);
    else
        mMessageQueue.push(mQuitMsg, MessageQueue::cBlock);
}

void Thread::quitAndWaitDoneSingleThread(bool is_jam)
{
    quit(is_jam);
    waitDone();
}

const u32 cStackCanaryMagic = 0x5EAD5CEC;

static bool checkStackMagic(uintptr_t addr)
{
    return BitUtil::bitCastPtr<u32>(reinterpret_cast<const void*>(addr)) == cStackCanaryMagic;
}

// FIXME
s32 Thread::calcStackUsedSizePeak() const
{
    u32* stackCheck = reinterpret_cast<u32*>(getStackCheckStartAddress_());
    if (stackCheck)
    {
        u32* stackCheckEnd = static_cast<u32*>(PtrUtil::addOffset(mStackTop, mStackSize));
        for (; stackCheck < stackCheckEnd; stackCheck++)
        {
            if (*stackCheck != cStackCanaryMagic)
                return PtrUtil::diff(stackCheckEnd, stackCheck);
        }
    }
    return 0;
}

void Thread::checkStackOverFlow(const char* source_file, s32 source_line) const
{
    const uintptr_t ptr = ThreadUtil::GetCurrentStackPointer();
    const uintptr_t start = getStackCheckStartAddress_();
    if (start)
    {
        SEAD_ASSERT_MSG(start <= ptr,
                        "sead::Thread Stack Pointer Overflow! [%s:%p]\n"
                        "  Source File: %s\n"
                        "  Line Number: %d\n"
                        "  Stack Size: %d, Over Size: %ld",
                        getName().cstr(), this,
                        source_file ? source_file : SafeString::cEmptyString.cstr(), source_line,
                        getStackSize(), start - ptr);
    }
}

void Thread::run_()
{
    while(true)
    {
#ifdef SEAD_DEBUG
        checkStackOverFlow(nullptr, 0);
#endif

        const MessageQueue::Element msg = mMessageQueue.pop(mBlockType);
        if (msg == mQuitMsg)
            break;

        calc_(msg);
    }
}

// NON_MATCHING: the first loop gets unrolled and the loop counter is not negated
void Thread::initStackCheck_()
{
    u32* stackCheck = reinterpret_cast<u32*>(getStackCheckStartAddress_());
    u32* stackCheckEnd = static_cast<u32*>(PtrUtil::addOffset(mStackTop, mStackSize));

    for (; stackCheck < stackCheckEnd; stackCheck++)
    {
        *stackCheck = cStackCanaryMagic;
    }
}

/* sead::ThreadMgr main */

SEAD_SINGLETON_DISPOSER_IMPL(ThreadMgr)

ThreadMgr::ThreadMgr():
#ifdef SEAD_DEBUG
    hostio::Node(),
#endif
    mList(),
    mListCS(),
    mMainThread(nullptr),
    mThreadPtrTLS()
{
}

ThreadMgr::~ThreadMgr()
{
    ScopedLock<CriticalSection> lock(getListCS());

    for (ThreadList::iterator it = mList.begin(); it != mList.end(); ++it)
    {
        Thread* thread = *it;
        thread->quit(false);
    }

    bool all_done;
    do
    {
        all_done = true;
        for (ThreadList::iterator it = mList.begin(); it != mList.end(); ++it)
        {
            Thread* thread = *it;
            all_done &= thread->isDone();
        }
        Thread::yield();
    } while (!all_done);

    for (ThreadList::iterator it = mList.begin(); it != mList.end(); ++it)
    {
        Thread* thread = *it;
        thread->waitDone();
    }
}

void ThreadMgr::initialize(Heap* heap)
{
    initMainThread_(heap);
    SEAD_ASSERT(mMainThread);
}

void ThreadMgr::destroy()
{
    destroyMainThread_();
}

void ThreadMgr::destroyMainThread_()
{
    if (mMainThread)
    {
        delete(mMainThread);
        mMainThread = nullptr;
    }
}

bool ThreadMgr::isMainThread() const
{
    return getCurrentThread() == mMainThread;
}

void ThreadMgr::waitDoneMultipleThread(Thread* const* threads, s32 num)
{
    bool all_done;
    do
    {
        all_done = true;
        for (s32 i = 0; i < num; ++i)
            all_done &= threads[i]->isDone();
        Thread::yield();
    } while (!all_done);

    for (s32 i = 0; i < num; ++i)
        threads[i]->waitDone();
}

void ThreadMgr::quitAndWaitDoneMultipleThread(Thread** threads, s32 num, bool is_jam)
{
    for (s32 i = 0; i < num; ++i)
        threads[i]->quit(is_jam);

    waitDoneMultipleThread(threads, num);
}

#ifdef SEAD_DEBUG
void ThreadMgr::initHostIO()
{
    hostio::AddNode(HostIOMgr::instance()->getSeadRoot(), "ThreadMgr", this, "$SEAD_META_THREADMGR");
}

void ThreadMgr::genMessage(hostio::Context* context)
{
    context->genNode(mMainThread->getName(), mMainThread, "$SEAD_META_THREAD");

    ScopedLock<CriticalSection> lock(&mIterateLockCS);
    for (ThreadList::iterator it = mList.begin(); it != mList.end(); ++it)
    {
        Thread* thread = *it;
        context->genNode(thread->getName(), thread, "$SEAD_META_THREAD");
    }
}
#endif
}  // namespace sead
