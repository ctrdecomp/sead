#include <prim/seadStringUtil.h>

#include <cstdio>
#include <cwchar>
#include <stdio.h>
#include <stdarg.h>
#include <ctype.h>

namespace sead
{
namespace StringUtil
{
namespace
{
template <typename T>
T getMin_()
{
    return 0;
}

template <typename T>
T getMax_()
{
    return static_cast<T>(-1);
}

s32 checkAndConvertDecChar_(int c)
{
    if (c <= '9')
        return c - '0';

    return -1;
}

s32 checkAndConvertHexChar_(int c)
{
    c = tolower(c);

    if (c <= '9')
        return c - '0';

    if (c < 'a' || c > 'f')
        return -1;

    return c - 'a' + 10;
}

template <typename T>
bool tryParseNumberImpl_(T* result, SafeString::iterator begin, int radix)
{
    T value = 0;
    bool hasDigit = false;

    const T max = getMax_<T>();
    const T maxDigit = max % radix;

    while (true)
    {
        int c = *begin;
        int digit = checkAndConvertHexChar_(c);

        if (digit < 0 || digit >= radix)
        {
            if (!hasDigit)
                return false;

            if (result)
                *result = value;

            return true;
        }

        if (value > maxDigit)
            return false;

        value *= radix;

        if (max - digit < value)
            return false;

        value += digit;
        hasDigit = true;
        ++begin;
    }
}

template <typename T>
bool tryParseDecImpl_(T* result, SafeString::iterator begin, int radix)
{
    T value = 0;
    const T min = getMin_<T>();
    const T max = getMax_<T>();

    T minDigit = min / 10;
    T maxDigit = max / 10;

    while (true)
    {
        int digit = checkAndConvertDecChar_(*begin);

        if (digit < 0)
        {
            if (!value)
                return false;

            if (result)
                *result = value;

            return true;
        }

        if (value > minDigit || value >= maxDigit)
            return false;

        value = value * 10;

        if (radix < 0)
        {
            if (min + digit > value)
                return false;
        }
        else
        {
            if (max - digit < value)
                return false;
        }

        value += radix * digit;
        ++begin;
    }
}

template <typename T>
void tryParseSignImpl_(T* value, SafeString::iterator& itr)
{
    *value = 1;

    char sign = *itr;

    if (sign == '+')
    {
        ++itr;
    }
    else if (sign == '-')
    {
        *value = static_cast<T>(-1);
        ++itr;
    }
}

bool tryParsePrefixImpl_(int* radix, SafeString::iterator* it, SafeString str)
{
    char c = **it;

    if (c < '0' || c > '9')
        return false;

    if (c == '0')
    {
        u32 index = it->getIndex();
        char prefix = str.at(index + 1);

        if (prefix == 'x')
        {
            *radix = 16;
            ++*it;
            ++*it;
        }
        else if (prefix == 'b')
        {
            *radix = 2;
            ++*it;
            ++*it;
        }
        else
        {
            *radix = 8;
        }
    }
    else
    {
        *radix = 10;
    }

    return true;
}

template <typename T>
void tryParseUnderDecimalPointImpl_(T* result,
    SafeString::iterator& it, T value, T radix)
{
    T fraction = 0;
    T place = 1;

    while (true)
    {
        int digit = checkAndConvertDecChar_(*it);

        if (digit < 0)
        {
            if (place == 1)
                return;

            T value2 = fraction / place + value;

            if (result)
                *result = value2;

            return;
        }

        fraction *= 10;
        fraction += digit;

        ++it;

        place *= 10;
    }
}

template <typename T>
bool tryParseDecimalFractionImpl_(T* result, SafeString str, int radix)
{
    SafeString::iterator it = str.begin();

    T sign;
    tryParseSignImpl_(&sign, it);

    T value;

    if (!tryParseDecImpl_<T>(&value, it, radix))
        return false;

    if (*it != '.')
        return true;

    ++it;

    tryParseUnderDecimalPointImpl_<T>(&value, it, value, static_cast<T>(radix));

    if (result)
        *result = value * sign;

    return true;
}

template <typename T>
bool tryParseImpl_(T* result, SafeStringBase<char> str, int radix)
{
    SafeString::iterator it = str.begin();

    T sign;
    tryParseSignImpl_(&sign, it);

    if (radix <= 0)
    {
        if (!tryParsePrefixImpl_(&radix, &it, str))
            return false;
    }

    if (radix == 10)
    {
        T value;

        bool success = tryParseDecImpl_<T>(&value, it, radix);

        if (success)
            *result = value * sign;

        return success;
    }

    T value;

    if (!tryParseNumberImpl_<T>(&value, it, radix))
        return false;

    *result = value * sign;
    return true;
}
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

bool tryParseU8(u8* out, const SafeString& str, CardinalNumber base)
{
    return tryParseImpl_<u8>(out, str, base);
}

bool tryParseU16(u16* out, const SafeString& str, CardinalNumber base)
{
    return tryParseImpl_<u16>(out, str, base);
}

bool tryParseU32(u32* out, const SafeString& str, CardinalNumber base)
{
    return tryParseImpl_<u32>(out, str, base);
}

bool tryParseU64(u64* out, const SafeString& str, CardinalNumber base)
{
    return tryParseImpl_<u64>(out, str, base);
}

bool tryParseS8(s8* out, const SafeString& str, CardinalNumber base)
{
    return tryParseImpl_<s8>(out, str, base);
}

bool tryParseS16(s16* out, const SafeString& str, CardinalNumber base)
{
    return tryParseImpl_<s16>(out, str, base);
}

bool tryParseS32(s32* out, const SafeString& str, CardinalNumber base)
{
    return tryParseImpl_<s32>(out, str, base);
}

bool tryParseS64(s64* out, const SafeString& str, CardinalNumber base)
{
    return tryParseImpl_<s64>(out, str, base);
}

bool tryParseF32(f32* out, const SafeString& str, CardinalNumber base)
{
    return tryParseDecimalFractionImpl_<f32>(out, str, base);
}

bool tryParseF64(f64* out, const SafeString& str, CardinalNumber base)
{
    return tryParseDecimalFractionImpl_<f64>(out, str, base);
}
}  // namespace StringUtil
}  // namespace sead