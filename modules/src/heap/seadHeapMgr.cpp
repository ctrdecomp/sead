#include <heap/seadExpHeap.h>
#include <heap/seadHeap.h>
#include <heap/seadHeapMgr.h>
#include <prim/seadScopedLock.h>
#include <thread/seadThread.h>
#include <time/seadTickSpan.h>

namespace sead
{

HeapMgr* HeapMgr::sInstancePtr = nullptr;
HeapMgr HeapMgr::sInstance;

Arena* HeapMgr::sArena = nullptr;
Arena HeapMgr::sDefaultArena;

AtomicU32 HeapMgr::sHeapCheckTag;
CriticalSection HeapMgr::sHeapTreeLockCS;

HeapMgr::RootHeaps HeapMgr::sRootHeaps;
HeapMgr::IndependentHeaps HeapMgr::sIndependentHeaps;

HeapMgr::HeapMgr():

#if defined(SEAD_DEBUG)
    mDebugFillHeapCreate(cDefaultDebugFillHeapCreate), 
    mDebugFillAlloc(cDefaultDebugFillAlloc), 
    mDebugFillFree(cDefaultDebugFillFree), 
    mDebugFillHeapDestroy(cDefaultDebugFillHeapDestroy), 
    mIsEnableDebugFillHeapCreate(false), 
    mIsEnableDebugFillAlloc(true), 
    mIsEnableDebugFillFree(true), 
    mIsEnableDebugFillHeapDestroy(true), 
    mAllocCallback(nullptr), 
    mAllocFailedCallback(nullptr), 
    mFreeCallback(nullptr), 
    mCreateCallback(nullptr), 
    mDestroyCallback(nullptr),
#else
    mAllocFailedCallback(nullptr),
#endif // SEAD_DEBUG
    mAllocFromNotSeadThreadHeap(nullptr)
{
}

HeapMgr::~HeapMgr()
{
}

void HeapMgr::initialize(size_t size)
{
    sHeapTreeLockCS.lock();

    sArena = &sDefaultArena;
    sDefaultArena.initialize(size);
    initializeImpl_();

    sHeapTreeLockCS.unlock();
}

void HeapMgr::initialize(Arena* arena)
{
    sArena = arena;
    initializeImpl_();
}

void HeapMgr::initializeImpl_()
{
#if defined(SEAD_DEBUG)
    sInstance.mAllocCallback = nullptr;
    sInstance.mAllocFailedCallback = nullptr;
    sInstance.mFreeCallback = nullptr;
    sInstance.mCreateCallback = nullptr;
    sInstance.mDestroyCallback = nullptr;
#else
    sInstance.mAllocFailedCallback = nullptr;
#endif // SEAD_DEBUG

    HeapMgr::createRootHeap_();

    sInstancePtr = &sInstance;
}

void HeapMgr::createRootHeap_()
{
    ExpHeap* expHeap =
        ExpHeap::tryCreate(sArena->mStart, sArena->mSize, "RootHeap", false);

    sRootHeaps.pushBack(expHeap);
}

Heap* HeapMgr::findContainHeap(const void* memBlock) const
{
    ScopedLock<CriticalSection> lock(&sHeapTreeLockCS);

    for (RootHeaps::iterator it_end = sRootHeaps.end(), it = sRootHeaps.begin(); it != it_end; ++it)
    {
        Heap* found = it->findContainHeap_(memBlock);
        if (found != nullptr)
            return found;
    }

    for (IndependentHeaps::iterator it_end = sIndependentHeaps.end(), it = sIndependentHeaps.begin(); it != it_end; ++it)
    {
        Heap* found = it->findContainHeap_(memBlock);
        if (found != nullptr)
            return found;
    }

    return nullptr;
}

void HeapMgr::destroy()
{
    sHeapTreeLockCS.lock();

    sInstance.mAllocFailedCallback = nullptr;

    while (!sIndependentHeaps.isEmpty())
    {
        sIndependentHeaps.back()->destroy();
        sIndependentHeaps.popBack();
    }

    while (!sRootHeaps.isEmpty())
    {
        sRootHeaps.back()->destroy();
        sRootHeaps.popBack();
    }

    sInstancePtr = nullptr;

    sArena->destroy();
    sArena = nullptr;

    sHeapTreeLockCS.unlock();
}

void HeapMgr::initHostIO()
{
#if defined(SEAD_DEBUG)
    hostio::AddNode(HostIOMgr::instance()->getSeadRoot(), "HeapMgr", this, "$SEAD_META_HEAPMGR");
#endif // SEAD_DEBUG
}

void HeapMgr::setAllocFromNotSeadThreadHeap(Heap* heap)
{
    mAllocFromNotSeadThreadHeap = heap;
}

Heap* HeapMgr::getCurrentHeap() const
{
    Thread* currentThread = ThreadMgr::instance()->getCurrentThread();

    if (currentThread)
        return currentThread->getCurrentHeap();

    return mAllocFromNotSeadThreadHeap;
}

Heap* HeapMgr::setCurrentHeap_(Heap* heap)
{
    return ThreadMgr::instance()->getCurrentThread()->setCurrentHeap(heap);
}

void HeapMgr::removeRootHeap(Heap* heap)
{
    if (sRootHeaps.size() < 1)
        return;

    s32 index = sRootHeaps.indexOf(heap);

    if (index != -1)
        sRootHeaps.erase(index);
}

HeapMgr::IAllocFailedCallback*
HeapMgr::setAllocFailedCallback(HeapMgr::IAllocFailedCallback* callback)
{
    IAllocFailedCallback* old = mAllocFailedCallback;
    mAllocFailedCallback = callback;

    return old;
}

}  // namespace sead