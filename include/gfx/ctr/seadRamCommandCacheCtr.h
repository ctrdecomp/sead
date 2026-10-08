#pragma once

#include "heap/seadHeap.h"
#include "heap/seadHeapMgr.h"

namespace sead
{
class RawCommandCacheCtr
{
public:
    RawCommandCacheCtr():
        mTopPtr(NULL),
        mTopSize(0)
    {
    }

    ~RawCommandCacheCtr()
    {
    }

    size_t adjust(Heap* h, size_t size);
    void initialize(Heap* h, size_t size);
    u32 useCache(u32* src) const;

    void* getTopPtr() const{ return mTopPtr; }
    size_t getTopSize() const{ return mTopSize; }
private:
    void* mTopPtr;
    size_t mTopSize;
};

}

#define SEAD_RAM_COMMAND_CACHE_CTR_H_
#include <gfx/ctr/seadRamCommandCacheCtr.hpp>
#undef SEAD_RAM_COMMAND_CACHE_CTR_H_
