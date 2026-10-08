#pragma once

#include <time/seadTickSpan.h>
#include <nn/os.h>

namespace sead 
{
class TickTime
{
public:
    TickTime()
    {
        setNow();
    }

    void setNow()
    {
        mTick = nn::os::Tick::GetSystemCurrent();
    }

    TickSpan diff(const TickTime& t) const
    {
        return mTick - t.mTick;
    }

    TickSpan diffToNow() const;

    TickTime& operator+=(const TickSpan& rhs)
    {
        mTick += rhs.toS64();
        return *this;
    }

    TickTime& operator-=(const TickSpan& rhs)
    {
        mTick -= rhs.toS64();
        return *this;
    }

    u64 toU64() const
    {
        return mTick;
    }

private:
    u64 mTick;
};

inline TickSpan operator-(TickTime lhs, TickTime rhs)
{
    return lhs.diff(rhs);
}

inline TickTime operator+(TickTime time, TickSpan span)
{
    TickTime t = time;
    t += span;
    return t;
}

inline TickTime operator-(TickTime time, TickSpan span)
{
    TickTime t = time;
    t -= span;
    return t;
}

} // namespace sead
