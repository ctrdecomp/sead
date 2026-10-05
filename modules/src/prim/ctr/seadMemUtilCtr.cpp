// Filename: seadMemUtilCtr.cpp
//
// Project: StandardEAD C++ Library for CTR

#include "prim/seadMemUtil.h"
#include "thread/seadThread.h"

namespace sead
{
bool MemUtil::isStack(const void* ptr)
{
    u32 addr = reinterpret_cast<u32>(ptr);

    if (addr >= 0x0E000000 && addr < 0x10000000)
        return true;

    ThreadMgr* mgr = ThreadMgr::instance();

    if (!mgr)
        return false;

    for (ThreadList::constIterator it = mgr->constBegin(); it != mgr->constEnd(); ++it) {
        Thread* thread = *it;

        uptr stackBottom = thread->GetStackBottom();
        u32 stackTop = thread->getStackSize();

        if (addr >= stackTop && addr < stackBottom)
            return true;
    }

    return false;
}
}