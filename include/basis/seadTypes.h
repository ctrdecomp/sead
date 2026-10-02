#ifndef SEAD_TYPES_H_
#define SEAD_TYPES_H_

#include <seadVersion.h>
#include "basis/seadDefines.h"

#include <stddef.h>
#include <stdint.h>

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

typedef int8_t s8;
typedef int16_t s16;
typedef int32_t s32;
typedef int64_t s64;

typedef float f32;
typedef double f64;

#if defined(__cplusplus) && __cplusplus >= 201103L
typedef char16_t char16;
#else
typedef unsigned short char16;
#endif

typedef size_t size_t;

/* Maintain compatability with nullptr hack */

#ifndef nullptr
#define nullptr NULL
#endif

#endif  // SEAD_NEW_H_
