#include "hostio/seadHostIOCurve.h"
#include <cmath>
#include "math/seadMathCalcCommon.h"
#include "math/seadMatrix.h"
#include "math/seadVector.h"
#include "math/seadMathNumbers.h"

namespace sead
{
namespace hostio
{
namespace 
{

static const sead::Matrix44f hermite(
     2.0f, -2.0f,  1.0f,  1.0f,
    -3.0f,  3.0f, -2.0f, -1.0f,
     0.0f,  0.0f,  1.0f,  0.0f,
     1.0f,  0.0f,  0.0f,  0.0f
);

Vector4f GetPositionOnCubic(const Vector4f& startPos, const Vector4f& startVel, const Vector4f& endPos, const Vector4f& endVel, f32 time)
{
    Matrix44f m1;
    Matrix44f m2;
    m1.setRow(0, Vector4f(startPos.x, startPos.y, startPos.z, 1.0f));
    m1.setRow(1, Vector4f(endPos.x, endPos.y, endPos.z, 1.0f));
    m1.setRow(2, Vector4f(startVel.x, startVel.y, startVel.z, 1.0f));
    m1.setRow(3, Vector4f(endVel.x, endVel.y, endVel.z, 1.0f));
    m2.setMul(hermite, m1);
    Vector4f timeVector(
        time * time * time,
        time * time,
        time,
        1.0f
    );
    return Vector4f(
        timeVector.dot(m2.getCol(0)),
        timeVector.dot(m2.getCol(1)),
        timeVector.dot(m2.getCol(2)),
        timeVector.dot(m2.getCol(3))
    );
}

// Rounded Nonuniform Spline
struct RNS
{
    static const s32 cMaxCtrlPoint = 64;

    struct splineData
    {
        Vector4f position;
        Vector4f velocity;
        f32 distance;
    };

    splineData node[cMaxCtrlPoint];
    f32 maxDistance;
    s32 nodeCount;

    RNS()
        : nodeCount(0)
    {
    }

    void addNode(const Vector4f& pos);

    Vector4f getStartVelocity(s32 index);
    Vector4f getEndVelocity(s32 index);
    void buildSpline();

