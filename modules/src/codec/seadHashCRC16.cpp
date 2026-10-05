#include <codec/seadHashCRC16.h>

namespace sead
{
u16 HashCRC16::sTable[256];
bool HashCRC16::sInitialized = false;

void HashCRC16::initialize()
{
    for (u32 i = 0; i < sizeof(sTable) / sizeof(sTable[0]); ++i)
    {
        u32 val = i;
        for (int j = 0; j < 8; ++j)
            val = ((val & 1) == 0) ? (val >> 1) : ((val >> 1) ^ 0xA001);
        sTable[i] = val;
    }
    sInitialized = true;
}

u32 HashCRC16::calcHash(const void* ptr, u32 size)
{
    if (!sInitialized)
        initialize();

    u32 hash = 0;
    const u8* data = static_cast<const u8*>(ptr);
    for (u32 i = 0; i < size; i++)
        hash = sTable[data[i] ^ (hash & 0xFF)] ^ (hash >> 8);
    return hash;
}

u32 HashCRC16::calcStringHash(const char* str)
{
    if (!sInitialized)
        initialize();

    u32 hash = 0;
    while (*str)
        hash = sTable[*str++ ^ (hash & 0xFF)] ^ (hash >> 8);
    return hash;
}

}  // namespace sead
