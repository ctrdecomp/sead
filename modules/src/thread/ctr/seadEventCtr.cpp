// Filename: seadEventCtr.cpp
//
// Project: StandardEAD C++ Library for CTR

#include "thread/seadEvent.h"

namespace sead
{
Event::Event(bool manual_reset):
    IDisposer(),
    mEventInner(manual_reset)
{
}

Event::Event():
    IDisposer(),
    mEventInner()
{
}

Event::~Event()
{
    mEventInner.Finalize();
}

void Event::initialize(bool manual_reset)
{
#ifdef SEAD_DEBUG
    SEAD_ASSERT_MSG(!mInitialized, "Event is already initialized.");
#endif
    mEventInner.Initialize(manual_reset);

    setInitialized(true);
}

void Event::wait()
{
#ifdef SEAD_DEBUG
    SEAD_ASSERT_MSG(!mInitialized, "Event is already initialized.");
#endif
    mEventInner.Wait();
}

bool Event::wait(TickSpan span)
{
#ifdef SEAD_DEBUG
    SEAD_ASSERT_MSG(mInitialized, "Event is not initialized.");
#endif
    mEventInner.Wait(nn::os::Tick(span.toS64()));
}

void Event::setSignal()
{
#ifdef SEAD_DEBUG
    SEAD_ASSERT_MSG(mInitialized, "Event is not initialized.");
#endif
    mEventInner.Signal();
}

void Event::resetSignal()
{
#ifdef SEAD_DEBUG
    SEAD_ASSERT_MSG(mInitialized, "Event is not initialized.");
#endif
    mEventInner.ClearSignal();
}
} // namespace sead