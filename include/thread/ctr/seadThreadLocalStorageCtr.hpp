#ifndef SEAD_THREAD_THREAD_LOCAL_STORAGE_H_
#include "thread/seadThreadLocalStorage.h"
#endif

#include "basis/seadRawPrint.h"

namespace sead
{
inline void ThreadLocalStorage::setValue(uptr value)
{
    mTlsSlot.SetValue(value);
}

inline uptr ThreadLocalStorage::getValue() const
{
    return mTlsSlot.GetValue();
}

}
