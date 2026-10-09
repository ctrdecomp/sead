#include "devenv/seadStackTrace.h"
#include "thread/seadThread.h"

#include <stdio.h>
#include <nn/os.h>

namespace
{
u32 getStackPointer()
{
    return __current_sp();
}
} // namespace
namespace sead
{
StackTraceBase::StackTraceBase()
{
}

void StackTraceBase::trace(void* stack)
{
    u32 stack_ = reinterpret_cast<u32>(stack);
    if (stack_ == NULL)
        stack_ = getStackPointer();

    u32* stackBottom = reinterpret_cast<u32*>(findThreadStackBottomByStackAddr_(stack_));

    if (stackBottom == NULL)
        return;

    u32 codeStart = nn::os::GetCodeRegionAddress();
    u32 codeEnd = codeStart + nn::os::GetCodeRegionSize();

    for (u32* p = reinterpret_cast<u32*>(stack); p < stackBottom; ++p)
    {
        u32 value = *p;

        if ((value & 3) != 0)
            continue;

        u32 addr = value - 4;

        if (addr < codeStart || addr > codeEnd)
            continue;

        u32 instruction = *reinterpret_cast<u32*>(addr);

        if ((instruction & 0xfe000000) == 0xfa000000 ||
            (instruction & 0x0fffff0f) == 0x012fff30 ||
            (instruction & 0x0f000000) == 0x0b000000)
        {
            push_(value);

            if (isFull_())
                break;
        }
    }
}

u32 StackTraceBase::findThreadStackBottomByStackAddr_(uptr addr)
{
    if (addr < 0x0e000000 || addr >= 0x10000000)
        return 0x10000000;

    ThreadMgr* mgr = ThreadMgr::instance();

    for (ThreadList::constIterator it = mgr->constBegin(); it != mgr->constEnd(); ++it)
    {
        Thread* thread = *it;
        u32 stackBottom = thread->GetStackBottom();

        Thread* thread2 = *it;
        u32 stackStart = thread2->getStackSize();

        if (stackBottom - addr <= stackBottom - addr &&
            addr < stackBottom)
        {
            return stackBottom;
        }
    }

    return 0;
}
}

