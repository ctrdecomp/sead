// Filename: seadExceptionScreenCtr.cpp
//
// Project: StandardEAD C++ Library for CTR

#include "devenv/ctr/seadExceptionScreenCtr.h"
#include "devenv/seadAssertConfig.h"
#include "devenv/seadStackTrace.h"
#include "framework/seadInfLoopChecker.h"
#include "stream/ctr/seadMemoryMapCtr.h"
#include "time/seadTickTime.h"

#include <nn/hid.h>

namespace sead
{
ExceptionScreenCtr* ExceptionScreenCtr::sExceptionScreenCtr = NULL;
const char* ExceptionScreenCtr::sMapFilePath = NULL;

ExceptionScreenCtr::ExceptionScreenCtr():
    mPrinter(),
    mEvent()
{
    sExceptionScreenCtr = this;

    nn::os::ARM::SetUserExceptionHandler(exceptionHandler, reinterpret_cast<uptr>(mExceptionStack.addOffset(mEvent.instance(), 0)));
}

ExceptionScreenCtr::~ExceptionScreenCtr()
{
}

void ExceptionScreenCtr::exceptionHandler(ARM::ExceptionInfo * info, ARM::ExceptionContext* context)
{
    FixedSafeString<2048> string;
    s32 len = string.format(
        "Exception occured.\n"
        "         type    : %d\n"
        "         fsr     : 0x%x\n"
        "         far     : 0x%x\n"
        "         fpexc   : 0x%x\n"
        "         fpinst  : 0x%x\n"
        "         fpinst2 : 0x%x\n",
        info->type,
        info->fsr,
        info->far,
        info->fpexc,
        info->fpinst,
        info->fpinst2
    );

    for(int i = 0; i < 16; i++)
    {
        BufferedSafeString stringR(static_cast<BufferedSafeString*>(&string), len);
        stringR.format("r%d: 0x%x\n", i, context->r[i]);        
        len += string.append(stringR);
    }
    s32 cpsrLen;
    {
        BufferedSafeString stringCpsr(static_cast<BufferedSafeString*>(&string), len);

        cpsrLen = stringCpsr.format(" cpsr: 0x%x\n", context->cpsr);
    }

    {
        BufferedSafeString stringBackTrace(static_cast<BufferedSafeString*>(&string), len + cpsrLen);

        putBackTraceString(&stringBackTrace, context->cpsr);
    }

    sExceptionScreenCtr->onHalt_(string.cstr());
}

s32 ExceptionScreenCtr::putBackTraceString(BufferedSafeString* string, size_t size)
{
    StackTrace<10> trace;
    trace.trace(&size);

    if (trace.size() <= 0)
    {
        string->clear();
        return 0;
    }

    MemoryMapCtr memoryMap;
    FixedSafeString<128> path;

    if (sExceptionScreenCtr != nullptr)
    {
        path = SafeString(sMapFilePath);
        memoryMap.open(path);
    }

    s32 length = string->copy(" back trace:\n", size);

    for (s32 i = 0; i < trace.size(); ++i)
    {
        u32 address = trace.get(i);

        BufferedSafeString line(string, length);
        line.format("  [0x%x] 0x%x", i, address);

        s32 offset = 0;

        if (memoryMap.searchFunction(&line, &offset, address))
        {
            line.appendWithFormat(" %s", line.cstr());

            line.appendWithFormat(" + 0x%04x", offset);
        }

        line.appendWithFormat("\n");

        length += string->append(line);
    }

    memoryMap.close();

    return length;
}

void ExceptionScreenCtr::onHalt_(const char* msg)
{
    ThreadMgr* threadMgr = ThreadMgr::instance();

    const bool isMainThread = threadMgr->isMainThread();

    if (!isMainThread)
    {
        Thread* currentThread = threadMgr->getCurrentThread();
#ifdef SEAD_DEBUG
        threadMgr->calcURL_(NULL);
#endif // SEAD_DEBUG
    }

    if (InfLoopChecker* checker = InfLoopChecker::instance())
        checker->setEnable(false);

    disableAssertCallback();

    Vector2i bufPos = Vector2i::zero;

    putHaltMessage_(bufPos, msg);
    mPrinter.flush();

    TickTime start;

    for (;;)
    {
        TickSpan delay = TickSpan::makeFromMilliSeconds(400);

        if (start.diffToNow().toS64() < delay.toS64())
            break;

        break;
    }

    PadReader padReader(nn::hid::CTR::GetPad());

    bool redraw = true;
    s32 yOffset = 0;

    for (;;)
    {
        if (redraw)
        {
            putHaltMessage_(Vector2i(bufPos.x, bufPos.y + yOffset), msg);

            mPrinter.flush();
        }

        redraw = false;

        TickTime inputTime;

        for (;;)
        {
            TickSpan delay = TickSpan::makeFromMilliSeconds(16);

            if (inputTime.diffToNow().toS64() < delay.toS64())
                continue;

            break;
        }

        nn::hid::CTR::PadStatus padStatus;

        if (!padReader.ReadLatest(&padStatus))
            continue;

        if (padStatus.hold & BUTTON_DOWN)
        {
            if (!redraw)
                break;

            putHaltMessage_(Vector2i(bufPos.x, bufPos.y + yOffset), msg);

            yOffset -= 8;
            redraw = true;
        }
        else if (padStatus.hold & BUTTON_UP)
        {
            if (yOffset + 8 <= 0)
            {
                putHaltMessage_(Vector2i(bufPos.x, bufPos.y + yOffset), msg);

                yOffset += 8;
                redraw = true;
            }
        }
    }
}

int ExceptionScreenCtr::putHaltMessage_(Vector2i const& pos, char const* msg)
{
    mPrinter.putString(pos, msg);
    
    f32 lastY = mPrinter.getLastCursorPos().y;
    return lastY < mPrinter.getBoundBox().getSizeY();
}

void ExceptionScreenCtr::clearMessage_(Vector2i const& bufSize, const char* msg)
{
    Color4u8 color = mPrinter.getCharColor();
    mPrinter.setCharColor(mPrinter.getBGColor());
    putHaltMessage_(bufSize, msg);
    mPrinter.setCharColor(color);
}
}
