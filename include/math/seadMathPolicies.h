#pragma once

#include <math/seadMathBase.h>
#include <nn/math.h>

namespace sead
{

using namespace nn::math;

template <typename T>
class Policies {
public:
    typedef VEC2 Vec2Base;
    typedef VEC3 Vec3Base;
    typedef VEC4 Vec4Base;
    typedef QUAT QuatBase;
    typedef MTX22 Mtx22Base;
    typedef MTX33 Mtx33Base;
    typedef MTX34 Mtx34Base;
    typedef MTX44 Mtx44Base;
};

}  // namespace sead

