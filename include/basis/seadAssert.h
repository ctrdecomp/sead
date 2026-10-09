#pragma once

#include <basis/seadTypes.h>
#include <basis/seadRawPrint.h>
#include <nn/gx.h>

#if defined(SEAD_DEBUG)
#define SEAD_ASSERT(condition)                                                                     \
    do                                                                                             \
    {                                                                                              \
        if (!(condition))                                                                          \
        {                                                                                          \
            sead::system::HaltWithDetailNoFormat(__FILE__, __LINE__, #condition);                  \
        }                                                                                          \
    } while (0)
#define SEAD_ASSERT_MSG(condition, format, ...)                                                    \
    do                                                                                             \
    {                                                                                              \
        if (!(condition))                                                                          \
        {                                                                                          \
            sead::system::HaltWithDetail(__FILE__, __LINE__, format, ##__VA_ARGS__);               \
        }                                                                                          \
    } while (0)
#define SEAD_ASSERT_GL()                                                                           \
    do                                                                                             \
    {                                                                                              \
        GLenum error = glGetError();                                                               \
        if (error != GL_NO_ERROR)                                                                  \
        {                                                                                          \
            sead::system::HaltWithDetail(__FILE__, __LINE__, "GL_ERROR 0x%x", error);              \
        }                                                                                          \
    } while (0)
#define SEAD_ASSERT_MIN_MSG(condition, format, ...)                                                    \
    do                                                                                             \
    {                                                                                              \
        if ((condition < 0))                                                                          \
        {                                                                                          \
            sead::system::HaltWithDetail(__FILE__, __LINE__, format, ##__VA_ARGS__);               \
        }                                                                                          \
    } while (0)
#else
#define SEAD_ASSERT(condition)                                                                     \
    do                                                                                             \
    {                                                                                              \
        if (false)                                                                                 \
        {                                                                                          \
            static_cast<void>(condition);                                                          \
        }                                                                                          \
    } while (0)
#define SEAD_ASSERT_MSG(condition, format, ...)                                                    \
    do                                                                                             \
    {                                                                                              \
        if (false)                                                                                 \
        {                                                                                          \
            static_cast<void>(condition);                                                          \
            sead::system::HaltWithDetail(nullptr, 0, format, ##__VA_ARGS__);                       \
        }                                                                                          \
    } while (0)
#define SEAD_ASSERT_GL()                                                                           \
    do                                                                                             \
    {                                                                                              \
        if (false)                                                                                 \
        {                                                                                          \
            sead::system::HaltWithDetail(nullptr, 0, nullptr);                                     \
        }                                                                                          \
    } while (0)
#define SEAD_ASSERT_MIN_MSG(condition, format, ...)                                                \
    do                                                                                             \
    {                                                                                              \
        if (false)                                                                                 \
        {                                                                                          \
            static_cast<void>(condition);                                                          \
            sead::system::HaltWithDetail(nullptr, 0, format, ##__VA_ARGS__);                       \
        }                                                                                          \
    } while (0)
#define SEAD_HALT() \
    sead::system::Halt(); \

#endif // SEAD_DEBUG

namespace sead 
{ 
namespace system 
{

void DebugBreak();
void Halt();
void HaltWithDetail(const char* pos, s32 line, const char* format, ...);
void HaltWithDetailNoFormat(const char* pos, s32 line, const char* str);

} // namespace system
} // namespace sead
