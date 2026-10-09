#pragma once

#include "gfx/seadStereoUtil.h"
#include "gfx/seadProjection.h"
#include "gfx/seadCamera.h"
#include <nn/ulcd.h>

namespace sead
{
class StereoUtilCtr
{
public:
    static f32 calcStereoCamera(StereoCamera* ctrCam, DirectProjection* projL, DirectCamera* camL, DirectProjection* projR, DirectCamera* camR, Projection const& projOri, Camera const& camOri, 
            float depthLevel, float factor, bool doCalcReal);
};
}
