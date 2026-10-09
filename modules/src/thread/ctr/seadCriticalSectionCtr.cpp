// Filename: seadCriticalSectionCtr.cpp
//
// Project: StandardEAD C++ Library for CTR

#include "thread/seadCriticalSection.h"

namespace sead
{
CriticalSection::CriticalSection():
    IDisposer()
{
}

CriticalSection::~CriticalSection()
{
}

void CriticalSection::lock()
{
    mCriticalSectionInner.Enter();
}

bool CriticalSection::tryLock()
{
    mCriticalSectionInner.TryEnter();
}

void CriticalSection::unlock()
{
    mCriticalSectionInner.Leave();
}
}// namespace sead
