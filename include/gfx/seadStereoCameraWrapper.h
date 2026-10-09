#pragma once

#include "gfx/ctr/seadStereoUtilCtr.h"

namespace sead
{
class StereoCameraWrapper
{
public:
    StereoCameraWrapper();
    ~StereoCameraWrapper();
    void inititalize();
    void finalize();
    f32 getLimitParallax() const;
    f32 getMaxParallax() const;
    f32 getParallax(f32 distance) const;
    f32 getDistanceToLevel() const;
    f32 getDistanceToNearClip() const;
    f32 getDistanceToFarClip() const;
    void setLimitParallax(f32 distance);
    void calculateMatrices(Projection const& projOri, Camera const& camOri, float depthLevel, float factor);
    void calculateMatricesReal(Projection const& projOri, Camera const& camOri, float depthLevel, float factor);
protected:
    DirectProjection mProjL;
    DirectProjection mProjR;
    DirectCamera mCamL;
    DirectCamera mCamR;
    StereoCamera mCtrCamera;
};
}
