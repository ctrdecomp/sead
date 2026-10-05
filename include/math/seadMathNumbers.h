#pragma once

namespace sead
{
namespace numbers
{

template <typename T>
struct MathNumbers
{
    static const T e;
    static const T log2e_v;
    static const T log10e;
    static const T pi_v;
    static const T inv_pi;
    static const T inv_sqrtpi;
    static const T ln2_v;
    static const T ln10;
    static const T sqrt2;
    static const T sqrt3;
    static const T inv_sqrt3;
    static const T egamma;
    static const T phi;
};

template <typename T>
const T MathNumbers<T>::e =
    static_cast<T>(2.718281828459045235360287471352662);

template <typename T>
const T MathNumbers<T>::log2e_v =
    static_cast<T>(1.442695040888963407359924918916605);

template <typename T>
const T MathNumbers<T>::log10e =
    static_cast<T>(0.434294481903251827651128918916605);

template <typename T>
const T MathNumbers<T>::pi_v =
    static_cast<T>(3.141592653589793238462643383279502);

template <typename T>
const T MathNumbers<T>::inv_pi =
    static_cast<T>(0.318309886183790671537767526745028);

template <typename T>
const T MathNumbers<T>::inv_sqrtpi =
    static_cast<T>(0.564189583547756286948079451560772);

template <typename T>
const T MathNumbers<T>::ln2_v =
    static_cast<T>(0.693147180559945309417232121458176);

template <typename T>
const T MathNumbers<T>::ln10 =
    static_cast<T>(2.302585092994045684017991454684364);

template <typename T>
const T MathNumbers<T>::sqrt2 =
    static_cast<T>(1.414213562373095048801688724209698);

template <typename T>
const T MathNumbers<T>::sqrt3 =
    static_cast<T>(1.732050807568877293527446341505872);

template <typename T>
const T MathNumbers<T>::inv_sqrt3 =
    static_cast<T>(0.577350269189625764509148780501957);

template <typename T>
const T MathNumbers<T>::egamma =
    static_cast<T>(0.577215664901532860606512090082402);

template <typename T>
const T MathNumbers<T>::phi =
    static_cast<T>(1.618033988749894848204586834365638);

extern const double e;
extern const double log2e;
extern const double log10e;
extern const double pi;
extern const double inv_pi;
extern const double inv_sqrtpi;
extern const double ln2;
extern const double ln10;
extern const double sqrt2;
extern const double sqrt3;
extern const double inv_sqrt3;
extern const double egamma;
extern const double phi;

}  // namespace numbers
}  // namespace sead