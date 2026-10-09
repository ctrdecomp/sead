#pragma once

#include "math/seadMatrix.h"
#include "math/seadVector.h"

namespace sead
{
class StereoUtil
{
public:
    static void getProjectionParam(float* p1, float* p2, float* p3, float* p4,float* p5,float* p6,const Matrix44<f32>& mtx);
};
}
