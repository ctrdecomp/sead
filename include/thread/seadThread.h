#pragma once

#include <utility>
#include <nn/os.h>

#include <basis/seadAssert.h>
#include <container/seadTList.h>
#include <heap/seadDisposer.h>
#include <heap/seadHeapMgr.h>
#include <hostio/seadHostIONode.h>
#include <hostio/seadHostIOReflexible.h>
#include <prim/seadEnum.h>
#include <prim/seadNamable.h>
#include <prim/seadSafeString.h>
#include <prim/seadScopedLock.h>
#include <thread/seadMessageQueue.h>
#include <thread/seadThreadLocalStorage.h>
#include <time/seadTickSpan.h>

namespace sead
{

class Heap;
class Thread;

typedef TList<Thread*> ThreadList;
typedef TListNode<Thread*> ThreadListNode;

class Thread : public IDisposer, public INamable, public hostio::Reflexible
{
public:
    enum State
    {
        cInitialized,
        cRunning,
        cQuitting,
        cTerminated,
        cReleased
    };


    Thread(const SafeString& name, Heap* heap, s32 priority, MessageQueue::BlockType block_type,
        MessageQueue::Element quit_msg, s32 stack_size, s32 message_queue_size);
    Thread(Heap* heap, nn::os::Thread* pThread, u32 thread_id);
    virtual ~Thread();

    Thread(const Thread&){ };
    Thread& operator=(const Thread&){ return *this; };

    virtual void destroy() { waitDone(); }

    virtual bool sendMessage(MessageQueue::Element msg, MessageQueue::BlockType block_type);
    virtual MessageQueue::Element recvMessage(MessageQueue::BlockType block_type);
    virtual const MessageQueue& getMessageQueue() const { return mMessageQueue; }

    virtual bool start();
    virtual void quit(bool is_jam);
    virtual void waitDone();
    virtual void quitAndDestroySingleThread(bool is_jam) { quitAndWaitDoneSingleThread(is_jam); }
    virtual void quitAndWaitDoneSingleThread(bool is_jam);

    virtual void setPriority(s32 prio);
    virtual s32 getPriority() const;
    virtual MessageQueue::BlockType getBlockType() const { return mBlockType; }
    virtual s32 getStackSize() const { return mStackSize; }
    virtual s32 calcStackUsedSizePeak() const;

    u32 getId() const { return mId; }
    State getState() const { return mState; }
    bool isDone() const { return mState == cTerminated || mState == cReleased; }
    bool isActive() const { return mState == cRunning || mState == cQuitting; }

    uptr GetStackBottom() const { return (uptr)PtrUtil::addOffset(mStackTopForCheck, mStackSize); }

    static void yield();
    static void sleep(TickSpan howLong);

    void checkStackOverFlow(const char* source_file, s32 source_line) const;
    void setStackOverflowExceptionEnable(bool);

    ThreadListNode* getThreadListNode() { return &mListNode; }

#ifdef SEAD_DEBUG
    virtual void listenPropertyEvent(const hostio::PropertyEvent* event);
    virtual void genMessage(hostio::Context* context);
#endif

    bool isDefaultPriority() const { return getPriority() == cDefaultPriority; }

    Heap* getCurrentHeap() const { return mCurrentHeap; }
    Heap* setCurrentHeap(Heap* heap)
    {
        Heap* oldHeap = mCurrentHeap;
        mCurrentHeap = heap;
        return oldHeap;
    }
    FindContainHeapCache* getFindContainHeapCache() { return &mFindContainHeapCache; }

    static const s32 cDefaultSeadPriority;

    static const s32 cDefaultPriority;

    static const s32 cDefaultMsgQueueSize = 32;
    static const s32 cDefaultStackSize = 0x1000;
    static const s32 cDefaultQuitMsg = 0x7FFFFFFF;
protected:
    static void ctrThreadFunc_(uptr param);

    virtual void run_();
    virtual void calc_(MessageQueue::Element msg) = 0;

    virtual uintptr_t getStackCheckStartAddress_() const;

    void initStackCheck_();

    MessageQueue mMessageQueue;
    s32 mStackSize;
    ThreadListNode mListNode;
    Heap* mCurrentHeap;
    FindContainHeapCache mFindContainHeapCache;
    MessageQueue::BlockType mBlockType;
    MessageQueue::Element mQuitMsg;
    u32 mId;
    State mState;
    nn::os::Thread* mThreadInner;
    void* mStackTop;
    void* mStackTopForCheck;
    s32 mPriority;
};

class ThreadMgr : public hostio::Node
{
public:
    ThreadMgr();
    virtual ~ThreadMgr();

    void initialize(Heap* heap);
    void destroy();

    ThreadList::constIterator constBegin() const { return mList.constBegin(); }
    ThreadList::constIterator constEnd() const { return mList.constEnd(); }

    bool isMainThread() const;
    Thread* getMainThread() const { return mMainThread; }
    Thread* getCurrentThread() const { return reinterpret_cast<Thread*>(mThreadPtrTLS.getValue()); }

    static void waitDoneMultipleThread(Thread* const* threads, s32 num);
    static void quitAndWaitDoneMultipleThread(Thread** threads, s32 num, bool is_jam);

    CriticalSection* getListCS() { return &mListCS; }

#ifdef SEAD_DEBUG
    void initHostIO();
    virtual void genMessage(hostio::Context* context);
    virtual void listenPropertyEvent(const hostio::PropertyEvent* event){ }
#endif

protected:
    friend class Thread;

    void addThread_(Thread* thread){
        ScopedLock<CriticalSection> lock(getListCS());
        mList.pushBack(thread->getThreadListNode());
    }

    void removeThread_(Thread* thread){
        ScopedLock<CriticalSection> lock(getListCS());
        mList.erase(thread->getThreadListNode());
    }

    void initMainThread_(Heap* heap);
    void destroyMainThread_();
    static u32 getCurrentThreadID_();

private:
    ThreadList mList;
    CriticalSection mListCS;
    Thread* mMainThread;
    ThreadLocalStorage mThreadPtrTLS;

    SEAD_SINGLETON_DISPOSER(ThreadMgr)
};

class MainThread : public Thread
{
public:
    MainThread(Heap* heap, nn::os::Thread* nn_thread, u32 thread_id): 
        Thread(heap, nn_thread, thread_id)
    {}

    virtual ~MainThread() { mState = cTerminated; }

    virtual void destroy() { SEAD_ASSERT_MSG(false, "Main thread can not destroy"); }
    virtual void quit(bool) { SEAD_ASSERT_MSG(false, "Main thread can not quit"); }
    virtual void waitDone() { SEAD_ASSERT_MSG(false, "Main thread can not waitDone"); }
    virtual void quitAndDestroySingleThread(bool)
    {
        SEAD_ASSERT_MSG(false, "Main thread can not quit");
    }
    virtual void setPriority(s32) { SEAD_ASSERT_MSG(false, "Main thread can not set priority"); }

protected:
    virtual void calc_(MessageQueue::Element) {}
};

} // namespace sead
