#include <math/seadVector.h>

namespace sead
{
template <>
const Vector2<int> Vector2<int>::zero(0, 0);

template <>
const Vector2<int> Vector2<int>::ex(1, 0);

template <>
const Vector2<int> Vector2<int>::ey(0, 1);

template <>
const Vector2<int> Vector2<int>::ones(1, 1);

template <>
const Vector2<f32> Vector2<f32>::zero(0.0f, 0.0f);

template <>
const Vector2<f32> Vector2<f32>::ex(1.0f, 0.0f);

template <>
const Vector2<f32> Vector2<f32>::ey(0.0f, 1.0f);

template <>
const Vector2<f32> Vector2<f32>::ones(1.0f, 1.0f);

template <>
const Vector3<f32> Vector3<f32>::zero(0.0f, 0.0f, 0.0f);

template <>
const Vector3<f32> Vector3<f32>::ex(1.0f, 0.0f, 0.0f);

template <>
const Vector3<f32> Vector3<f32>::ey(0.0f, 1.0f, 0.0f);

template <>
const Vector3<f32> Vector3<f32>::ez(0.0f, 0.0f, 1.0f);

template <>
const Vector3<f32> Vector3<f32>::ones(1.0f, 1.0f, 1.0f);

template <>
const Vector4<f32> Vector4<f32>::zero(0.0f, 0.0f, 0.0f, 0.0f);

template <>
const Vector4<f32> Vector4<f32>::ex(1.0f, 0.0f, 0.0f, 0.0f);

template <>
const Vector4<f32> Vector4<f32>::ey(0.0f, 1.0f, 0.0f, 0.0f);

template <>
const Vector4<f32> Vector4<f32>::ez(0.0f, 0.0f, 1.0f, 0.0f);

template <>
const Vector4<f32> Vector4<f32>::ew(0.0f, 0.0f, 0.0f, 1.0f);

template <>
const Vector4<f32> Vector4<f32>::ones(1.0f, 1.0f, 1.0f, 1.0f);
}