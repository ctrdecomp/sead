// Filename: seadStereoUtil.cpp
//
// Project: StandardEAD C++ Library for CTR

#include <gfx/seadStereoUtil.h>
#include <math/seadVector.h>

namespace
{
    sead::Vector2f cSrcSize;
}

namespace sead
{
void StereoUtil::getProjectionParam(float* p1, float* p2, float* p3, float* p4,float* p5,float* p6,const Matrix44<f32>& mtx)
{
    *p5 = mtx.f._23 / mtx.f._22;

    *p6 = mtx.f._22 * *p5;
    *p6 /= mtx.f._22 + 1.0f;

    *p4 = *p5 * (mtx.f._02 + 1.0f);
    *p4 /= mtx.f._00;

    *p3 = *p4 - *p5 * 2.0f / mtx.f._00;

    *p1 = *p5 * (mtx.f._12 + 1.0f);
    *p1 /= mtx.f._11;

    *p2 = *p1 - *p5 * 2.0f / mtx.f._11;
}
}