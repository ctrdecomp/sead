#ifndef SEAD_HEAPMGR_H_
#define SEAD_HEAPMGR_H_

#include <container/seadPtrArray.h>
#include <heap/seadArena.h>
#include <heap/seadHeap.h>
#include <hostio/seadHostIONode.h>
#include <prim/seadDelegate.h>
#include <prim/seadSafeString.h>
#include <thread/seadAtomic.h>
#include <thread/seadCriticalSection.h>
#include <time/seadTickSpan.h>

#define NUM_ROOT_HEAPS_MAX        4
#define NUM_INDEPENDENT_HEAPS_MAX 4

namespace sead
{
class HeapMgr : public hostio::Node
{
    struct AllocCallbackArg;
    struct CreateCallbackArg;
    struct DestroyCallbackArg;
    struct FreeCallbackArg;
    typedef IDelegate1<const AllocCallbackArg*> IAllocCallback;
    typedef IDelegate1<const CreateCallbackArg*> ICreateCallback;
    typedef IDelegate1<const DestroyCallbackArg*> IDestroyCallback;
    typedef IDelegate1<const FreeCallbackArg*> IFreeCallback;

public:
    struct AllocFailedCallbackArg
    {
        Heap* heap;
        size_t request_size;
        s32 request_alignment;
        size_t alloc_size;
        s32 alloc_alignment;
    };

    typedef IDelegate1<const AllocFailedCallbackArg*> IAllocFailedCallback;

    HeapMgr();
    virtual ~HeapMgr();

    static void initialize(size_t size);
    static void initializeImpl_();
    static void initialize(Arena* arena);
    static void createRootHeap_();
    static void destroy();
    void initHostIO();

    Heap* findContainHeap(const void* ptr) const;
    void setAllocFromNotSeadThreadHeap(Heap* heap);

    Heap* getCurrentHeap() const;

    static void removeRootHeap(Heap*);

    IAllocFailedCallback* setAllocFailedCallback(IAllocFailedCallback* callback);
    IAllocFailedCallback* getAllocFailedCallback() { return mAllocFailedCallback; }

    static HeapMgr* instance() { return sInstancePtr; }
    static s32 getRootHeapNum() { return sRootHeaps.size(); }

    static Heap* getRootHeap(s32 index) { return sRootHeaps[index]; }

    static u32 getHeapCheckTag() { return sHeapCheckTag.increment(); }
    static u32 peekHeapCheckTag() { return sHeapCheckTag.getValue(); }

    typedef FixedPtrArray<Heap, NUM_ROOT_HEAPS_MAX> RootHeaps;
    typedef FixedPtrArray<Heap, NUM_INDEPENDENT_HEAPS_MAX> IndependentHeaps;

    static HeapMgr sInstance;
    static HeapMgr* sInstancePtr;
    static Arena* sArena;
    static Arena sDefaultArena;
    static AtomicU32 sHeapCheckTag;
    static RootHeaps sRootHeaps;
    static CriticalSection sHeapTreeLockCS;
    static IndependentHeaps sIndependentHeaps;

protected:

    friend class ExpHeap;
    friend class FrameHeap;
    friend class UnboundHeap;
    friend class CurrentHeapSetter;

private:
    friend class ScopedCurrentHeapSetter;

    /// Set the current heap to the specified heap and returns the previous "current heap".
    Heap* setCurrentHeap_(Heap* heap);
#if defined(SEAD_DEBUG)
    u8 mDebugFillHeapCreate;
    u8 mDebugFillAlloc;
    u8 mDebugFillFree;
    u8 mDebugFillHeapDestroy;
    bool mIsEnableDebugFillHeapCreate;
    bool mIsEnableDebugFillAlloc;
    bool mIsEnableDebugFillFree;
    bool mIsEnableDebugFillHeapDestroy;
    IAllocCallback* mAllocCallback;
    IAllocFailedCallback* mAllocFailedCallback;
    IFreeCallback* mFreeCallback;
    ICreateCallback* mCreateCallback;
    IDestroyCallback* mDestroyCallback;
#else
    IAllocFailedCallback* mAllocFailedCallback;
#endif // SEAD_DEBUG
    Heap* mAllocFromNotSeadThreadHeap;
};

/// Sets the "current heap" to the specified heap and restores the previous "current heap"
/// when this goes out of scope.
class ScopedCurrentHeapSetter
{
public:
    explicit ScopedCurrentHeapSetter(sead::Heap* heap):
        mPreviousHeap(0)
    {
        if (heap)
            setPreviousHeap_(HeapMgr::instance()->setCurrentHeap_(heap));
        else
            setPreviousHeapToNone_();
    }

    ~ScopedCurrentHeapSetter()
    {
        if (hasPreviousHeap_())
            HeapMgr::instance()->setCurrentHeap_(getPreviousHeap_());
    }

protected:
    /// @warning Only call this if hasPreviousHeap returns true.
    Heap* getPreviousHeap_() const { return reinterpret_cast<Heap*>(mPreviousHeap); }
    void setPreviousHeap_(Heap* heap) { mPreviousHeap = reinterpret_cast<uintptr_t>(heap); }
    void setPreviousHeapToNone_() { mPreviousHeap = 1; }
    bool hasPreviousHeap_() const
    {
        // XXX: We cannot just do `mPreviousHeap != 1` because that results in different codegen.
        // The cast smells like implementation defined behavior, but 1 should not be a valid
        // pointer on any platform that we support. In practice, this will work correctly.
        return reinterpret_cast<Heap*>(mPreviousHeap) != reinterpret_cast<Heap*>(1);
    }

    uintptr_t mPreviousHeap;
};

class FindContainHeapCache
{
public:
    FindContainHeapCache():
        mHeap(0)
    {
    }

    bool tryRemoveHeap(Heap* heap);
    Heap* tryAddHeap()
    {
        mHeap |= 1;
        return reinterpret_cast<Heap*>(mHeap.load());
    }
    Heap* getHeap() const { return reinterpret_cast<Heap*>(mHeap.load()); }
    void setHeap(Heap* heap) { mHeap.storeNonAtomic(uintptr_t(heap)); }
    void resetHeap() { mHeap.fetchAnd(~1LL); }

    Atomic<u32> mHeap;
};

}  // namespace sead

#endif  // SEAD_HEAPMGR_H_
