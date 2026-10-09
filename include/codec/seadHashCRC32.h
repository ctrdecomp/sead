#pragma once

#include <basis/seadTypes.h>
#include <prim/seadSafeString.h>

namespace sead
{
class HashCRC32
{
public:
    struct Context
    {
        Context():
            hash(-1)
        {
        }

        Context(u32 hash):
            hash(hash)
        {
        }
        
        u32 hash;
    };

    static u32 calcHash(const void* ptr, u32 size);

    static u32 calcStringHash(const char* str);
    static u32 calcStringHash(const SafeString& str) { return calcStringHash(str.cstr()); }

    static void initialize();

private:
    static u32 sTable[256];
    static bool sInitialized;
};
}  // namespace sead

