#ifndef SEAD_ARENA_H_
#define SEAD_ARENA_H_

#include <basis/seadTypes.h>
#include <nn/os.h>

namespace sead
{
class Arena
{
public:
    Arena();
    ~Arena();

    u8* initialize(size_t size);
    void destroy();

    u8* mStart;
    size_t mSize;
    bool mInitWithStartAddress;

    static bool sIsUsingDeviceMemory;
};

}  // namespace sead

#endif  // SEAD_ARENA_H_

