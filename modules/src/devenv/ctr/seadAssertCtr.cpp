// Filename: seadAssertCtr.cpp
//
// Project: StandardEAD C++ Library for CTR

#include "devenv/seadAssertConfig.h"
#include "devenv/ctr/seadExceptionScreenCtr.h"
#include "prim/seadSafeString.h"
#include "prim/seadMemUtil.h"

#include <stdio.h>
#include <nn/dbg.h>

namespace
{
u32 getStackPointer()
{
    return __current_sp();
}
} // namespace

namespace sead
{
namespace system
{

    
static char tmp[0x800];

void Halt()
{
    nn::dbg::Break();
}

void HaltWithDetail(const char* file, int lineNo, const char* fmt, ...)
{

    MemUtil::fillZero(&tmp, sizeof(tmp));

    s32 len = snprintf(
        tmp,
        sizeof(tmp),
        "\n//================= PROGRAM HALT ==================//\n"
        "Source File: %s\n"
        "Line Number: %d\n"
        "Description: %s",
        file,
        lineNo,
        fmt
    );
    if(len >= 0)
    {
        va_list args;
        va_start(args, fmt);

        s32 written = vsnprintf(tmp + len, sizeof(tmp) - len, fmt, args);

        va_end(args);

        if(written < 0)
        {
            len = -1;
        }
        else
        {
            len += written;

            s32 suffixLen = snprintf(tmp + len, sizeof(tmp) - len, "\n//=================================================//");

            if(suffixLen > 0)
            {
                len += suffixLen;

                if (len >= 0x7fe)
                {
                    len = 0x7ff;
                }
                else
                {
                    tmp[len]     = '\n';
                    tmp[len + 1] = '\0';
                    len += 1;
                }
            }
        }
    }
    tmp[0x7ff] = '\0';

    if (len < 0)
        len = strlen(tmp);

    PrintString(tmp, len);

    {
        BufferedSafeString backtraceBuf(tmp + len, static_cast<u32>(sizeof(tmp) - len));
        ExceptionScreenCtr::putBackTraceString(&backtraceBuf, getStackPointer());
    }

    AssertConfig::execCallbacks(tmp);
    Halt();
}

} // namespace system
} // namespace sead