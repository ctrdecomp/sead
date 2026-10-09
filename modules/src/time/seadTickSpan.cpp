#include <cstdlib>
#include <limits>

#include <nn/os.h>

#include <time/seadTickSpan.h>

namespace sead
{
    const s64 TickSpan::cFrequency = nn::os::Tick::GetSystemCurrent();

s64 TickSpan::toNanoSeconds() const
{
    s64 spanAbs = mSpan > 0 ? mSpan : -mSpan;

    if (spanAbs < LLONG_MAX / 1000 / 1000 / 1000)
        return mSpan * 1000 * 1000 * 1000 / cFrequency;
    else if (spanAbs < LLONG_MAX / 1000 / 1000)
        return mSpan * 1000 * 1000 / cFrequency * 1000;
    else if (spanAbs < LLONG_MAX / 1000)
        return mSpan * 1000 / cFrequency * 1000 * 1000;
    else
        return mSpan / cFrequency * 1000 * 1000 * 1000;
}

void TickSpan::setNanoSeconds(s64 nsec)
{
    s64 nsecAbs = nsec > 0 ? nsec : -nsec;

    if (nsecAbs <= LLONG_MAX / cFrequency)
        mSpan = cFrequency * nsec / 1000 / 1000 / 1000;
    else if (nsecAbs <= LLONG_MAX / cFrequency * 1000)
        mSpan = cFrequency * (nsec / 1000) / 1000 / 1000;
    else if (nsecAbs <= LLONG_MAX / cFrequency * 1000 * 1000)
        mSpan = cFrequency * (nsec / 1000 / 1000) / 1000;
    else
        mSpan = cFrequency * (nsec / 1000 / 1000 / 1000);
}
}  // namespace sead

