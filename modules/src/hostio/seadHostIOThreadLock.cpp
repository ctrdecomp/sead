#include "hostio/seadHostIOThreadLock.h"
#include "thread/seadCriticalSection.h"

namespace sead
{
namespace hostio
{
Atomic<u32> ThreadLock::sLockCnt;

ThreadLock::ThreadLock()
{
    lock();
}

ThreadLock::~ThreadLock()
{
    unlock();
}

void ThreadLock::lock()
{
    sLockCnt.increment();
    getCS().lock();
}

void ThreadLock::unlock()
{
    getCS().unlock();
    sLockCnt.decrement();
}

CriticalSection& ThreadLock::getCS()
{
    static CriticalSection sCS;
    return sCS;
}
}  // namespace hostio
}  // namespace sead