    Vector4f getPosition(f32 time);
};

void RNS::addNode(const Vector4f& pos)
{
    if (nodeCount == 0)
        maxDistance = 0.0f;

    else
    {
        node[nodeCount - 1].distance = (node[nodeCount - 1].position - pos).length();
        maxDistance += node[nodeCount - 1].distance;
    }

    node[nodeCount++].position = pos;
}

Vector4f RNS::getStartVelocity(s32 index)
{
    Vector4f temp = 3.0f * (node[index + 1].position - node[index].position) / node[index].distance;
    return (temp - node[index + 1].velocity) * 0.5f;
}

Vector4f RNS::getEndVelocity(s32 index)
{
    Vector4f temp = 3.0f * (node[index].position - node[index - 1].position) / node[index - 1].distance;
    return (temp - node[index - 1].velocity) * 0.5f;
}

void RNS::buildSpline()
{
    for (s32 i = 1; i < nodeCount - 1; i++)
    {
        Vector4f v1 = node[i + 1].position - node[i].position;
        Vector4f v2 = node[i - 1].position - node[i].position;
        v1.normalize();
        v2.normalize();
        node[i].velocity = v1 - v2;
        node[i].velocity.normalize();
    }
    node[0].velocity = getStartVelocity(0);
    node[nodeCount - 1].velocity = getEndVelocity(nodeCount - 1);
}

Vector4f RNS::getPosition(f32 time)
{
    f32 distance = time * maxDistance;
    f32 currentDistance = 0.0f;
    s32 i = 0;
    while (currentDistance + node[i].distance < distance && i < nodeCount - 2)
    {
        currentDistance += node[i].distance;
        i++;
    }
    f32 t = (distance - currentDistance) / node[i].distance;
    Vector4f startVel = node[i].velocity * node[i].distance;
    Vector4f endVel = node[i+1].velocity * node[i].distance;
    return GetPositionOnCubic(
        node[i].position, startVel,
        node[i+1].position, endVel,
        t
    );
}

} // namespace

f32 (*sCurveFunctionTbl_f32[cNumCurveType])(
    f32, const CurveDataInfo*, const f32*) =
{
    curveLinear_<f32>,
    curveHermit_<f32>,
    curveStep_<f32>,
    curveSin_<f32>,
    curveCos_<f32>,
    curveSinPow2_<f32>,
    curveLinear2D_<f32>,
    curveHermit2D_<f32>,
    curveStep2D_<f32>,
    curveNonuniformSpline_<f32>,
};

f64 (*sCurveFunctionTbl_f64[cNumCurveType])(
    f32, const CurveDataInfo*, const f64*) =
{
    curveLinear_<f64>,
    curveHermit_<f64>,
    curveStep_<f64>,
    curveSin_<f64>,
    curveCos_<f64>,
    curveSinPow2_<f64>,
    curveLinear2D_<f64>,
    curveHermit2D_<f64>,
    curveStep2D_<f64>,
    curveNonuniformSpline_<f64>,
};

Vector2<f32> (*sCurveFunctionTbl_Vec2f[cNumCurveType])(
    f32, const CurveDataInfo*, const f32*) =
{
    curveLinearVec2_<f32>,
    curveHermitVec2_<f32>,
    curveStepVec2_<f32>,
    curveSinVec2_<f32>,
    curveCosVec2_<f32>,
    curveSinPow2Vec2_<f32>,
    curveLinear2DVec2_<f32>,
    curveHermit2DVec2_<f32>,
    curveStep2DVec2_<f32>,
    curveNonuniformSplineVec2_<f32>,
};

Vector2<f64> (*sCurveFunctionTbl_Vec2d[cNumCurveType])(
    f32, const CurveDataInfo*, const f64*) =
{
    curveLinearVec2_<f64>,
    curveHermitVec2_<f64>,
    curveStepVec2_<f64>,
    curveSinVec2_<f64>,
    curveCosVec2_<f64>,
    curveSinPow2Vec2_<f64>,
    curveLinear2DVec2_<f64>,
    curveHermit2DVec2_<f64>,
    curveStep2DVec2_<f64>,
    curveNonuniformSplineVec2_<f64>,
};

template <typename T>
static T fracPart(T x)
{
    return x - T(int(x));
}

template <typename T>
T curveLinear_(f32 t, const CurveDataInfo* info, const T* f)
{
    if (t < 0)
        return f[0];

    const u8 n = info->numUse - 1;
    const int i = n * t;
    if (i >= n)
        return f[n];
    return f[i] + (fracPart(n * t) * (f[i + 1] - f[i]));
}

// NON_MATCHING: instruction ordering
template <typename T>
T curveHermit_(f32 t, const CurveDataInfo* info, const T* f)
{
    if (info->numUse % 2 == 1)
        return 0;
    if (t < 0)
        return f[0];

    const u8 n = (info->numUse / 2) - 1;
    const int i = n * t;
    const int j = 2 * i;
    if (i >= n)
        return f[j];

    const f32 x = fracPart(n * t);
    const T* coeff = &f[j];

    return ((2 * x * x * x) - (3 * x * x) + 1) * coeff[0]  // (2t^3 - 3t^2 + 1)p0
           + ((-2 * x * x * x) + (3 * x * x)) * coeff[2]   // (-2t^3 + 2t^2)p1
           + ((x * x * x) - (x * x)) * coeff[3]            // (t^3 - t^2)m1
           + ((x * x * x) - (2 * x * x) + x) * f[j | 1]    // (t^3 - 2t^2 + t)m0
        ;
}

template <typename T>
T curveStep_(f32 t, const CurveDataInfo* info, const T* f)
{
    const f32 x = Mathf::clamp(t, 0.0, 1.0);
    return f[int(x * (info->numUse - 1))];
}

template <typename T>
T curveSin_(f32 t_, const CurveDataInfo*, const T* f)
{
    const T t = t_;
    return std::sin(f[0] * t * (2 * numbers::pi)) * f[1];
}

template <typename T>
T curveCos_(f32 t_, const CurveDataInfo*, const T* f)
{
    const T t = t_;
    return std::cos(f[0] * T(t) * (2 * numbers::pi)) * f[1];
}

template <typename T>
T curveSinPow2_(f32 t_, const CurveDataInfo*, const T* f)
{
    const T t = t_;
    const f32 y = std::sin(f[0] * t * (2 * numbers::pi));
    return y * y * f[1];
}

// NON_MATCHING: instruction reordering (which results in localized regalloc differences)
template <typename T>
T curveLinear2D_(f32 t_, const CurveDataInfo* info, const T* f)
{
    const T t = t_;
    if (f[0] >= t)
        return f[1];

    const u8 n = info->numUse / 2;
    if (f[2 * (n - 1)] <= t)
        return f[2 * (n - 1) + 1];

    for (s32 i = 0; i < n; ++i)
    {
        const s32 j = 2 * i;
        if (f[j + 2] > t)
            return f[j + 1] + ((t - f[j]) / (f[j + 2] - f[j])) * (f[j + 3] - f[j + 1]);
    }
    return 0;
}

// NON_MATCHING: same as curveHermit_<T>
template <typename T>
T curveHermit2D_(f32 t_, const CurveDataInfo* info, const T* f)
{
    const T t = t_;
    const s8 n = info->numUse / 3;
    if (f[0] >= t)
        return f[1];

    if (f[3 * (n - 1)] <= t)
        return f[3 * (n - 1) + 1];

    for (s32 i = 0; i < n; ++i)
    {
        const s32 j = 3 * i;
        if (f[j + 3] > t)
        {
            const T x = (t - f[j]) / (f[j + 3] - f[j]);
            return ((2 * x * x * x) - (3 * x * x) + 1) * f[j + 1]  // (2t^3 - 3t^2 + 1)p0
                   + ((-2 * x * x * x) + (3 * x * x)) * f[j + 4]   // (-2t^3 + 2t^2)p1
                   + ((x * x * x) - (x * x)) * f[j + 5]            // (t^3 - t^2)m1
                   + ((x * x * x) - (2 * x * x) + x) * f[j + 2]    // (t^3 - 2t^2 + t)m0
                ;
        }
    }

    return 0;
}

template <typename T>
T curveStep2D_(f32 t_, const CurveDataInfo* info, const T* f)
{
    const T t = t_;
    const s8 n = info->numUse / 2;
    if (t <= f[0])
        return f[1];

    if (t >= f[2 * (n - 1)])
        return f[2 * (n - 1) + 1];

    for (s32 i = 0; i < n; ++i)
    {
        if (t < f[2 * i + 2])
            return f[2 * i + 1];
    }
    return 0;
}

template <typename T>
T curveNonuniformSpline_(f32, const CurveDataInfo*, const T*)
{
    SEAD_ASSERT_MSG(false, "You must call ICurve::interpolateToVec2 at this curve type.");
    return 0;
}

template <typename T>
Vector2<T> curveLinearVec2_(f32 t, const CurveDataInfo* info, const T* f)
{
    return Vector2<T>(t, curveLinear_(t, info, f));
}

template <typename T>
Vector2<T> curveHermitVec2_(f32 t, const CurveDataInfo* info, const T* f)
{
    return Vector2<T>(t, curveHermit_(t, info, f));
}

template <typename T>
Vector2<T> curveStepVec2_(f32 t, const CurveDataInfo* info, const T* f)
{
    return Vector2<T>(t, curveStep_(t, info, f));
}

template <typename T>
Vector2<T> curveSinVec2_(f32 t, const CurveDataInfo* info, const T* f)
{
    return Vector2<T>(t, curveSin_(t, info, f));
}

template <typename T>
Vector2<T> curveCosVec2_(f32 t, const CurveDataInfo* info, const T* f)
{
    return Vector2<T>(t, curveCos_(t, info, f));
}

template <typename T>
Vector2<T> curveSinPow2Vec2_(f32 t, const CurveDataInfo* info, const T* f)
{
    return Vector2<T>(t, curveSinPow2_(t, info, f));
}

template <typename T>
Vector2<T> curveLinear2DVec2_(f32 t, const CurveDataInfo* info, const T* f)
{
    return Vector2<T>(t, curveLinear2D_(t, info, f));
}

template <typename T>
Vector2<T> curveHermit2DVec2_(f32 t, const CurveDataInfo* info, const T* f)
{
    return Vector2<T>(t, curveHermit2D_(t, info, f));
}

template <typename T>
Vector2<T> curveStep2DVec2_(f32 t, const CurveDataInfo* info, const T* f)
{
    return Vector2<T>(t, curveStep2D_(t, info, f));
}

// curveNonuniformSplineVec2_ has an assertion

template <typename T>
Vector2<T> curveHermit2DSmoothVec2_(f32 t, const CurveDataInfo* info, const T* f)
{
    return Vector2<T>(t, curveHermit2DSmooth_(t, info, f));
}

template <typename T>
inline Vector2<T> curveNonuniformSplineVec2_(f32 t, const CurveDataInfo* info, const T* buf)
{
    SEAD_ASSERT(info->numUse / 2 < RNS::cMaxCtrlPoint);

    RNS rns;
    Vector4f result;

    for (s32 i = 0; i < info->numUse / 2; i++)
        rns.addNode(Vector4f(buf[i * 2 + 0], buf[i * 2 + 1], 0.0f, 1.0f));

    rns.buildSpline();
    result = rns.getPosition(t);
    return Vector2<T>(result.x, result.y);
}
}  // namespace hostio
}  // namespace sead