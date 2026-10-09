#pragma once

#include <basis/seadTypes.h>

namespace sead
{
class Base64
{
public:
    static void encode(char* dst, const void* src, size_t length, bool url_safe);
    static bool decode(void* dst, size_t dst_size, const char* src, size_t src_size,
                       size_t* decoded_size);

    static void encodeWithNullTerminate(char* dst, const void* src, int size, bool line_break)
    {
        int block_count = size / 3;

        if (size % 3 != 0)
            ++block_count;

        dst[block_count * 4] = '\0';

        Base64::encode(dst, src, size, line_break);
    }

};
}  // namespace sead

