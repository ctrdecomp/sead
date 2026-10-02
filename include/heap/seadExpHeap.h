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
        FirstFit = 0,
        BestFit = 1,
    };

    enum FindFreeBlockMode
    {
        Auto = 0,
        FromFreeList = 1,
        ByIteratingMemBlock = 2,
    };

    enum FindMode
    {
        cFirstFit = 0,
        cBestFit,
        cMaxSize
    };

    static ExpHeap* create(size_t size, const SafeString& name, Heap* parent,
                           s32 alignment = sizeof(void*),
                           HeapDirection direction = cHeapDirection_Forward,
                           bool enable_lock = false);
    static ExpHeap* create(void* address, size_t size, const SafeString& name,
                           bool enable_lock = false);

    static ExpHeap* tryCreate(size_t size, const SafeString& name, Heap* parent,
                              s32 alignment = sizeof(void*),
                              HeapDirection direction = cHeapDirection_Forward,
                              bool enable_lock = false);
    static ExpHeap* tryCreate(void* address, size_t size, const SafeString& name,
                              bool enable_lock = false);

    static size_t getManagementAreaSize(s32);

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
    virtual size_t getMaxAllocatableSize(s32 alignment = cDefaultAlignment) const = 0;
    virtual bool isInclude(const void* p_void) const;
    virtual bool isEmpty() const;
    virtual bool isFreeable() const;
    virtual bool isResizable() const;
    virtual bool isAdjustable() const;
    virtual void dump() const;
    virtual void dumpYAML(WriteStream& stream, int i) const;
    virtual void genInformation_(hostio::Context* context);

    virtual s32 destroyAndGetAllocatableSize(s32);
    virtual void setFindFreeBlockMode(FindFreeBlockMode mode);

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
};
}  // namespace sead
