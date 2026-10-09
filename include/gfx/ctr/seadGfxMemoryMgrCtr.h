#pragma once

#include "heap/seadHeap.h"

namespace sead
{
class GfxMemoryMgrCtr
{
    SEAD_RTTI_BASE(GfxMemoryMgrCtr);
public:
    GfxMemoryMgrCtr();
    
    virtual s32  allocate(size_t area, u32 alignment, u32 size, Heap* heap) = 0;
    virtual void deallocate(size_t area, u32 alignment, u32 size, void* obj) = 0;

    static u32 aimToAlignment(u32 alignment);
    
    void setInitialize(bool b){ mIsInitialized = b; }
private:
    bool mIsInitialized;
};

class DefaultGfxMemoryMgrCtr : public GfxMemoryMgrCtr
{
    SEAD_RTTI_OVERRIDE(GfxMemoryMgrCtr, DefaultGfxMemoryMgrCtr);
public:
    DefaultGfxMemoryMgrCtr()
    {
    }
    
    DefaultGfxMemoryMgrCtr(Heap* heap);

    virtual s32  allocate(size_t area, u32 alignment, u32 size, Heap* heap);
    virtual void deallocate(size_t area, u32 alignment, u32 size, void* obj);
    virtual ~DefaultGfxMemoryMgrCtr();

    struct State
    {
        uptr mVramAStart;
        uptr mVramBStart;
    };

    void loadCurrentState(const State& dst);
    void saveCurrentState(State* dst) const;

    Heap* mGfxHeap;
    uptr mMemVramAStart;
    uptr mMemVramBStart;
    uptr mMemVramAEnd;
    uptr mMemVramBEnd;
    s32 _1C;
    s32 _20;
};
}
