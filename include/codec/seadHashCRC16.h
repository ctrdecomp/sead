#pragma once

#include <basis/seadTypes.h>
#include <prim/seadSafeString.h>

namespace sead
{
class HashCRC16
{
public:
    struct Context
    {
        Context():
            hash(0)
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
    static u16 sTable[256];
    static bool sInitialized;
};
}  // namespace sead

