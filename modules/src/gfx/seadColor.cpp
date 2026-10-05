#include <algorithm>
#include <cmath>

#include <gfx/seadColor.h>
#include <math/seadMathCalcCommon.h>
namespace sead
{
const Color4f Color4f::cBlack(0.0f, 0.0f, 0.0f, 1.0f);
const Color4f Color4f::cGray(0.5f, 0.5f, 0.5f, 1.0f);
const Color4f Color4f::cWhite(1.0f, 1.0f, 1.0f, 1.0f);
const Color4f Color4f::cRed(1.0f, 0.0f, 0.0f, 1.0f);
const Color4f Color4f::cGreen(0.0f, 1.0f, 0.0f, 1.0f);
const Color4f Color4f::cBlue(0.0f, 0.0f, 1.0f, 1.0f);
const Color4f Color4f::cYellow(1.0f, 1.0f, 0.0f, 1.0f);
const Color4f Color4f::cMagenta(1.0f, 0.0f, 1.0f, 1.0f);
const Color4f Color4f::cCyan(0.0f, 1.0f, 1.0f, 1.0f);
const f32 Color4f::cElementMax = 1.0f;
const f32 Color4f::cElementMin = 0.0f;

Color4f Color4f::lerp(const Color4f& color1, const Color4f& color2, f32 t)
{
    t = sead::Mathf::clamp(t, cElementMin, cElementMax);
    const f32 a = sead::lerp(color1.cl.a, color2.cl.a, t);
    const f32 r = sead::lerp(color1.cl.r, color2.cl.r, t);
    const f32 g = sead::lerp(color1.cl.g, color2.cl.g, t);
    const f32 b = sead::lerp(color1.cl.b, color2.cl.b, t);
    return Color4f(r, g, b, a);
}

void Color4f::setLerp(const Color4f& color1, const Color4f& color2, f32 t)
{
    t = sead::Mathf::clamp(t, cElementMin, cElementMax);
    cl.a = sead::lerp(color1.cl.a, color2.cl.a, t);
    cl.r = sead::lerp(color1.cl.r, color2.cl.r, t);
    cl.g = sead::lerp(color1.cl.g, color2.cl.g, t);
    cl.b = sead::lerp(color1.cl.b, color2.cl.b, t);
}

void Color4f::setGammaCollection(const Color4f& value, f32 gamma)
{
    cl.a = value.cl.a;
    cl.r = std::pow(value.cl.r, gamma);
    cl.g = std::pow(value.cl.g, gamma);
    cl.b = std::pow(value.cl.b, gamma);
}

void Color4f::adjustOverflow()
{
    cl.r = sead::Mathf::clamp(cl.r, cElementMin, cElementMax);
    cl.g = sead::Mathf::clamp(cl.g, cElementMin, cElementMax);
    cl.b = sead::Mathf::clamp(cl.b, cElementMin, cElementMax);
    cl.a = sead::Mathf::clamp(cl.a, cElementMin, cElementMax);
}

#define SEAD_COLOR4F_OPERATORS(OP, OP2)                                                            \
    Color4f& Color4f::operator OP(const Color4f& rhs)                                              \
    {                                                                                              \
        cl.r OP rhs.cl.r;                                                                                \
        cl.g OP rhs.cl.g;                                                                                \
        cl.b OP rhs.cl.b;                                                                                \
        cl.a OP rhs.cl.a;                                                                                \
        return *this;                                                                              \
    }                                                                                              \
    Color4f& Color4f::operator OP(f32 x)                                                           \
    {                                                                                              \
        cl.r OP x;                                                                                    \
        cl.g OP x;                                                                                    \
        cl.b OP x;                                                                                    \
        cl.a OP x;                                                                                    \
        return *this;                                                                              \
    }                                                                                              \
    Color4f operator OP2(const Color4f& lhs, const Color4f& rhs)                                   \
    {                                                                                              \
        Color4f result = lhs;                                                                      \
        result OP rhs;                                                                             \
        return result;                                                                             \
    }                                                                                              \
    Color4f operator OP2(const Color4f& lhs, f32 x)                                                \
    {                                                                                              \
        Color4f result = lhs;                                                                      \
        result OP x;                                                                               \
        return result;                                                                             \
    }

SEAD_COLOR4F_OPERATORS(+=, +)
SEAD_COLOR4F_OPERATORS(-=, -)
SEAD_COLOR4F_OPERATORS(*=, *)
SEAD_COLOR4F_OPERATORS(/=, /)

bool operator==(const Color4f& lhs, const Color4f& rhs)
{
    return lhs.cl.r == rhs.cl.r && lhs.cl.g == rhs.cl.g && lhs.cl.b == rhs.cl.b && lhs.cl.a == rhs.cl.a;
}

const Color4u8 Color4u8::cBlack(0, 0, 0, 255);
const Color4u8 Color4u8::cGray(128, 128, 128, 255);
const Color4u8 Color4u8::cWhite(255, 255, 255, 255);
const Color4u8 Color4u8::cRed(255, 0, 0, 255);
const Color4u8 Color4u8::cGreen(0, 255, 0, 255);
const Color4u8 Color4u8::cBlue(0, 0, 255, 255);
const Color4u8 Color4u8::cYellow(255, 255, 0, 255);
const Color4u8 Color4u8::cMagenta(255, 0, 255, 255);
const Color4u8 Color4u8::cCyan(0, 255, 255, 255);
const u8 Color4u8::cElementMax = 255;
const u8 Color4u8::cElementMin = 0;

// NON_MATCHING: but semantically equivalent (setLerp is matching after all)
Color4u8 Color4u8::lerp(const Color4u8& color1, const Color4u8& color2, f32 t)
{
    Color4u8 result = color1;
    result.setLerp(color1, color2, t);
    return result;
}

void Color4u8::setf(f32 fr, f32 fg, f32 fb, f32 fa)
{
    r = sead::Mathf::clamp(fr, 0.0f, 1.0f) * 255.0f;
    g = sead::Mathf::clamp(fg, 0.0f, 1.0f) * 255.0f;
    b = sead::Mathf::clamp(fb, 0.0f, 1.0f) * 255.0f;
    a = sead::Mathf::clamp(fa, 0.0f, 1.0f) * 255.0f;
}

void Color4u8::setLerp(const Color4u8& color1, const Color4u8& color2, f32 t)
{
    t = sead::Mathf::clamp(t, 0.0f, 1.0f);
    a = sead::lerp(color1.a, color2.a, t);
    r = sead::lerp(color1.r, color2.r, t);
    g = sead::lerp(color1.g, color2.g, t);
    b = sead::lerp(color1.b, color2.b, t);
}

void Color4u8::setGammaCollection(const Color4u8& value, f32 gamma)
{
    a = value.a;
    r = sead::Mathf::clamp(std::pow(f32(value.r) / 255.0f, gamma), 0.0f, 1.0f) * 255.0f;
    g = sead::Mathf::clamp(std::pow(f32(value.g) / 255.0f, gamma), 0.0f, 1.0f) * 255.0f;
    b = sead::Mathf::clamp(std::pow(f32(value.b) / 255.0f, gamma), 0.0f, 1.0f) * 255.0f;
}

#define SEAD_Color4u8_OPERATORS(OP, OP2)                                                           \
    Color4u8 operator OP2(const Color4u8& lhs, const Color4u8& rhs)                                \
    {                                                                                              \
        Color4u8 result = lhs;                                                                     \
        result OP rhs;                                                                             \
        return result;                                                                             \
    }                                                                                              \
    Color4u8 operator OP2(const Color4u8& lhs, u8 x)                                               \
    {                                                                                              \
        Color4u8 result = lhs;                                                                     \
        result OP x;                                                                               \
        return result;                                                                             \
    }

SEAD_Color4u8_OPERATORS(+=, +);
SEAD_Color4u8_OPERATORS(-=, -);
SEAD_Color4u8_OPERATORS(*=, *);
SEAD_Color4u8_OPERATORS(/=, /);
SEAD_Color4u8_OPERATORS(|=, |);
SEAD_Color4u8_OPERATORS(&=, &);


Color4u8& Color4u8::operator+=(const Color4u8& rhs)
{
    r = std::min<u32>(0xFF, u32(r) + rhs.r);
    g = std::min<u32>(0xFF, u32(g) + rhs.g);
    b = std::min<u32>(0xFF, u32(b) + rhs.b);
    a = std::min<u32>(0xFF, u32(a) + rhs.a);
    return *this;
}

Color4u8& Color4u8::operator-=(const Color4u8& rhs)
{
    r = r >= rhs.r ? r - rhs.r : 0;
    g = g >= rhs.g ? g - rhs.g : 0;
    b = b >= rhs.b ? b - rhs.b : 0;
    a = a >= rhs.a ? a - rhs.a : 0;
    return *this;
}

Color4u8& Color4u8::operator*=(const Color4u8& rhs)
{
    r = r * rhs.r / 0xFF;
    g = g * rhs.g / 0xFF;
    b = b * rhs.b / 0xFF;
    a = a * rhs.a / 0xFF;
    return *this;
}

Color4u8& Color4u8::operator/=(const Color4u8& rhs)
{
    r = rhs.r ? std::min<u32>(0xFF, 255 * u32(r) / u32(rhs.r)) : 255;
    g = rhs.g ? std::min<u32>(0xFF, 255 * u32(g) / u32(rhs.g)) : 255;
    b = rhs.b ? std::min<u32>(0xFF, 255 * u32(b) / u32(rhs.b)) : 255;
    a = rhs.a ? std::min<u32>(0xFF, 255 * u32(a) / u32(rhs.a)) : 255;
    return *this;
}

Color4u8& Color4u8::operator|=(const Color4u8& rhs)
{
    r |= rhs.r;
    g |= rhs.g;
    b |= rhs.b;
    a |= rhs.a;
    return *this;
}

Color4u8& Color4u8::operator&=(const Color4u8& rhs)
{
    r &= rhs.r;
    g &= rhs.g;
    b &= rhs.b;
    a &= rhs.a;
    return *this;
}

bool operator==(const Color4u8& lhs, const Color4u8& rhs)
{
    return lhs.r == rhs.r && lhs.g == rhs.g && lhs.b == rhs.b && lhs.a == rhs.a;
}

Color4u8& Color4u8::operator+=(u8 x)
{
    r = std::min<u32>(0xFF, x + u32(r));
    g = std::min<u32>(0xFF, x + u32(g));
    b = std::min<u32>(0xFF, x + u32(b));
    a = std::min<u32>(0xFF, x + u32(a));
    return *this;
}

Color4u8& Color4u8::operator-=(u8 x)
{
    r = r >= x ? r - x : 0;
    g = g >= x ? g - x : 0;
    b = b >= x ? b - x : 0;
    a = a >= x ? a - x : 0;
    return *this;
}

// NON_MATCHING: regalloc, one harmless reordering
Color4u8& Color4u8::operator*=(float x)
{
    r = std::max(0.0f, r * x);
    g = std::max(0.0f, g * x);
    b = std::max(0.0f, b * x);
    a = std::max(0.0f, a * x);
    return *this;
}

Color4u8& Color4u8::operator/=(float x)
{
    if (x == 0.0f)
    {
        r = 255;
        g = 255;
        b = 255;
        a = 255;
        return *this;
    }

    float q = float(r) / x;
    if (q < 0.0f)
        r = 0;
    else if (q > 255.0f)
        r = 255;
    else
        r = q;

    q = float(g) / x;
    if (q < 0.0f)
        g = 0;
    else if (q > 255.0f)
        g = 255;
    else
        g = q;

    q = float(b) / x;
    if (q < 0.0f)
        b = 0;
    else if (q > 255.0f)
        b = 255;
    else
        b = q;

    q = float(a) / x;
    if (q < 0.0f)
        a = 0;
    else if (q > 255.0f)
        a = 255;
    else
        a = q;

    return *this;
}

Color4u8& Color4u8::operator|=(u8 x)
{
    r |= x;
    g |= x;
    b |= x;
    a |= x;
    return *this;
}

Color4u8& Color4u8::operator&=(u8 x)
{
    r &= x;
    g &= x;
    b &= x;
    a &= x;
    return *this;
}
}  // namespace sead
