#ifndef SEAD_HEAP_POLICIES_H_
#define SEAD_HEAP_POLICIES_H_

#include <basis/seadTypes.h>
#include <heap/seadHeapMgr.h>

namespace sead
{
class Heap;

class HeapArray
{
public:
    Heap* getHeap(s32 index) const
    {
        if (index < HeapMgr::getRootHeapNum())
            return mHeaps[index];
        return mHeaps[0];
    }

    Heap* getPrimaryHeap() const { return mHeaps[mPrimaryIndex]; }

    s32 getPrimaryHeapIndex() const
    {
        return mPrimaryIndex;
    }

    Heap* mHeaps[4];
    bool mAdjusted[4];
    s32 mPrimaryIndex;

    HeapArray()
        : mAdjusted()
        , mPrimaryIndex(0)
    {
    }
};

struct HeapPolicy
{
    Heap* parent;
    u32 size;
    u32 create_slack;
    u32 adjust_slack;
    u8 adjust;
    u8 temporary;
    u8 dont_create;

    HeapPolicy()
        : parent(NULL)
        , size(0)
        , create_slack(0)
        , adjust_slack(0)
        , adjust(0)
        , temporary(0)
        , dont_create(0)
    {
    }
};

struct HeapPolicies
{
    const HeapPolicy& operator[](s32 idx) const
    {
        if (idx >= HeapMgr::getRootHeapNum())
        {
            SEAD_ASSERT_MSG(false, "illegal idx: %d", idx);
            return mPolicies[0];
        }

        return mPolicies[idx];
    }

    void useOnly(s32 idx)
    {
        s32 rootHeapNum = HeapMgr::getRootHeapNum();
        if (idx >= rootHeapNum)
        {
            SEAD_ASSERT_MSG(false, "illegal idx: %d", idx);
            return;
        }

        for (s32 i = 0; i < rootHeapNum; i++)
        {
            mPolicies[i].dont_create = idx != i;
        }

        mPrimaryIndex = idx;
    }

    void useOnlyPrimaryHeap()
    {
        useOnly(mPrimaryIndex);
    }

    s32 getPrimaryHeapIndex() const
    {
        return mPrimaryIndex;
    }

    void setAdjustAll(bool b)
    {
        s32 rootHeapNum = HeapMgr::getRootHeapNum();
        for (s32 i = 0; i < rootHeapNum; i++)
        {
            mPolicies[i].adjust = b;
        }
    }

    HeapPolicy mPolicies[4];
    s32 mPrimaryIndex;

    HeapPolicies()
        : mPrimaryIndex(0)
    {
    }
};

}  // namespace sead

#endif  // SEAD_HEAP_POLICIES_H_