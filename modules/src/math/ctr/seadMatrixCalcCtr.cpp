#pragma once

#include <nn/math.h>

#include <math/seadMathCalcCommon.h>
#include <math/ctr/seadMatrixCalcCtr.h>

namespace sead {

template <>
void Matrix34CalcCtr<f32>::multiply(Base& o, const Base& a, const Base& b)
{
    const f32 a11 = a.m[0][0];
    const f32 a12 = a.m[0][1];
    const f32 a13 = a.m[0][2];
    const f32 a14 = a.m[0][3];

    const f32 a21 = a.m[1][0];
    const f32 a22 = a.m[1][1];
    const f32 a23 = a.m[1][2];
    const f32 a24 = a.m[1][3];

    const f32 a31 = a.m[2][0];
    const f32 a32 = a.m[2][1];
    const f32 a33 = a.m[2][2];
    const f32 a34 = a.m[2][3];

    const f32 b11 = b.m[0][0];
    const f32 b12 = b.m[0][1];
    const f32 b13 = b.m[0][2];
    const f32 b14 = b.m[0][3];

    const f32 b21 = b.m[1][0];
    const f32 b22 = b.m[1][1];
    const f32 b23 = b.m[1][2];
    const f32 b24 = b.m[1][3];

    const f32 b31 = b.m[2][0];
    const f32 b32 = b.m[2][1];
    const f32 b33 = b.m[2][2];
    const f32 b34 = b.m[2][3];

    o.m[0][0] = a11 * b11 + a12 * b21 + a13 * b31;
    o.m[0][1] = a11 * b12 + a12 * b22 + a13 * b32;
    o.m[0][2] = a11 * b13 + a12 * b23 + a13 * b33;
    o.m[0][3] = a11 * b14 + a12 * b24 + a13 * b34 + a14;

    o.m[1][0] = a21 * b11 + a22 * b21 + a23 * b31;
    o.m[1][1] = a21 * b12 + a22 * b22 + a23 * b32;
    o.m[1][2] = a21 * b13 + a22 * b23 + a23 * b33;
    o.m[1][3] = a21 * b14 + a22 * b24 + a23 * b34 + a24;

    o.m[2][0] = a31 * b11 + a32 * b21 + a33 * b31;
    o.m[2][1] = a31 * b12 + a32 * b22 + a33 * b32;
    o.m[2][2] = a31 * b13 + a32 * b23 + a33 * b33;
    o.m[2][3] = a31 * b14 + a32 * b24 + a33 * b34 + a34;
}

template <>
void Matrix34CalcCtr<f32>::makeQ(Base& o, const Quat& q)
{
    // Assuming the quaternion "q" is normalized

    const f32 yy = 2 * q.y * q.y;
    const f32 zz = 2 * q.z * q.z;
    const f32 xx = 2 * q.x * q.x;
    const f32 xy = 2 * q.x * q.y;
    const f32 xz = 2 * q.x * q.z;
    const f32 yz = 2 * q.y * q.z;
    const f32 wz = 2 * q.w * q.z;
    const f32 wx = 2 * q.w * q.x;
    const f32 wy = 2 * q.w * q.y;

    o.m[0][0] = 1 - yy - zz;
    o.m[0][1] =     xy - wz;
    o.m[0][2] =     xz + wy;

    o.m[1][0] =     xy + wz;
    o.m[1][1] = 1 - xx - zz;
    o.m[1][2] =     yz - wx;

    o.m[2][0] =     xz - wy;
    o.m[2][1] =     yz + wx;
    o.m[2][2] = 1 - xx - yy;

    o.m[0][3] = 0;
    o.m[1][3] = 0;
    o.m[2][3] = 0;
}

template <>
void Matrix34CalcCtr<f32>::makeS(Base& o, const Vec3& s)
{
    o.m[0][0] = s.x;
    o.m[1][0] = 0;
    o.m[2][0] = 0;

    o.m[0][1] = 0;
    o.m[1][1] = s.y;
    o.m[2][1] = 0;

    o.m[0][2] = 0;
    o.m[1][2] = 0;
    o.m[2][2] = s.z;

    o.m[0][3] = 0;
    o.m[1][3] = 0;
    o.m[2][3] = 0;
}

template <>
void Matrix34CalcCtr<f32>::makeSRT(Base& o, const Vec3& s, const Vec3& r, const Vec3& t)
{
    const f32 sinV[3] = { MathCalcCommon<f32>::sin(r.x),
                        MathCalcCommon<f32>::sin(r.y),
                        MathCalcCommon<f32>::sin(r.z) };

    const f32 cosV[3] = { MathCalcCommon<f32>::cos(r.x),
                        MathCalcCommon<f32>::cos(r.y),
                        MathCalcCommon<f32>::cos(r.z) };

    o.m[0][0] = s.x * (cosV[1] * cosV[2]);
    o.m[1][0] = s.x * (cosV[1] * sinV[2]);
    o.m[2][0] = s.x * -sinV[1];

    o.m[0][1] = s.y * (sinV[0] * sinV[1] * cosV[2] - cosV[0] * sinV[2]);
    o.m[1][1] = s.y * (sinV[0] * sinV[1] * sinV[2] + cosV[0] * cosV[2]);
    o.m[2][1] = s.y * (sinV[0] * cosV[1]);

    o.m[0][2] = s.z * (cosV[0] * cosV[2] * sinV[1] + sinV[0] * sinV[2]);
    o.m[1][2] = s.z * (cosV[0] * sinV[2] * sinV[1] - sinV[0] * cosV[2]);
    o.m[2][2] = s.z * (cosV[0] * cosV[1]);

    o.m[0][3] = t.x;
    o.m[1][3] = t.y;
    o.m[2][3] = t.z;
}

template <>
void Matrix34CalcCtr<f32>::makeST(Base& o, const Vec3& s, const Vec3& t)
{
    o.m[0][0] = s.x;
    o.m[1][0] = 0;
    o.m[2][0] = 0;

    o.m[0][1] = 0;
    o.m[1][1] = s.y;
    o.m[2][1] = 0;

    o.m[0][2] = 0;
    o.m[1][2] = 0;
    o.m[2][2] = s.z;

    o.m[0][3] = t.x;
    o.m[1][3] = t.y;
    o.m[2][3] = t.z;
}

template <>
void Matrix44CalcCtr<f32>::copy(Base& o, const Mtx34& n, const Vec4& v)
{
    o.m[0][0] = n.m[0][0];
    o.m[0][1] = n.m[0][1];
    o.m[0][2] = n.m[0][2];
    o.m[0][3] = n.m[0][3];

    o.m[1][0] = n.m[1][0];
    o.m[1][1] = n.m[1][1];
    o.m[1][2] = n.m[1][2];
    o.m[1][3] = n.m[1][3];

    o.m[2][0] = n.m[2][0];
    o.m[2][1] = n.m[2][1];
    o.m[2][2] = n.m[2][2];
    o.m[2][3] = n.m[2][3];

    o.m[3][0] = v.x;
    o.m[3][1] = v.y;
    o.m[3][2] = v.z;
    o.m[3][3] = v.w;
}

template <>
void Matrix44CalcCtr<f32>::multiply(Base& o, const Base& a, const Mtx34& b)
{
    const f32 a11 = a.m[0][0];
    const f32 a12 = a.m[0][1];
    const f32 a13 = a.m[0][2];
    const f32 a14 = a.m[0][3];

    const f32 a21 = a.m[1][0];
    const f32 a22 = a.m[1][1];
    const f32 a23 = a.m[1][2];
    const f32 a24 = a.m[1][3];

    const f32 a31 = a.m[2][0];
    const f32 a32 = a.m[2][1];
    const f32 a33 = a.m[2][2];
    const f32 a34 = a.m[2][3];

    const f32 a41 = a.m[3][0];
    const f32 a42 = a.m[3][1];
    const f32 a43 = a.m[3][2];
    const f32 a44 = a.m[3][3];

    const f32 b11 = b.m[0][0];
    const f32 b12 = b.m[0][1];
    const f32 b13 = b.m[0][2];
    const f32 b14 = b.m[0][3];

    const f32 b21 = b.m[1][0];
    const f32 b22 = b.m[1][1];
    const f32 b23 = b.m[1][2];
    const f32 b24 = b.m[1][3];

    const f32 b31 = b.m[2][0];
    const f32 b32 = b.m[2][1];
    const f32 b33 = b.m[2][2];
    const f32 b34 = b.m[2][3];

    o.m[0][0] = a11 * b11 + a12 * b21 + a13 * b31;
    o.m[0][1] = a11 * b12 + a12 * b22 + a13 * b32;
    o.m[0][2] = a11 * b13 + a12 * b23 + a13 * b33;
    o.m[0][3] = a11 * b14 + a12 * b24 + a13 * b34 + a14;

    o.m[1][0] = a21 * b11 + a22 * b21 + a23 * b31;
    o.m[1][1] = a21 * b12 + a22 * b22 + a23 * b32;
    o.m[1][2] = a21 * b13 + a22 * b23 + a23 * b33;
    o.m[1][3] = a21 * b14 + a22 * b24 + a23 * b34 + a24;

    o.m[2][0] = a31 * b11 + a32 * b21 + a33 * b31;
    o.m[2][1] = a31 * b12 + a32 * b22 + a33 * b32;
    o.m[2][2] = a31 * b13 + a32 * b23 + a33 * b33;
    o.m[2][3] = a31 * b14 + a32 * b24 + a33 * b34 + a34;

    o.m[3][0] = a41 * b11 + a42 * b21 + a43 * b31;
    o.m[3][1] = a41 * b12 + a42 * b22 + a43 * b32;
    o.m[3][2] = a41 * b13 + a42 * b23 + a43 * b33;
    o.m[3][3] = a41 * b14 + a42 * b24 + a43 * b34 + a44;
}

template <>
void Matrix34CalcCtr<f32>::setBase(Base& n, s32 axis, const Vec3& v)
{
    n.m[0][axis] = v.x;
    n.m[1][axis] = v.y;
    n.m[2][axis] = v.z;
}

template <>
void Matrix34CalcCtr<f32>::setTranslation(Base& n, const Vec3& v)
{
    setBase(n, 3, v);
}

template <>
void Matrix44CalcCtr<f32>::copy(Base& n, const Base& o)
{
    MTX44Copy(&n, o);
}

template <>
void Matrix44CalcCtr<f32>::getRow(Vec4* v, const Base& n, s32 row)
{
    v->x = n.m[row][0];
    v->y = n.m[row][1];
    v->z = n.m[row][2];
    v->w = n.m[row][3];
}

template <>
void Matrix44CalcCtr<f32>::setRow(Base& n, const Vec4& v, s32 row)
{
    n.m[row][0] = v.x;
    n.m[row][1] = v.y;
    n.m[row][2] = v.z;
    n.m[row][3] = v.w;
}

template <>
void Matrix44CalcCtr<f32>::inverse(Base& o, const Base& n)
{
    MTX44Inverse(&o, n);
}

} // namespace sead
