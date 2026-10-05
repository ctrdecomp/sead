#pragma once

#include "basis/seadAssert.h"
#include "basis/seadTypes.h"
#include "math/seadVector.h"

namespace sead
{
namespace hostio
{
class ICurve
{
public:
    virtual f32 interpolateToF32(f32 t) = 0;
    virtual Vector2f interpolateToVec2f(f32 t) = 0;
};

enum CurveType
{
    cCurveLinear = 0,
    cCurveHermit,
    cCurveStep,
    cCurveSin,
    cCurveCos,
    cCurveSinPow2,
    cCurveLinear2D,
    cCurveHermit2D,
    cCurveStep2D,
    cCurveNonuniformSpline,
    cNumCurveType
};

struct CurveDataInfo
{
    u8 curveType;
    u8 _1;
    u8 numFloats;
    u8 numUse;
};

struct CurveData
{
    u32 numUse;
    u32 curveType;
    f32 f[30];
};

template <typename T>
class Curve : public ICurve
{
public:
    Curve()
    {
        mInfo.curveType = 0;
        mInfo.numFloats = 0;
        mInfo.numUse = 0;
        mInfo._1 = 4;
        mFloats = NULL;
    }

    virtual f32 interpolateToF32(f32 t);
    virtual Vector2f interpolateToVec2f(f32 t);

    CurveType getCurveType() const
    {
        return CurveType(mInfo.curveType);
    }

    void setData(CurveData* data, CurveType type, u32 num_floats, u32 num_use)
    {
        data->curveType = u32(type);
        data->numUse = num_use;
        setCurveType(type);
        setFloats(data, num_floats);
        setNumUse(num_use);
    }

    void setFloats(CurveData* data, u32 num_floats)
    {
        mInfo.numFloats = u8(num_floats);
        mFloats = data->f;
    }

    void setCurveType(CurveType type)
    {
        SEAD_ASSERT(type < cNumCurveType);
        mInfo.curveType = u8(type);
    }

    void setNumUse(u32 numUse)
    {
        SEAD_ASSERT(numUse <= 0xff);
        mInfo.numUse = u8(numUse);
    }

    f32* mFloats;
    CurveDataInfo mInfo;
};

template <typename T>
T curveLinear_(f32 t, const CurveDataInfo* info, const T* f);
template <typename T>
T curveHermit_(f32 t, const CurveDataInfo* info, const T* f);
template <typename T>
T curveStep_(f32 t, const CurveDataInfo* info, const T* f);
template <typename T>
T curveSin_(f32 t, const CurveDataInfo* info, const T* f);
template <typename T>
T curveCos_(f32 t, const CurveDataInfo* info, const T* f);
template <typename T>
T curveSinPow2_(f32 t, const CurveDataInfo* info, const T* f);
template <typename T>
T curveLinear2D_(f32 t, const CurveDataInfo* info, const T* f);
template <typename T>
T curveHermit2D_(f32 t, const CurveDataInfo* info, const T* f);
template <typename T>
T curveStep2D_(f32 t, const CurveDataInfo* info, const T* f);
template <typename T>
T curveNonuniformSpline_(f32 t, const CurveDataInfo* info, const T* f);
template <typename T>
T curveHermit2DSmooth_(f32 t, const CurveDataInfo* info, const T* f);

template <typename T>
Vector2<T> curveLinearVec2_(f32 t, const CurveDataInfo* info, const T* f);
template <typename T>
Vector2<T> curveHermitVec2_(f32 t, const CurveDataInfo* info, const T* f);
template <typename T>
Vector2<T> curveStepVec2_(f32 t, const CurveDataInfo* info, const T* f);
template <typename T>
Vector2<T> curveSinVec2_(f32 t, const CurveDataInfo* info, const T* f);
template <typename T>
Vector2<T> curveCosVec2_(f32 t, const CurveDataInfo* info, const T* f);
template <typename T>
Vector2<T> curveSinPow2Vec2_(f32 t, const CurveDataInfo* info, const T* f);
template <typename T>
Vector2<T> curveLinear2DVec2_(f32 t, const CurveDataInfo* info, const T* f);
template <typename T>
Vector2<T> curveHermit2DVec2_(f32 t, const CurveDataInfo* info, const T* f);
template <typename T>
Vector2<T> curveStep2DVec2_(f32 t, const CurveDataInfo* info, const T* f);
template <typename T>
Vector2<T> curveNonuniformSplineVec2_(f32 t, const CurveDataInfo* info, const T* f);
template <typename T>
Vector2<T> curveHermit2DSmoothVec2_(f32 t, const CurveDataInfo* info, const T* f);

extern f32 (*sCurveFunctionTbl_f32[cNumCurveType])(f32, const CurveDataInfo*, const f32*);
extern f64 (*sCurveFunctionTbl_f64[cNumCurveType])(f32, const CurveDataInfo*, const f64*);
extern Vector2<f32> (*sCurveFunctionTbl_Vec2f[cNumCurveType])(f32, const CurveDataInfo*, const f32*);
extern Vector2<f64> (*sCurveFunctionTbl_Vec2d[cNumCurveType])(f32, const CurveDataInfo*, const f64*);

template <>
inline f32 Curve<f32>::interpolateToF32(f32 t)
{
    return sCurveFunctionTbl_f32[mInfo.curveType](t, &mInfo, mFloats);
}

template <>
inline Vector2f Curve<f32>::interpolateToVec2f(f32 t)
{
    return sCurveFunctionTbl_Vec2f[mInfo.curveType](t, &mInfo, mFloats);
}

}  // namespace hostio
}  // namespace sead