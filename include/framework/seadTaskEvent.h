#pragma once

#include <prim/seadRuntimeTypeInfo.h>

namespace sead {

class TaskEvent
{
    SEAD_RTTI_BASE(TaskEvent);

public:
    enum Type
    {
        cPopped = 0,
        cTransitOut,
        cTransitIn,
        cTakeoverOut,
        cTakeoverIn,
        cCalcPaused,
        cCalcResumed,
        cDrawPaused,
        cDrawResumed,
        cOther,
        cNone
    };

public:
    TaskEvent(Type type): 
        mType(type)
    {
    }

protected:
    Type mType;
};

} // namespace sead
