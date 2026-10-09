#pragma once

#ifndef SEAD_RAM_COMMAND_CACHE_CTR_H_
#include <gfx/ctr/seadRamCommandCacheCtr.h>
#endif

namespace sead
{
inline size_t RawCommandCacheCtr::adjust(Heap* h, size_t size)
{
    if(h == nullptr)
    {
        h = HeapMgr::instance()->getCurrentHeap();
    }
    mTopSize = size - (u32)mTopPtr >> 2;
    mTopPtr = h->resizeBack(mTopPtr, mTopSize << 2);
    return mTopSize;
}

inline void RawCommandCacheCtr::initialize(Heap* h, size_t size)
{
    if(h == nullptr)
    {
        h = HeapMgr::instance()->getCurrentHeap();
    }
    if(size == 0)
    {
        h->getMaxAllocatableSize(4);
    }
    mTopPtr = h->alloc(size, 4);
}

inline u32 RawCommandCacheCtr::useCache(u32* src) const
{
    MemUtil::copy(src, mTopPtr, mTopSize << 2);
    return src[mTopSize];
}
}
