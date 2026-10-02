#pragma once

#include <utility>

#include "basis/seadNew.h"
#include "basis/seadTypes.h"

namespace sead
{
struct InitializeTag
{
};

struct ZeroInitializeTag
{
};

/// Provides suitably aligned uninitialized storage for a type T.
/// Use this as a std::aligned_storage replacement that is easier to use and less error prone for
/// common cases (std::aligned_storage_t<sizeof(T), alignof(T)>).
template <typename T, bool AutoDestruct = false>
class StorageFor
{
public:
    StorageFor()
    {
    }

    explicit StorageFor(InitializeTag) { constructDefault(); }

    explicit StorageFor(ZeroInitializeTag) { constructDefault(); }

    T* constructDefault() { return new (storage()) T; }

    template <typename A1>
    T* construct(const A1& a1)
    {
        return new (storage()) T(a1);
    }

    template <typename A1, typename A2>
    T* construct(const A1& a1, const A2& a2)
    {
        return new (storage()) T(a1, a2);
    }

    ~StorageFor()
    {
#if defined(__cplusplus) && __cplusplus >= 201103L
        if (AutoDestruct)
            destruct();
#else
        if (AutoDestruct)
            destruct();
#endif
    }

    /// @warning It is undefined behavior to call this if no object has been constructed.
    void destruct() { data()->~T(); }

    /// @warning It is undefined behavior to call this if no object has been constructed.
    T& ref() { return reinterpret_cast<T&>(mStorage); }

    /// @warning It is undefined behavior to call this if no object has been constructed.
    const T& ref() const { return reinterpret_cast<const T&>(mStorage); }

    /// @warning It is undefined behavior to call this if no object has been constructed.
    T* data() { return reinterpret_cast<T*>(mStorage); }

    /// @warning It is undefined behavior to call this if no object has been constructed.
    const T* data() const { return reinterpret_cast<const T*>(mStorage); }

    /// @warning It is undefined behavior to call this if no object has been constructed.
    T* operator->() { return data(); }
    /// @warning It is undefined behavior to call this if no object has been constructed.
    const T* operator->() const { return data(); }
    /// @warning It is undefined behavior to call this if no object has been constructed.
    T& operator*() { return ref(); }
    /// @warning It is undefined behavior to call this if no object has been constructed.
    const T& operator*() const { return ref(); }

    void* storage() { return mStorage; }
    const void* storage() const { return mStorage; }

private:
    union StorageUnion
    {
        u8 bytes[sizeof(T)];
        T value;
    } mStorage;
};
}  // namespace sead
