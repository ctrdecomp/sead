#pragma once

#include "basis/seadTypes.h"
#include "container/seadSafeArray.h"

namespace sead
{
class StackTraceBase
{
public:
    StackTraceBase();
    virtual ~StackTraceBase(){ };

    virtual uintptr_t get(s32 index) const = 0;
    virtual s32 size() const = 0;

    void trace(void* stack);
    u32 findThreadStackBottomByStackAddr_(uptr ptr);

protected:
    virtual void clear_() = 0;
    virtual void push_(uintptr_t addr) = 0;
    virtual bool isFull_() = 0;
};

template <s32 Capacity>
class StackTrace : public StackTraceBase
{
public:
    virtual ~StackTrace(){ };

    virtual uintptr_t get(s32 index) const
    {
        if (index >= mSize)
            return 0;
        return mBuffer[index];
    }

    virtual s32 size() const { return mSize; }

protected:
    virtual void clear_() { mSize = 0; }

    virtual void push_(uintptr_t addr)
    {
        mBuffer[mSize] = addr;
        ++mSize;
    }

    virtual bool isFull_() { return mSize >= mBuffer.size(); }

private:
    SafeArray<uintptr_t, Capacity> mBuffer;
    s32 mSize;
};
}  // namespace sead
