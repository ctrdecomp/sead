#pragma once

#include <basis/seadAssert.h>
#include <basis/seadTypes.h>
#include <container/seadSafeArray.h>
#include <prim/seadSafeString.h>
#include <prim/seadScopedLock.h>
#include <thread/seadCriticalSection.h>

namespace sead
{
class EnumUtil
{
public:
    static void parseText_(char** text_ptr, char* text_all, int size);

    static CriticalSection* getParseTextCS_();
    static CriticalSection* getInitValueArrayCS_();

    static const int countValues(const char* text_all, size_t text_all_len)
    {
        int count = 1;
        for (size_t i = 0; i < text_all_len; ++i)
        {
            if (text_all[i] == ',')
                ++count;
        }
        return count;
    }

private:
    static void skipToWordEnd_(char** p_ptr, char** p_next);
    static void skipToWordStart_(char** p_ptr);
};

}  // namespace sead

/// Define an enum class. Custom enumerator values are *not* supported.
///
/// Example:
///
/// SEAD_ENUM(CoreId, cMain, cSub1, cSub2)
///
#define SEAD_ENUM(NAME,...)                                                                  \
    class NAME                                                                               \
    {                                                                                        \
        struct CvRef                                                                         \
        {                                                                                    \
            CvRef(bool isCv, const volatile NAME& ref)                                       \
                : is_cv(isCv), cvref(ref)                                                     \
            {                                                                                \
            }                                                                                \
                                                                                             \
            const NAME& asRef() const                                                        \
            {                                                                                \
                return const_cast<const NAME&>(cvref);                                       \
            }                                                                                \
                                                                                             \
            bool is_cv;                                                                      \
            const volatile NAME& cvref;                                                      \
        };                                                                                   \
                                                                                             \
    public:                                                                                  \
        enum ValueType                                                                       \
        {                                                                                    \
            __VA_ARGS__                                                                      \
        };                                                                                   \
                                                                                             \
        NAME() : mIdx(0)                                                                     \
        {                                                                                    \
        }                                                                                    \
                                                                                             \
        NAME(ValueType value)                                                                \
        {                                                                                    \
            setRelativeIndex(value);                                                         \
        }                                                                                    \
                                                                                             \
        NAME(int idx)                                                                        \
        {                                                                                    \
            setRelativeIndex(idx);                                                           \
        }                                                                                    \
                                                                                             \
        NAME(const NAME& other) : mIdx(other.mIdx)                                           \
        {                                                                                    \
        }                                                                                    \
                                                                                             \
        NAME& operator=(const NAME& other)                                                   \
        {                                                                                    \
            mIdx = other.mIdx;                                                               \
            return *this;                                                                    \
        }                                                                                    \
                                                                                             \
        NAME& operator=(ValueType value)                                                     \
        {                                                                                    \
            setRelativeIndex(value);                                                         \
            return *this;                                                                    \
        }                                                                                    \
                                                                                             \
        volatile NAME& operator=(ValueType value) volatile                                   \
        {                                                                                    \
            setRelativeIndex(value);                                                         \
            return *this;                                                                    \
        }                                                                                    \
                                                                                             \
        volatile NAME& operator=(CvRef other) volatile                                       \
        {                                                                                    \
            setRelativeIndex(other.is_cv ? other.cvref.mIdx : other.asRef().mIdx);           \
            return *this;                                                                    \
        }                                                                                    \
                                                                                             \
        bool operator==(const NAME& rhs) const                                               \
        {                                                                                    \
            return mIdx == rhs.mIdx;                                                         \
        }                                                                                    \
                                                                                             \
        bool operator!=(const NAME& rhs) const                                               \
        {                                                                                    \
            return mIdx != rhs.mIdx;                                                         \
        }                                                                                    \
                                                                                             \
        bool operator==(ValueType value) const                                               \
        {                                                                                    \
            return ValueType(mIdx) == value;                                                 \
        }                                                                                    \
                                                                                             \
        bool operator==(ValueType value) const volatile                                      \
        {                                                                                    \
            return ValueType(mIdx) == value;                                                 \
        }                                                                                    \
                                                                                             \
        bool operator!=(ValueType value) const                                               \
        {                                                                                    \
            return ValueType(mIdx) != value;                                                 \
        }                                                                                    \
                                                                                             \
        bool operator!=(ValueType value) const volatile                                      \
        {                                                                                    \
            return ValueType(mIdx) != value;                                                 \
        }                                                                                    \
                                                                                             \
        ValueType value() const                                                              \
        {                                                                                    \
            return static_cast<ValueType>(mIdx);                                             \
        }                                                                                    \
                                                                                             \
        ValueType value() const volatile                                                     \
        {                                                                                    \
            return static_cast<ValueType>(mIdx);                                             \
        }                                                                                    \
                                                                                             \
        operator int() const volatile                                                        \
        {                                                                                    \
            return value();                                                                  \
        }                                                                                    \
                                                                                             \
        operator CvRef() const                                                               \
        {                                                                                    \
            return CvRef(false, *this);                                                      \
        }                                                                                    \
                                                                                             \
        operator CvRef() const volatile                                                       \
        {                                                                                    \
            return CvRef(true, *this);                                                       \
        }                                                                                    \
                                                                                                   \
        bool fromText(const sead::SafeString& name)                                                \
        {                                                                                          \
            for (int i = 0; i < size(); ++i)                                                       \
            {                                                                                      \
                if (name.isEqual(text(i)))                                                         \
                {                                                                                  \
                    mIdx = i;                                                                      \
                    return true;                                                                   \
                }                                                                                  \
            }                                                                                      \
            return false;                                                                          \
        }                                                                                          \
                                                                                                   \
        const char* text() const                                                                  \
        {                                                                                          \
            return text(mIdx);                                                                     \
        }                                                                                          \
                                                                                                   \
        const char* text() const volatile                                                         \
        {                                                                                          \
            return text(mIdx);                                                                     \
        }                                                                                          \
                                                                                                   \
        static const char* text(int idx)                                                          \
        {                                                                                          \
            return text_(idx);                                                                     \
        }                                                                                          \
                                                                                                   \
        int getRelativeIndex() const                                                              \
        {                                                                                          \
            return mIdx;                                                                           \
        }                                                                                          \
                                                                                                   \
        int getRelativeIndex() const volatile                                                     \
        {                                                                                          \
            return mIdx;                                                                           \
        }                                                                                          \
                                                                                                   \
        void setRelativeIndex(int idx)                                                            \
        {                                                                                          \
            SEAD_ASSERT_MSG(u32(idx) < u32(size()),                                                \
                            "range over: %d, [%d - %d)", idx, 0, size());                          \
            mIdx = idx;                                                                            \
        }                                                                                          \
                                                                                                   \
        void setRelativeIndex(int idx) volatile                                                   \
        {                                                                                          \
            SEAD_ASSERT_MSG(u32(idx) < u32(size()),                                                \
                            "range over: %d, [%d - %d)", idx, 0, size());                          \
            mIdx = idx;                                                                            \
        }                                                                                          \
                                                                                                   \
        const char* getTypeText() const                                                           \
        {                                                                                          \
            return #NAME;                                                                          \
        }                                                                                          \
                                                                                                   \
        const char* getTypeText() const volatile                                                  \
        {                                                                                          \
            return #NAME;                                                                          \
        }                                                                                          \
                                                                                                   \
        static int size()                                                                         \
        {                                                                                          \
            return cCount;                                                                         \
        }                                                                                          \
                                                                                                   \
        static int getSize()                                                                      \
        {                                                                                          \
            return size();                                                                         \
        }                                                                                          \
                                                                                                   \
        static int getLastIndex()                                                                 \
        {                                                                                          \
            return size() - 1;                                                                     \
        }                                                                                          \
                                                                                                   \
        static void initialize()                                                                  \
        {                                                                                          \
            text(0);                                                                               \
        }                                                                                          \
                                                                                                   \
        class iterator                                                                             \
        {                                                                                          \
        public:                                                                                    \
            explicit iterator(int idx) : mIdx(idx)                                                \
            {                                                                                      \
            }                                                                                      \
                                                                                                   \
            bool operator==(const iterator& rhs) const                                            \
            {                                                                                      \
                return mIdx == rhs.mIdx;                                                           \
            }                                                                                      \
                                                                                                   \
            bool operator!=(const iterator& rhs) const                                            \
            {                                                                                      \
                return mIdx != rhs.mIdx;                                                           \
            }                                                                                      \
                                                                                                   \
            iterator& operator++()                                                                \
            {                                                                                      \
                if (mIdx <= getLastIndex())                                                       \
                {                                                                                  \
                    ++mIdx;                                                                       \
                }                                                                                  \
                else                                                                               \
                {                                                                                  \
                    SEAD_ASSERT_MSG(false, "enum iterator overflow");                             \
                    mIdx = size();                                                                 \
                }                                                                                  \
                return *this;                                                                      \
            }                                                                                      \
                                                                                                   \
            iterator& operator--()                                                                \
            {                                                                                      \
                --mIdx;                                                                            \
                return *this;                                                                      \
            }                                                                                      \
                                                                                                   \
            NAME operator*() const                                                                \
            {                                                                                      \
                return NAME(mIdx);                                                                 \
            }                                                                                      \
                                                                                                   \
        private:                                                                                   \
            int mIdx;                                                                              \
        };                                                                                         \
                                                                                                   \
        static iterator begin()                                                                   \
        {                                                                                          \
            return iterator(0);                                                                    \
        }                                                                                          \
                                                                                                   \
        static iterator end()                                                                     \
        {                                                                                          \
            return iterator(size());                                                               \
        }                                                                                          \
                                                                                                   \
    private:                                                                                       \
        static const char* text_(int idx)                                                         \
        {                                                                                          \
            if (u32(idx) >= u32(cCount))                                                          \
                return NULL;                                                                       \
                                                                                                   \
            static char** spTextPtr = NULL;                                                       \
                                                                                                   \
            if (spTextPtr)                                                                         \
                return spTextPtr[idx];                                                             \
                                                                                                   \
            {                                                                                      \
                sead::ScopedLock<sead::CriticalSection>                                            \
                    lock(sead::EnumUtil::getParseTextCS_());                                      \
                                                                                                   \
                if (!spTextPtr)                                                                    \
                {                                                                                  \
                    static char* sTextPtr[cCount];                                                 \
                    static sead::FixedSafeString<cTextAllLen> sTextAll =                           \
                        sead::SafeString(cTextAll);                                                \
                                                                                                   \
                    sead::EnumUtil::parseText_(                                                   \
                        sTextPtr, sTextAll.getBuffer(), cCount);                                   \
                                                                                                   \
                    spTextPtr = sTextPtr;                                                         \
                }                                                                                  \
            }                                                                                      \
                                                                                                   \
            return spTextPtr[idx];                                                                 \
        }                                                                                          \
                                                                                                   \
        static const char* cTextAll;                                                              \
        static const size_t cTextAllLen;                                                          \
        static const int cCount = sead::EnumUtil::countValues(cTextAll, cTextAllLen);   \
                                                                                                   \
        int mIdx;                                                                                  \
    };