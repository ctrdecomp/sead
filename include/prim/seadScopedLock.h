#pragma once

#include <utility>

namespace sead
{
template <typename T>
class ScopedLock
{
public:
    explicit ScopedLock(T* lock) : mEngaged(true), mLocked(lock) { mLocked->lock(); }

private:
    ScopedLock(const ScopedLock& other);
    ScopedLock& operator=(const ScopedLock& other);

public:
    virtual ~ScopedLock()
    {
        if (mEngaged)
            mLocked->unlock();
    }

protected:
    bool mEngaged;
    T* mLocked;
};

template <typename T>
inline ScopedLock<T> makeScopedLock(T& lock)
{
    return ScopedLock<T>(&lock);
}

template <typename T>
class ConditionalScopedLock
{
public:
    ConditionalScopedLock(T* lock, bool do_lock) : mEngaged(true), mLocked(NULL)
    {
        if (!do_lock)
            return;
        mLocked = lock;
        mLocked->lock();
    }

    ConditionalScopedLock(const ConditionalScopedLock& other) : mEngaged(false), mLocked(NULL)
    {
    }
    ConditionalScopedLock& operator=(const ConditionalScopedLock& other)
    {
        return *this;
    }

    virtual ~ConditionalScopedLock()
    {
        if (mEngaged && mLocked)
            mLocked->unlock();
    }

protected:
    bool mEngaged;
    T* mLocked;
};

template <typename T>
inline ConditionalScopedLock<T> makeScopedLock(T& lock, bool do_lock)
{
    return ConditionalScopedLock<T>(&lock, do_lock);
}
}  // namespace sead
