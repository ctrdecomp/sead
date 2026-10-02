#pragma once

#include "controller/seadControllerAddon.h"

namespace sead
{
class PatternRumbleAddon : public ControllerAddon
{
    SEAD_RTTI_OVERRIDE(PatternRumbleAddon, ControllerAddon)

public:
    explicit PatternRumbleAddon(Controller* controller);
    virtual ~PatternRumbleAddon() { }

    virtual bool calc();

protected:
    virtual void startRumbleImpl_() = 0;
    virtual void stopRumbleImpl_() = 0;

public:
    bool isPatternEnable() const;
    void startPattern(const char* pattern, u32 duration);
    void stopPattern();

protected:
    const char* mPattern;
    u32 mPatternIdx;
    u32 mPatternDuration;
};

}  // namespace sead
