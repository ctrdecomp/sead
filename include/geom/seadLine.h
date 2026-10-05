#pragma once

#include <math/seadVector.h>

namespace sead 
{

template <typename T>
class Ray
{
public:
    Ray();

    void setBy2Points(const T& p0, const T& p1);

    void setPos(const T& p);
    void setDir(const T& d, bool isNormalized);

    bool isValid() const;

private:
    T mP;
    T mD;
};

template <typename T>
inline void Ray<T>::setDir(const T& d, bool normalized)
{
    mD = d;

    if (!normalized)
        mD.normalize();
    else
    {
        SEAD_ASSERT_MSG(isValid(), "Ray.d must be normalized");
    }
}

template <typename T>
inline void Ray<T>::setPos(const T& pos)
{
    mP = pos;
}

template <typename T>
inline void Ray<T>::setBy2Points(const T& p1, const T& p2)
{
    setPos(p1);
    setDir(p2 - p1, false);
}

template <typename T>
inline bool Ray<T>::isValid() const
{
    return MathCalcCommon<f32>::equalsEpsilon(
        mD.squaredLength(),
        MathCalcCommon<f32>::one(),
        MathCalcCommon<f32>::epsilon() * 10
    );
}

typedef Ray<Vector2f> Ray2f;
typedef Ray<Vector3f> Ray3f;

} // namespace sead