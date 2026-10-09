// Filename: seadStereoCameraWrapper.cpp
//
// Project: StandardEAD C++ Library for CTR

#include "gfx/seadStereoCameraWrapper.h"

namespace sead
{
StereoCameraWrapper::StereoCameraWrapper():
    mProjL(),
    mProjR(),
    mCamL(),
    mCamR(),
    mCtrCamera()
{
}

StereoCameraWrapper::~StereoCameraWrapper()
{
}

void StereoCameraWrapper::inititalize()
{
    mCtrCamera.Initialize();
}

void StereoCameraWrapper::finalize()
{
    mCtrCamera.Finalize();
}

f32 StereoCameraWrapper::getLimitParallax() const
{
    return mCtrCamera.GetLimitParallax();
}

f32 StereoCameraWrapper::getMaxParallax() const
{
    return mCtrCamera.GetMaxParallax();
}

f32 StereoCameraWrapper::getParallax(f32 distance) const
{
    return mCtrCamera.GetParallax(distance);
}

f32 StereoCameraWrapper::getDistanceToLevel() const
{
    return mCtrCamera.GetDistanceToLevel();
}

f32 StereoCameraWrapper::getDistanceToNearClip() const
{
    return mCtrCamera.GetDistanceToNearClip();
}

f32 StereoCameraWrapper::getDistanceToFarClip() const
{
    return mCtrCamera.GetDistanceToFarClip();
}

void StereoCameraWrapper::setLimitParallax(f32 distance)
{
    mCtrCamera.SetLimitParallax(distance);
}

void StereoCameraWrapper::calculateMatrices(Projection const& projOri, Camera const& camOri, float depthLevel, float factor)
{
    StereoUtilCtr::calcStereoCamera(&mCtrCamera, &mProjL, &mCamL, &mProjR, &mCamR, projOri, camOri, depthLevel, factor, false);
}

void StereoCameraWrapper::calculateMatricesReal(Projection const& projOri, Camera const& camOri, float depthLevel, float factor)
{
    StereoUtilCtr::calcStereoCamera(&mCtrCamera, &mProjL, &mCamL, &mProjR, &mCamR, projOri, camOri, depthLevel, factor, true);
}
}
