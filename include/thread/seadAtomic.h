#pragma once

#include <basis/seadTypes.h>

namespace sead
{

struct AtomicDirectInitTag
{
};

template <class T>
struct AtomicBase
{
public:
    AtomicBase(T value = T())
    {
        storeNonAtomic(value);
    }

    AtomicBase(AtomicDirectInitTag, T value)
        : mValue(value)
    {
    }

    AtomicBase(const AtomicBase& rhs)
    {
        *this = rhs;
    }

    operator T() const
    {
        return load();
    }

    AtomicBase& operator=(const AtomicBase& rhs)
    {
        store(rhs.load());
        return *this;
    }

    AtomicBase& operator=(T value)
    {
        store(value);
        return *this;
    }

    T load() const
    {
        return mValue;
    }

    void store(T value)
    {
        exchange(value);
    }

    void storeNonAtomic(T value)
    {
        mValue = value;
    }

    T exchange(T value)
    {
        T oldValue;
        T result;

        do
        {
            oldValue = mValue;
            result = __strex(value, &mValue);
        }
        while (result != 0);

        return oldValue;
    }

    bool compareExchange(T expected, T desired, T* original = NULL)
    {
        T oldValue;

        do
        {
            oldValue = __ldrex(&mValue);

            if (oldValue != expected)
            {
                if (original != NULL)
                    *original = oldValue;

                return false;
            }
        }
        while (__strex(desired, &mValue) != 0);

        return true;
    }

protected:
    volatile T mValue;
};


template <class T>
struct Atomic : AtomicBase<T>
{
    Atomic()
        : AtomicBase<T>()
    {
    }

    explicit Atomic(T value)
        : AtomicBase<T>(value)
    {
    }

    Atomic(AtomicDirectInitTag tag, T value)
        : AtomicBase<T>(tag, value)
    {
    }

    Atomic(const Atomic& rhs)
        : AtomicBase<T>(rhs)
    {
    }

    Atomic& operator=(const Atomic& rhs)
    {
        AtomicBase<T>::operator=(rhs);
        return *this;
    }

    Atomic& operator=(T value)
    {
        AtomicBase<T>::operator=(value);
        return *this;
    }

    T fetchAdd(T x)
    {
        T oldValue;
        T newValue;

        do
        {
            oldValue = __ldrex(&this->mValue);
            newValue = oldValue + x;
        }
        while (__strex(newValue, &this->mValue) != 0);

        return oldValue;
    }

    T fetchSub(T x)
    {
        T oldValue;
        T newValue;

        do
        {
            oldValue = __ldrex(&this->mValue);
            newValue = oldValue - x;
        }
        while (__strex(newValue, &this->mValue) != 0);

        return oldValue;
    }

    T fetchAnd(T x)
    {
        T oldValue;
        T newValue;

        do
        {
            oldValue = __ldrex(&this->mValue);
            newValue = oldValue & x;
        }
        while (__strex(newValue, &this->mValue) != 0);

        return oldValue;
    }

    T fetchOr(T x)
    {
        T oldValue;
        T newValue;

        do
        {
            oldValue = __ldrex(&this->mValue);
            newValue = oldValue | x;
        }
        while (__strex(newValue, &this->mValue) != 0);

        return oldValue;
    }

    T fetchXor(T x)
    {
        T oldValue;
        T newValue;

        do
        {
            oldValue = __ldrex(&this->mValue);
            newValue = oldValue ^ x;
        }
        while (__strex(newValue, &this->mValue) != 0);

        return oldValue;
    }

    T increment()
    {
        return fetchAdd(1);
    }

    T decrement()
    {
        return fetchSub(1);
    }

    bool isBitOn(unsigned int bit) const
    {
        return (this->load() & (1 << bit)) != 0;
    }

    bool setBitOn(unsigned int bit)
    {
        T oldValue;
        T newValue;

        do
        {
            oldValue = __ldrex(&this->mValue);
            newValue = oldValue | (1 << bit);
        }
        while (__strex(newValue, &this->mValue) != 0);

        return (oldValue & (1 << bit)) == 0;
    }

    bool setBitOff(unsigned int bit)
    {
        T oldValue;
        T newValue;

        do
        {
            oldValue = __ldrex(&this->mValue);
            newValue = oldValue & ~(1 << bit);
        }
        while (__strex(newValue, &this->mValue) != 0);

        return (oldValue & (1 << bit)) != 0;
    }

    T getValue() const
    {
        return this->load();
    }

    void setValue(T value)
    {
        this->store(value);
    }

    T operator+=(T x)
    {
        return fetchAdd(x);
    }

    T operator-=(T x)
    {
        return fetchSub(x);
    }

    T operator&=(T x)
    {
        return fetchAnd(x);
    }

    T operator|=(T x)
    {
        return fetchOr(x);
    }

    T operator^=(T x)
    {
        return fetchXor(x);
    }

    T operator++()
    {
        return fetchAdd(1) + 1;
    }

    T operator++(int)
    {
        return fetchAdd(1);
    }

    T operator--()
    {
        return fetchSub(1) - 1;
    }

    T operator--(int)
    {
        return fetchSub(1);
    }
};

typedef Atomic<u32> AtomicU32;

template <class T>
struct Atomic<T*> : AtomicBase<T*>
{
    Atomic()
        : AtomicBase<T*>()
    {
    }

    explicit Atomic(T* value)
        : AtomicBase<T*>(value)
    {
    }

    Atomic(AtomicDirectInitTag tag, T* value)
        : AtomicBase<T*>(tag, value)
    {
    }

    Atomic(const Atomic& rhs)
        : AtomicBase<T*>(rhs)
    {
    }

    Atomic& operator=(const Atomic& rhs)
    {
        AtomicBase<T*>::operator=(rhs);
        return *this;
    }

    Atomic& operator=(T* value)
    {
        AtomicBase<T*>::operator=(value);
        return *this;
    }

    T& operator*() const
    {
        return *this->load();
    }

    T* operator->() const
    {
        return this->load();
    }
};

} // namespace sead