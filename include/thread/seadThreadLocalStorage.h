#pragma once

#include <nn/os.h>
#include "basis/seadTypes.h"

namespace sead
{
class ThreadLocalStorage
{
public:
    ThreadLocalStorage():
        mTlsSlot()
    {
    }
    ~ThreadLocalStorage(){}

    ThreadLocalStorage(const ThreadLocalStorage&){ };
    ThreadLocalStorage& operator=(const ThreadLocalStorage&){ };

    void setValue(uptr value);
    uptr getValue() const;

private:
    nn::os::ThreadLocalStorage mTlsSlot;
};
}  // namespace sead

#define SEAD_THREAD_THREAD_LOCAL_STORAGE_H_
#include "thread/ctr/seadThreadLocalStorageCtr.hpp"
#undef SEAD_THREAD_THREAD_LOCAL_STORAGE_H_
