#include "basis/seadRawPrint.h"

namespace sead
{
namespace system
{

bool sIsWarningEnable;

void Warning(const char* file, int line, const char* msg, ...)
{
    if(sIsWarningEnable)
    {
        va_list list;
        va_start(list, msg);

        Print("%s(%d) Warning: ", file, line);
        PrintV(msg, list);

        va_end(list);
    }
}

void SetWarningEnable(bool enable)
{
    sIsWarningEnable = enable;
}
} // namespace system
} // namespace sead