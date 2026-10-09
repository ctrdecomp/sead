// Filename: seadMutexCtr.cpp
//
// Project: StandardEAD C++ Library for CTR

#include "thread/seadMutex.h"

namespace sead
{
Mutex::Mutex():
    IDisposer(), 
    mMutexInner(false)
{
}

Mutex::~Mutex()
{
}

bool Mutex::tryLock()
{
    mMutexInner.TryLock();
}

void Mutex::lock()
{
    mMutexInner.Lock();
}

void Mutex::unlock()
{
    mMutexInner.Unlock();
}
} // namespace sead
