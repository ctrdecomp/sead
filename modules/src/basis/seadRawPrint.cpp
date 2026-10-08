#include <basis/seadRawPrint.h>
#include "devenv/seadPrintConfig.h"

#include <stdio.h>

namespace sead 
{ 
namespace system 
{
void PrintString(const char* fmt, s32 len)
{
    PrintConfig::execCallbacks(PrintConfig::PrintEventArg(fmt, len));
}

void Print(const char* format, ...)
{
    va_list list;
    va_start(list, format);

    PrintV(format, list);

    va_end(list);
}

void PrintV(const char* format, va_list list)
{
    char buf[256];
    int len = vsnprintf(buf, sizeof(buf), format, list);

    if (256 < len) 
    {
        len = 256;
    }

    PrintString(buf, len);
}
} 
}
