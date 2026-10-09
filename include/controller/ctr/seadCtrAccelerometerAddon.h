#pragma once

#include "controller/seadAccelerometerAddon.h"
#include <nn/hid.h>

namespace sead
{

class CtrAccelerometerAddon : public AccelerometerAddon
{
public:
    CtrAccelerometerAddon(Controller* controller);
    virtual ~CtrAccelerometerAddon();
    virtual bool calc();

    AccelerometerStatus mAccelerationStatus;
};
}
