#pragma once

#include "heap/seadHeap.h"
#include "heap/seadMemBlock.h"
#include "prim/seadSizedEnum.h"

namespace sead
{
class ExpHeap : public Heap
{
    SEAD_RTTI_OVERRIDE(ExpHeap, Heap)
public:
    enum AllocMode
    {
        cAllocFirstFit = 0,
        cAllocBestFit = 1,
    };

    enum FindFreeBlockMode
    {
        cAuto = 0,
        cFromFreeList = 1,
        cByIteratingMemBlock = 2,
    };

    enum FindMode
    {
        cFindFirstFit = 0,
        cFindBestFit,
        cFindMaxSize
    };

    static ExpHeap* create(size_t size, const SafeString& name, Heap* parent, HeapDirection direction = cHeapDirection_Forward, bool enableLock = false);

    static ExpHeap* create(void* start, size_t size, const SafeString& name, bool enableLock = false)
    {
        ExpHeap* heap = ExpHeap::tryCreate(start, size, name, enableLock);
        SEAD_ASSERT_MSG(heap, "heap create failed. [%s] start: 0x%p, size: %zu", name.cstr(), start, size);
        return heap;
    }

    static ExpHeap* tryCreate(size_t size, const SafeString& name, Heap* parent, HeapDirection direction = cHeapDirection_Forward, bool enableLock = false);
    static ExpHeap* tryCreate(void* start, size_t size, const SafeString& name, bool enableLock = false);

    static size_t getManagementAreaSize(s32 alignment = cDefaultAlignment);
    virtual void destroy();
    virtual size_t adjust();
    virtual void* tryAlloc(size_t size, s32 alignment);
    virtual void free(void* ptr);
    virtual void* resizeFront(void* p_void, size_t size);
    virtual void* resizeBack(void* p_void, size_t size);
    virtual void* tryRealloc(void* ptr, size_t size, s32 alignment);
    virtual void freeAll();
    virtual uintptr_t getStartAddress() const;
    virtual uintptr_t getEndAddress() const;
    virtual size_t getSize() const;
    virtual size_t getFreeSize() const;
    virtual size_t getMaxAllocatableSize(s32 alignment = cDefaultAlignment) const;
    virtual bool isInclude(const void* p_void) const;
    virtual bool isEmpty() const;
    virtual bool isFreeable() const;
    virtual bool isResizable() const;
    virtual bool isAdjustable() const;
    virtual void dump() const;

    AllocMode getAllocMode() const { return mAllocMode; }
    void setAllocMode(AllocMode mode) { mAllocMode = mode; }

    // XXX: this isn't const-correct...
    size_t getAllocatedSize(void* object);

    void dumpFreeList() const;
    void dumpUseList() const;

    void checkFreeList() const;
    bool tryCheckFreeList() const;
    void checkUseList() const;
    bool tryCheckUseList() const;

protected:
    ExpHeap(const SafeString& name, Heap* parent, void* address, size_t size,
            HeapDirection direction, bool);
    virtual ~ExpHeap();

    static void doCreate(ExpHeap*, Heap*);

    static void createMaxSizeFreeMemBlock_(ExpHeap*);
    MemBlock* findFreeMemBlockFromHead_(size_t, FindMode) const;
    MemBlock* findFreeMemBlockFromHead_(size_t, s32, FindMode) const;
    MemBlock* findFreeMemBlockFromTail_(size_t, FindMode) const;
    MemBlock* findFreeMemBlockFromTail_(size_t, s32, FindMode) const;
    MemBlock* findLastMemBlockIfFree_();
    MemBlock* findFirstMemBlockIfFree_();

    void* realloc_(void* ptr, u8* oldMem, size_t copySize, size_t newSize, s32 alignment);

    friend class PrintFormatter;

    void pushToUseList_(MemBlock*);
    void pushToFreeList_(MemBlock*);

    size_t adjustBack_();
    size_t adjustFront_();

    MemBlock* allocFromHead_(size_t);
    MemBlock* allocFromHead_(size_t, s32);
    MemBlock* allocFromTail_(size_t);
    MemBlock* allocFromTail_(size_t, s32);

    static s32 compareMemBlockAddr_(const MemBlock*, const MemBlock*);

    SizedEnum<AllocMode, u8> mAllocMode;
    SizedEnum<FindFreeBlockMode, u8> mFindFreeBlockMode;
    MemBlockList mFreeList;
    MemBlockList mUseList;
    size_t mFreeSize; // MAYBE HERE..?
};
}  // namespace sead

