#pragma once

#include <math/seadMathPolicies.h>

namespace sead {

// TODO
template <typename T>
class Matrix34CalcCommon
{
public:
    typedef typename Policies<T>::Mtx34Base Base;
    typedef typename Policies<T>::Mtx33Base Mtx33;

    typedef typename Policies<T>::QuatBase Quat;
    typedef typename Policies<T>::Vec3Base Vec3;

public:
    static void multiply(Base& o, const Base& a, const Base& b);
    static void multiply(Base& o, const Mtx33& a, const Base& b);
    static void multiply(Base& o, const Base& a, const Mtx33& b);

    static void makeQ(Base& o, const Quat& q);
    static void makeS(Base& o, const Vec3& s);
    static void makeSRT(Base& o, const Vec3& s, const Vec3& r, const Vec3& t);
    static void makeST(Base& o, const Vec3& s, const Vec3& t);

    static void setBase(Base& n, s32 axis, const Vec3& v);
    static void setTranslation(Base& n, const Vec3& v);
};

// TODO
template <typename T>
class Matrix44CalcCommon
{
public:
    typedef typename Policies<T>::Mtx44Base Base;
    typedef typename Policies<T>::Mtx33Base Mtx33;
    typedef typename Policies<T>::Mtx34Base Mtx34;

    typedef typename Policies<T>::Vec3Base Vec3;
    typedef typename Policies<T>::Vec4Base Vec4;

public:
    static void copy(Base& o, const Base& n);
    static void copy(Base& o, const Mtx33& n, const Vec3& t, const Vec4& v);
    static void copy(Base& o, const Mtx34& n, const Vec4& v);

    static void multiply(Base& o, const Base& a, const Base& b);
    static void multiply(Base& o, const Mtx34& a, const Base& b);
    static void multiply(Base& o, const Base& a, const Mtx34& b);

    static void getCol(Vec4* v, const Base& n, s32 axis);
    static void getRow(Vec4* v, const Base& n, s32 row);

    static void setCol(Base& n, s32 axis, const Vec4& v);
    static void setRow(Base& n, const Vec4& v, s32 row);
};

} // namespace sead

#include <math/seadMatrixCalcCommon.hpp>