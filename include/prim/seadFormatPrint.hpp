#pragma once

#include <prim/seadFormatPrint.h>

namespace sead {

template <typename T>
void PrintFormatter::out(const T& obj, const char* option, PrintOutput* output)
{
    PrintFormatter::outSimpleObject_("%u", option, output, obj);
}

} // namespace sead