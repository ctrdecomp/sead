#pragma once

#include "framework/ctr_nw4c/seadGameFrameworkCtrNw4c.h"

namespace sead
{
class SeriesGameFrameworkCtrNw4c : public GameFrameworkCtrNw4c
{
    SEAD_RTTI_OVERRIDE(SeriesGameFrameworkCtrNw4c, GameFrameworkCtrNw4c)
public:
    static void initialize(const Framework::InitializeArg& arg);

    SeriesGameFrameworkCtrNw4c(const CreateArg& arg);
    virtual ~SeriesGameFrameworkCtrNw4c();
protected:
    virtual void mainLoop_();
    virtual void procFrame_();
};
}