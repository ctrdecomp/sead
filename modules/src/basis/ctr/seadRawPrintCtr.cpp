// Filename: seadRawPrintCtr.cpp
//
// Project: StandardEAD C++ Library for CTR

#include <nn/dbg.h>

namespace sead
{
namespace system
{
void PrintStringImpl(const char* fmt, s32 len)
{
    if(0 < len)
    {
        nn::dbg::detail::PutString(fmt);
    }
    return;
}


}
}