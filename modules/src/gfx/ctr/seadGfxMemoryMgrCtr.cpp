// Filename: seadGfxMemoryMgrCtr.cpp
//
// Project: StandardEAD C++ Library for CTR

#include "gfx/ctr/seadGfxMemoryMgrCtr.h"
#include "math/seadMathCalcCommon.h"
#include <nn/gx.h>

namespace sead
{
GfxMemoryMgrCtr::GfxMemoryMgrCtr():
    mIsInitialized(false)
{
}

DefaultGfxMemoryMgrCtr::DefaultGfxMemoryMgrCtr(Heap* heap):
    GfxMemoryMgrCtr(),
    mGfxHeap(heap),
    mMemVramAStart(0),
    mMemVramBStart(0),
    _1C(0),
    _20(0)
{
    mMemVramAStart = GetVramStartAddr(MEM_VRAMA);
    mMemVramBStart = GetVramStartAddr(MEM_VRAMB);
    mMemVramAEnd = GetVramEndAddr(MEM_VRAMA);
    mMemVramBEnd = GetVramEndAddr(MEM_VRAMB);
}

u32 GfxMemoryMgrCtr::aimToAlignment(u32 alignment)
{
    u32 newAlignment = 128;
    switch(alignment)
    {
    case NN_GX_MEM_SYSTEM:
    {
        return newAlignment = 4;
    }
    case NN_GX_MEM_TEXTURE:
        break;
    case NN_GX_MEM_VERTEXBUFFER:
    case NN_GX_MEM_RENDERBUFFER:
    {
        return newAlignment = 192;
    }
    case NN_GX_MEM_DISPLAYBUFFER:
    case NN_GX_MEM_COMMANDBUFFER:
    {
        return newAlignment = 16;
    }
    default:
    {
        SEAD_ASSERT_MSG(false, "undefined aim(%d).", alignment);
    }
    }
}

DefaultGfxMemoryMgrCtr::~DefaultGfxMemoryMgrCtr()
{
    // Empty just like your love life [Literally def of a STRAIGHT WHITE MAN]
}

s32 DefaultGfxMemoryMgrCtr::allocate(size_t area, u32 alignment, u32 size, Heap* heap)
{
    alignment = GfxMemoryMgrCtr::aimToAlignment(alignment);

    if (alignment == 0)
    {
        if (area == NN_GX_MEM_FCRAM)
        {
            if (size != 0)
                mMemVramAStart += size;
        }
        else if (area == NN_GX_MEM_VRAMA)
        {
            if (size != 0)
                mMemVramBStart += size;
        }
    }
    else if (alignment == 1)
    {
        if (area == NN_GX_MEM_FCRAM)
        {
            if (size != 0)
                mMemVramAStart += size;
        }
        else if (area == NN_GX_MEM_VRAMA)
        {
            if (size != 0)
                mMemVramBStart += size;
        }
    }

    uptr start = 0;

    if (area == NN_GX_MEM_FCRAM)
    {
        start = reinterpret_cast<uintptr_t>(new (mGfxHeap) u8[size]);
    }
    else if (area == NN_GX_MEM_VRAMA)
    {
        start = MathCalcCommon<int>::roundUpN(alignment, mMemVramAStart);

        const void* end = PtrUtil::addOffset(reinterpret_cast<const void*>(start), size);

        if (PtrUtil::diff(reinterpret_cast<const void*>(mMemVramAEnd), end) < 0)
        {
            SEAD_ASSERT_MSG(false, "out of memory on VRAM-A.");
            return 0;
        }

        mMemVramAStart = (uptr)end;
    }
    else if (area == NN_GX_MEM_VRAMB)
    {
        start = MathCalcCommon<int>::roundUpN(alignment, mMemVramBStart);

        const void* end = PtrUtil::addOffset(reinterpret_cast<const void*>(start), size);

        if (PtrUtil::diff(reinterpret_cast<PtrUtil*>(mMemVramBEnd), end) < 0)
        {
            SEAD_ASSERT_MSG(false, "out of memory on VRAM-B.");
            return 0;
        }

        mMemVramBStart = (uptr)end;
    }
    else
    {
        SEAD_ASSERT_MSG(false, "undefined area(%d).", area);
    }

    return start;
}

void DefaultGfxMemoryMgrCtr::deallocate(size_t area, u32 alignment, u32 buf, void* obj)
{
    if(area == NN_GX_MEM_FCRAM)
    {
        delete obj;
    }
    else if(area != NN_GX_MEM_VRAMA && area != NN_GX_MEM_VRAMB)
    {
        SEAD_ASSERT_MSG(false, "undefined area(%d).", area);
    }
}

void DefaultGfxMemoryMgrCtr::loadCurrentState(const State& dst)
{
    mMemVramAStart = dst.mVramAStart;
    mMemVramBStart = dst.mVramBStart;
}

void DefaultGfxMemoryMgrCtr::saveCurrentState(State* dst) const
{
    SEAD_ASSERT(dst);

    dst->mVramAStart = mMemVramAStart;
    dst->mVramBStart = mMemVramBStart;
}
}