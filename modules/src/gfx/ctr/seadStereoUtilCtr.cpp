// Filename: seadStereoUtilCtr.cpp
//
// Project: StandardEAD C++ Library for CTR

#include "gfx/ctr/seadStereoUtilCtr.h"
#include "gfx/seadStereoUtil.h"

// Rare LF27 Commentary upcoming

namespace sead
{
f32 StereoUtilCtr::calcStereoCamera(StereoCamera* ctrCam, DirectProjection* projL, DirectCamera* camL, 
    DirectProjection* projR, DirectCamera* camR, 
    Projection const& projOri, Camera const& camOri, 
    float depthLevel, float factor, bool doCalcReal)
{
    // Get projection param before start.
    float up, down, left, right, near, far;
    StereoUtil::getProjectionParam(&up, &down, &left, &right, &near, &far, projOri.getProjectionMatrix());

    // Setup CTR Cameras.
    ctrCam->SetBaseFrustum(left, right, down, up, near, far);
    ctrCam->SetBaseCamera(&camOri.getViewMatrix());

    Matrix44f projMtxL, projMtxR;
    Matrix34f viewMtxL, viewMtxR;
    Matrix44f projMtxOri;
    Matrix34f viewMtxOri;

    if(doCalcReal)
    {
        ctrCam->CalculateMatricesReal(&projMtxL, &viewMtxL, &projMtxR, &viewMtxR, depthLevel, factor);
    }
    else
    {
        ctrCam->CalculateMatrices(&projMtxL, &viewMtxL, &projMtxR, &viewMtxR, &projMtxOri, &viewMtxOri, depthLevel, factor);
    }

    // Plug in our MtxL & R into setting projection MTX
    projMtxL *= -1.0f;
    projL->setProjectionMatrix(projMtxL, Graphics::cDevicePosture_Same);

    projMtxR *= -1.0f;
    projR->setProjectionMatrix(projMtxR, Graphics::cDevicePosture_Same);

    camL->setViewMatrix(viewMtxL);
    camL->updateMatrix();

    camR->setViewMatrix(viewMtxR);
    camR->updateMatrix();

    // Returns current MaxParallax
    return ctrCam->GetMaxParallax();
}
}