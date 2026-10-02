#pragma once

// Forward declarations for Vector types.
// Most users should #include <seadVector.h> instead.

#include <basis/seadTypes.h>

namespace sead
{
template <typename T>
struct Vector2;

template <typename T>
struct Vector3;

template <typename T>
struct Vector4;

typedef Vector2<s32> Vector2i;
typedef Vector2<u32> Vector2u;
typedef Vector2<f32> Vector2f;

typedef Vector3<s32> Vector3i;
typedef Vector3<u32> Vector3u;
typedef Vector3<f32> Vector3f;

typedef Vector4<s32> Vector4i;
typedef Vector4<u32> Vector4u;
typedef Vector4<f32> Vector4f;

}  // namespace sead
