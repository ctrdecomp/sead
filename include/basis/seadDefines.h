#pragma once

#if !defined(SEAD_DEBUG) && !defined(SEAD_RELEASE)
    #error "No build target defined"
#endif

#if !defined(SEAD_PLATFORM_CTR) && !defined(SEAD_PLATFORM_CTRWIN)
    #error "No plaform defined"
#endif

#if !defined(SEAD_USE_NW4C)
    #error "No graphics backend defined"
#endif

#if defined(NN_SWITCH_ENABLE_MEMBER_NAME_SHORTCUT)
    #define SEAD_USE_ANONYMOUS_STRUCT
#endif

#if defined(__clang__)
    #define SEAD_COMPILER_ARM_CLANG
#elif defined(__CC_ARM)
    #define SEAD_COMPILER_ARMCC
#elif defined(__GNUC__) || defined(__GNUG__)
    #define SEAD_COMPILER_GCC
#elif defined(_MSC_VER)
    #define SEAD_COMPILER_MSVC
#else
    #error "Unsupported compiler"
#endif // __clang__

#define SEAD_NO_COPY(CLASS)                  \
public:                                      \
    CLASS(const CLASS&){}                    \
    CLASS& operator=(const CLASS&){}

#define SEAD_UNUSED(VARIABLE) static_cast<void>(VARIABLE)
