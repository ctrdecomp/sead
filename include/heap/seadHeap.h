#ifndef SEAD_HEAP_H_
#define SEAD_HEAP_H_

#include <stddef.h>

#include <basis/seadAssert.h>
#include <basis/seadTypes.h>
#include <container/seadOffsetList.h>
#include <heap/seadDisposer.h>
#include <hostio/seadHostIOReflexible.h>
#include <prim/seadBitFlag.h>
#include <prim/seadEnum.h>
#include <prim/seadNamable.h>
#include <prim/seadRuntimeTypeInfo.h>
#include <prim/seadSafeString.h>
#include <thread/seadCriticalSection.h>

namespace sead
{
class Thread;
class WriteStream;

namespace hostio
{
class Context;
class PropertyEvent;
}  // namespace hostio

class Heap : public IDisposer, public INamable, public hostio::Reflexible
{
protected:
#if defined(SEAD_DEBUG)
protected:
    class ScopedDebugFillSystemDisabler
    {
    public:
        ScopedDebugFillSystemDisabler(Heap* heap)
            : mHeap(heap)
        {
            SEAD_ASSERT(mHeap);

            if (mHeap->isEnableLock())
                mHeap->mCS.lock();

            SEAD_ASSERT(Heap::isEnableDebugFillSystem_(mHeap));

            Heap::setEnableDebugFillSystem_(mHeap, false);
        }

        ~ScopedDebugFillSystemDisabler()
        {
            Heap::setEnableDebugFillSystem_(mHeap, true);

            if (mHeap->isEnableLock())
                mHeap->mCS.unlock();
        }

    private:
        Heap* mHeap;
    };
#endif // SEAD_DEBUG
public:
    enum Flag
    {
        cEnableLock = 0,
        cDisposing,
        cEnableWarning,
#if defined(SEAD_DEBUG)
        cEnableDebugFillSystem,
        cEnableDebugFillUser
#endif // SEAD_DEBUG
    };

    enum HeapDirection
    {
        cHeapDirection_Forward = 1,
        cHeapDirection_Reverse = -1
    };

    Heap(const SafeString& name, Heap* parent, void* address, size_t size, HeapDirection direction,
         bool);
    virtual ~Heap();

    SEAD_RTTI_BASE(Heap)

    virtual void destroy() = 0;
    virtual size_t adjust() = 0;
    virtual void* tryAlloc(size_t size, s32 alignment) = 0;
    virtual void free(void* ptr) = 0;
    virtual void* resizeFront(void*, size_t) = 0;
    virtual void* resizeBack(void*, size_t) = 0;
    virtual void* tryRealloc(void* ptr, size_t size, s32 alignment);
    virtual void freeAll() = 0;
    virtual uintptr_t getStartAddress() const = 0;
    virtual uintptr_t getEndAddress() const = 0;
    virtual size_t getSize() const = 0;
    virtual size_t getFreeSize() const = 0;
    virtual size_t getMaxAllocatableSize(s32 alignment = cDefaultAlignment) const = 0;
    virtual bool isInclude(const void*) const = 0;
    virtual bool isEmpty() const = 0;
    virtual bool isFreeable() const = 0;
    virtual bool isResizable() const = 0;
    virtual bool isAdjustable() const = 0;

    virtual void dump() const {}

    static const s32 cMinAlignment = sizeof(void*);
    static const size_t cMinAllocSize = cPtrSize;

#ifdef SEAD_DEBUG
    virtual void listenPropertyEvent(const hostio::PropertyEvent* event);
    virtual void genMessage(hostio::Context*);

    virtual void genInformation_(hostio::Context*);
    virtual void makeMetaString_(BufferedSafeString*);
#endif

    virtual void pushBackChild_(Heap* child);

    void appendDisposer_(IDisposer* disposer);
    void removeDisposer_(IDisposer* disposer);
    Heap* findContainHeap_(const void* ptr);

    void* alloc(size_t size, s32 alignment = sizeof(void*))
    {
        void* ptr = tryAlloc(size, alignment);
        SEAD_ASSERT_MSG(ptr,
                        "alloc failed. size: %zu, allocatable size: %zu, alignment: %d, heap: %s",
                        size, getMaxAllocatableSize(alignment), alignment, getName().cstr());
        return ptr;
    }

    void enableLock(bool on) { mFlag.changeBit(cEnableLock, on); }
    void enableWarning(bool on) { mFlag.changeBit(cEnableWarning, on); }

    bool isLockEnabled() const { return mFlag.isOnBit(cEnableLock); }
    bool isWarningEnabled() const { return mFlag.isOnBit(cEnableWarning); }
#ifdef SEAD_DEBUG
    bool isDebugFillSystemEnabled() const { return mFlag.isOnBit(cEnableDebugFillSystem); }
    bool isDebugFillUserEnabled() const { return mFlag.isOnBit(cEnableDebugFillUser); }

    void enableDebugFillSystem(bool on) { mFlag.changeBit(cEnableDebugFillSystem, on); }
    void enableDebugFillUser(bool on) { mFlag.changeBit(cEnableDebugFillUser, on); }

    bool isEnableDebugFillAlloc_() const;
    bool isEnableDebugFillFree_() const;
    bool isEnableDebugFillHeapDestroy_() const;
#endif

    friend class IDisposer;
    friend class HeapMgr;
    friend class PrintFormatter;

    void destruct_();
    void dispose_(const void* begin, const void* end);
    void eraseChild_(Heap* child);
    void checkAccessThread_() const;

    Heap* getParent() const { return mParent; }
    HeapDirection getDirection() const { return mDirection; }

    void setEnableLock(bool enable) { mFlag.changeBit(cEnableLock, enable); }
    bool isEnableLock() const { return mFlag.isOnBit(cEnableLock); }

    sead::CriticalSection& getCriticalSection() { return mCS; }

    typedef OffsetList<Heap> HeapList;
    typedef OffsetList<IDisposer> DisposerList;

    void* mStart;
    size_t mSize;
    Heap* mParent;
    HeapList mChildren;
    ListNode mListNode;
    DisposerList mDisposerList;
    HeapDirection mDirection;
    mutable CriticalSection mCS;
    BitFlag16 mFlag;
    u16 mHeapCheckTag;
#ifdef SEAD_DEBUG
    sead::Thread* mAccessThread;
#endif
};

inline void* Heap::tryRealloc(void*, size_t, s32)
{
    SEAD_ASSERT_MSG(false, "tryRealloc is not implement.");
    return nullptr;
}

}  // namespace sead

#endif  // SEAD_HEAP_H_

