#include <prim/seadStringUtil.h>

#include <cstdio>
#include <cwchar>
#include <stdio.h>
#include <stdarg.h>

namespace sead
{
namespace StringUtil
{

template <typename T>
static bool tryParseImpl_(T* value, const SafeString& str, CardinalNumber cardinalNumber)
{
    // TODO
    SEAD_UNUSED(value);
    SEAD_UNUSED(str);
    SEAD_UNUSED(cardinalNumber);
    SEAD_ASSERT(false);
    return false;
}

static s32 compareChar16Pair_(const Char16Pair* p1, const Char16Pair* p2)
{
    return p1->before - p2->before;
}

char16 replace(char16 c, const Buffer<const Char16Pair>& sorted_table)
{
    if (sorted_table.size() == 0)
        return c;

    Char16Pair key(c, 0);

    const s32 idx = sorted_table.binarySearch(key, compareChar16Pair_);

    if (idx < 0)
        return c;

    return sorted_table[idx].after;
}

s32 vsw16printf(char16* s, size_t n, const char16* format, std::va_list args)
{
    return vswprintf(s, n, format, args);
}

s32 vsnprintf(char* s, size_t n, const char* format, va_list args)
{
    return ::vsnprintf(s, n, format, args);
}

bool tryParseU32(u32* out, const SafeString& str, CardinalNumber base)
{
    return tryParseImpl_<u32>(out, str, base);
}
}  // namespace StringUtil
}  // namespace sead