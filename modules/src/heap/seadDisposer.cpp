#include <heap/seadDisposer.h>
#include <heap/seadHeap.h>
#include <heap/seadHeapMgr.h>

namespace
{
const u32 cDestructedFlag = 1;

}  // namespace

namespace sead
{
IDisposer::IDisposer():
    mDisposerHeap(nullptr)
{
}

IDisposer::IDisposer(Heap* const disposer_heap, HeapNullOption option)
{
    mDisposerHeap = disposer_heap;
    if (mDisposerHeap)
    {
        mDisposerHeap->appendDisposer_(this);
        return;
    }

    switch (option)
    {
    case AlwaysUseSpecifiedHeap:
        SEAD_ASSERT_MSG(false, "disposer_heap must not be nullptr");
    case UseSpecifiedOrContainHeap:
        if (!sead::HeapMgr::sInstancePtr)
            return;
        mDisposerHeap = sead::HeapMgr::sInstancePtr->findContainHeap(this);
        if (mDisposerHeap)
            mDisposerHeap->appendDisposer_(this);
        return;
    case DoNotAppendDisposerIfNoHeapSpecified:
        return;
    case UseSpecifiedOrCurrentHeap:
        if (!sead::HeapMgr::sInstancePtr)
            return;
        mDisposerHeap = sead::HeapMgr::sInstancePtr->getCurrentHeap();
        if (mDisposerHeap)
            mDisposerHeap->appendDisposer_(this);
        return;
    default:
        SEAD_ASSERT_MSG(false, "illegal option[%d]", int(option));
        return;
    }
}

IDisposer::~IDisposer()
{
    if (reinterpret_cast<uintptr_t>(mDisposerHeap) != cDestructedFlag)
    {
        if (mDisposerHeap != NULL)
            mDisposerHeap->removeDisposer_(this);

        *reinterpret_cast<uintptr_t*>(&mDisposerHeap) = cDestructedFlag;
    }
    else
    {
        #pragma line 96
        SEAD_ASSERT_MSG(false, "Destruct twice. [%p] Your class has possibilities for wrong order of multiple inheri tance.", this);
    }
}

}  // namespace sead
