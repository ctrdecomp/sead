#include <time/seadTickTime.h>

namespace sead 
{

TickSpan TickTime::diffToNow() const
{
    return TickTime().diff(*this);
}

} // namespace sead