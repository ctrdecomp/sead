#pragma once

// A delegate is a wrapper around a pointer to member function or a lambda function.
//
// IDelegates: interface types.
// Delegates: implementations of delegates for pointer to member functions (only).
// LambdaDelegates: implementations of delegates for lambda functions (and more generally functors).
// AnyDelegates: type-erased type that can store any Delegate or LambdaDelegate.
//
// Unlike std::function or inplace_function, delegates cannot wrap functions with arbitrary
// signatures. Each category of delegate has several variants:
//
// - XDelegate (no argument, no return value),
// - XDelegateR (no argument),
// - XDelegate1 (1 argument, no return value)
// - XDelegate1R (1 argument)
// - XDelegate2 (2 argument, no return value)
// - XDelegate2R (2 arguments)
//
// To make a lambda delegate, use the makeLambdaDelegate functions, which will return a
// LambdaDelegate with most template arguments automatically deduced.

#include <basis/seadNew.h>

namespace sead
{
class Heap;

/// Interface of a delegate for a member function with no argument.
class IDelegate
{
public:
    virtual void invoke() = 0;
    virtual IDelegate* clone(Heap*) const { return nullptr; }

    void operator()() { return invoke(); }
};

/// @tparam R Return type
template <typename R>
class IDelegateR
{
public:
    virtual R invoke() = 0;
    virtual IDelegateR* clone(Heap*) const { return nullptr; }

    R operator()() { return invoke(); }
};

/// Interface of a delegate for a member function with one argument.
/// @tparam A1 Type of the argument
template <typename A1>
class IDelegate1
{
public:
    virtual void invoke(A1 a1) = 0;
    virtual IDelegate1* clone(Heap*) const { return nullptr; }

    void operator()(A1 a1) { return invoke(a1); }
};

/// @tparam A1 Type of the argument
/// @tparam R  Return type
template <typename A1, typename R>
class IDelegate1R
{
public:
    virtual R invoke(A1 a1) = 0;
    virtual IDelegate1R* clone(Heap*) const { return nullptr; }

    R operator()(A1 a1) { return invoke(a1); }
};

/// Interface of a delegate for a member function with two arguments.
/// @tparam A1 Type of the first argument
/// @tparam A2 Type of the second argument
template <typename A1, typename A2>
class IDelegate2
{
public:
    virtual void invoke(A1 a1, A2 a2) = 0;
    virtual IDelegate2* clone(Heap*) const { return nullptr; }

    void operator()(A1 a1, A2 a2) { return invoke(a1, a2); }
};

/// @tparam A1 Type of the first argument
/// @tparam A2 Type of the second argument
/// @tparam R  Return type
template <typename A1, typename A2, typename R>
class IDelegate2R
{
public:
    virtual R invoke(A1 a1, A2 a2) = 0;
    virtual IDelegate2R* clone(Heap*) const { return nullptr; }

    R operator()(A1 a1, A2 a2) { return invoke(a1, a2); }
};

/// Base class for delegate implementations.
template <typename T, typename PTMF, typename Base>
class DelegateBase : public Base
{
public:
    DelegateBase(){ }
    DelegateBase(T* instance, PTMF fn) : mInstance(instance), mFunctionPtr(fn) {}

    T* instance() const { return mInstance; }
    void setInstance(T* instance) { mInstance = instance; }
    void setFunction(PTMF fn) { mFunctionPtr = fn; }

    void bind(T* instance, PTMF fn)
    {
        setInstance(instance);
        setFunction(fn);
    }

protected:
    T* mInstance;
    PTMF mFunctionPtr;
};

/// Partial specialization of DelegateBase for regular function pointers
/// (*not* pointers-to-member-function).
template <typename FunctionPointer, typename Base>
class DelegateBase<void, FunctionPointer, Base> : public Base
{
public:
    DelegateBase(){ }
    explicit DelegateBase(FunctionPointer fn) : mFunctionPtr(fn) {}

    void setFunction(FunctionPointer fn) { mFunctionPtr = fn; }

protected:
    FunctionPointer mFunctionPtr;
};

/// Delegate for a member function with no argument.
/// @tparam T  Class type
template <typename T>
class Delegate : public DelegateBase<T, void (T::*)(), IDelegate>
{
public:
    typedef void (T::*PTMF)();
    typedef DelegateBase<T, PTMF, IDelegate> Base;

    Delegate() {}
    Delegate(T* instance, PTMF fn) : Base(instance, fn) {}

    void invoke() { operator()(); }
    void operator()() const
    {
        if (this->mInstance && this->mFunctionPtr)
            return (this->mInstance->*(this->mFunctionPtr))();
    }
    Delegate* clone(Heap* heap) const { return new (heap) Delegate(*this); }
};

/// @tparam T  Class type
/// @tparam R  Return type
template <typename T, typename R>
class DelegateR : public DelegateBase<T, R (T::*)(), IDelegateR<R> >
{
public:
    typedef DelegateBase<T, R (T::*)(), IDelegateR<R> > Base;

    DelegateR() {}
    DelegateR(T* instance, R (T::*fn)()) : Base(instance, fn) {}

    R invoke() { return operator()(); }
    R operator()() const
    {
        if (this->mInstance && this->mFunctionPtr)
            return (this->mInstance->*(this->mFunctionPtr))();
        return R();
    }
    DelegateR* clone(Heap* heap) const { return new (heap) DelegateR(*this); }
};

/// Delegate for a member function with one argument.
/// @tparam T  Class type
/// @tparam A1 Type of the argument
template <typename T, typename A1>
class Delegate1 : public DelegateBase<T, void (T::*)(A1), IDelegate1<A1> >
{
public:
    typedef DelegateBase<T, void (T::*)(A1), IDelegate1<A1> > Base;

    Delegate1() {}
    Delegate1(T* instance, void (T::*fn)(A1)) : Base(instance, fn) {}

    void invoke(A1 a1) { operator()(a1); }
    void operator()(A1 a1) const
    {
        if (this->mInstance && this->mFunctionPtr)
            return (this->mInstance->*(this->mFunctionPtr))(a1);
    }
    Delegate1* clone(Heap* heap) const { return new (heap) Delegate1(*this); }
};

/// @tparam T  Class type
/// @tparam A1 Type of the argument
/// @tparam R  Return type
template <typename T, typename A1, typename R>
class Delegate1R : public DelegateBase<T, R (T::*)(A1), IDelegate1R<A1, R> >
{
public:
    typedef DelegateBase<T, R (T::*)(A1), IDelegate1R<A1, R> > Base;

    Delegate1R() {}
    Delegate1R(T* instance, R (T::*fn)(A1)) : Base(instance, fn) {}

    R invoke(A1 a1) { return operator()(a1); }
    R operator()(A1 a1) const
    {
        if (this->mInstance && this->mFunctionPtr)
            return (this->mInstance->*(this->mFunctionPtr))(a1);
        return R();
    }
    Delegate1R* clone(Heap* heap) const { return new (heap) Delegate1R(*this); }
};

/// Delegate for a member function with two arguments.
/// @tparam T  Class type
/// @tparam A1 Type of the first argument
/// @tparam A2 Type of the second argument
template <typename T, typename A1, typename A2>
class Delegate2 : public DelegateBase<T, void (T::*)(A1, A2), IDelegate2<A1, A2> >
{
public:
    typedef DelegateBase<T, void (T::*)(A1, A2), IDelegate2<A1, A2> > Base;

    Delegate2() {}
    Delegate2(T* instance, void (T::*fn)(A1, A2)) : Base(instance, fn) {}

    void invoke(A1 a1, A2 a2) { return operator()(a1, a2); }
    void operator()(A1 a1, A2 a2) const
    {
        if (this->mInstance && this->mFunctionPtr)
            return (this->mInstance->*(this->mFunctionPtr))(a1, a2);
    }
    Delegate2* clone(Heap* heap) const { return new (heap) Delegate2(*this); }
};

/// @tparam T  Class type
/// @tparam A1 Type of the first argument
/// @tparam A2 Type of the second argument
/// @tparam R  Return type
template <typename T, typename A1, typename A2, typename R>
class Delegate2R : public DelegateBase<T, R (T::*)(A1, A2), IDelegate2R<A1, A2, R> >
{
public:
    typedef DelegateBase<T, R (T::*)(A1, A2), IDelegate2R<A1, A2, R> > Base;

    Delegate2R() {}
    Delegate2R(T* instance, R (T::*fn)(A1, A2)) : Base(instance, fn) {}

    R invoke(A1 a1, A2 a2) { return operator()(a1, a2); }
    R operator()(A1 a1, A2 a2) const
    {
        if (this->mInstance && this->mFunctionPtr)
            return (this->mInstance->*(this->mFunctionPtr))(a1, a2);
        return R();
    }
    Delegate2R* clone(Heap* heap) const { return new (heap) Delegate2R(*this); }
};

class DelegateFunc : public DelegateBase<void, void (*)(), IDelegate>
{
public:
    typedef DelegateBase<void, void (*)(), IDelegate> Base;

    DelegateFunc() {}
    explicit DelegateFunc(void (*fn)()) : Base(fn) {}

    void invoke() { operator()(); }
    void operator()() const
    {
        if (this->mFunctionPtr)
            return (*this->mFunctionPtr)();
    }
    DelegateFunc* clone(Heap* heap) const { return new (heap) DelegateFunc(*this); }
};

template <typename R>
class DelegateRFunc : public DelegateBase<void, R (*)(), IDelegateR<R> >
{
public:
    typedef DelegateBase<void, R (*)(), IDelegateR<R> > Base;

    DelegateRFunc() {}
    explicit DelegateRFunc(R (*fn)()) : Base(fn) {}

    R invoke() { return operator()(); }
    R operator()() const
    {
        if (this->mFunctionPtr)
            return (*this->mFunctionPtr)();
        return R();
    }
    DelegateRFunc* clone(Heap* heap) const { return new (heap) DelegateRFunc(*this); }
};

template <typename A1>
class Delegate1Func : public DelegateBase<void, void (*)(A1), IDelegate1<A1> >
{
public:
    typedef DelegateBase<void, void (*)(A1), IDelegate1<A1> > Base;

    Delegate1Func() {}
    explicit Delegate1Func(void (*fn)(A1)) : Base(fn) {}

    void invoke(A1 a1) { operator()(a1); }
    void operator()(A1 a1) const
    {
        if (this->mFunctionPtr)
            return (*this->mFunctionPtr)(a1);
    }
    Delegate1Func* clone(Heap* heap) const { return new (heap) Delegate1Func(*this); }
};

template <typename A1, typename R>
class Delegate1RFunc : public DelegateBase<void, R (*)(A1), IDelegate1R<A1, R> >
{
public:
    typedef DelegateBase<void, R (*)(A1), IDelegate1R<A1, R> > Base;

    Delegate1RFunc() {}
    explicit Delegate1RFunc(R (*fn)(A1)) : Base(fn) {}

    R invoke(A1 a1) { return operator()(a1); }
    R operator()(A1 a1) const
    {
        if (this->mFunctionPtr)
            return (*this->mFunctionPtr)(a1);
        return R();
    }
    Delegate1RFunc* clone(Heap* heap) const { return new (heap) Delegate1RFunc(*this); }
};

template <typename A1, typename A2>
class Delegate2Func : public DelegateBase<void, void (*)(A1, A2), IDelegate2<A1, A2> >
{
public:
    typedef DelegateBase<void, void (*)(A1, A2), IDelegate2<A1, A2> > Base;

    Delegate2Func() {}
    explicit Delegate2Func(void (*fn)(A1, A2)) : Base(fn) {}

    void invoke(A1 a1, A2 a2) { return operator()(a1, a2); }
    void operator()(A1 a1, A2 a2) const
    {
        if (this->mFunctionPtr)
            return (*this->mFunctionPtr)(a1, a2);
    }
    Delegate2Func* clone(Heap* heap) const { return new (heap) Delegate2Func(*this); }
};

template <typename A1, typename A2, typename R>
class Delegate2RFunc : public DelegateBase<void, R (*)(A1, A2), IDelegate2R<A1, A2, R> >
{
public:
    typedef DelegateBase<void, R (*)(A1, A2), IDelegate2R<A1, A2, R> > Base;

    Delegate2RFunc() {}
    explicit Delegate2RFunc(R (*fn)(A1, A2)) : Base(fn) {}

    R invoke(A1 a1, A2 a2) { return operator()(a1, a2); }
    R operator()(A1 a1, A2 a2) const
    {
        if (this->mFunctionPtr)
            return (*this->mFunctionPtr)(a1, a2);
        return R();
    }
    Delegate2RFunc* clone(Heap* heap) const { return new (heap) Delegate2RFunc(*this); }
};

#if defined(__cplusplus) && __cplusplus >= 201103L

template <typename Lambda>
class LambdaDelegate : public IDelegate
{
public:
    explicit LambdaDelegate(Lambda l) : mLambda(l) {}
    void invoke() { mLambda(); }
    void operator()() const { mLambda(); }
    LambdaDelegate* clone(Heap* heap) const { return new (heap) LambdaDelegate(*this); }

protected:
    Lambda mLambda;
};

template <typename Lambda, typename R>
class LambdaDelegateR : public IDelegateR<R>
{
public:
    explicit LambdaDelegateR(Lambda l) : mLambda(l) {}
    R invoke() { return mLambda(); }
    R operator()() const { return mLambda(); }
    LambdaDelegateR* clone(Heap* heap) const { return new (heap) LambdaDelegateR(*this); }

protected:
    Lambda mLambda;
};

template <typename Lambda, typename A1>
class LambdaDelegate1 : public IDelegate1<A1>
{
public:
    explicit LambdaDelegate1(Lambda l) : mLambda(l) {}
    void invoke(A1 a1) { mLambda(a1); }
    void operator()(A1 a1) const { mLambda(a1); }
    LambdaDelegate1* clone(Heap* heap) const { return new (heap) LambdaDelegate1(*this); }

protected:
    Lambda mLambda;
};

template <typename Lambda, typename A1, typename R>
class LambdaDelegate1R : public IDelegate1R<A1, R>
{
public:
    explicit LambdaDelegate1R(Lambda l) : mLambda(l) {}
    R invoke(A1 a1) { return mLambda(a1); }
    R operator()(A1 a1) const { return mLambda(a1); }
    LambdaDelegate1R* clone(Heap* heap) const
    {
        return new (heap) LambdaDelegate1R(*this);
    }

protected:
    Lambda mLambda;
};

template <typename Lambda, typename A1, typename A2>
class LambdaDelegate2 : public IDelegate2<A1, A2>
{
public:
    explicit LambdaDelegate2(Lambda l) : mLambda(l) {}
    void invoke(A1 a1, A2 a2) { mLambda(a1, a2); }
    void operator()(A1 a1, A2 a2) const { mLambda(a1, a2); }
    LambdaDelegate2* clone(Heap* heap) const { return new (heap) LambdaDelegate2(*this); }

protected:
    Lambda mLambda;
};

template <typename Lambda, typename A1, typename A2, typename R>
class LambdaDelegate2R : public IDelegate2R<A1, A2, R>
{
public:
    explicit LambdaDelegate2R(Lambda l) : mLambda(l) {}
    R invoke(A1 a1, A2 a2) { return mLambda(a1, a2); }
    R operator()(A1 a1, A2 a2) const { return mLambda(a1, a2); }
    LambdaDelegate2R* clone(Heap* heap) const
    {
        return new (heap) LambdaDelegate2R(*this);
    }

protected:
    Lambda mLambda;
};

// To work around the lack of CTAD.
template <typename Lambda>
static LambdaDelegate<Lambda> makeLambdaDelegate(const Lambda& l)
{
    return LambdaDelegate<Lambda>(l);
}

template <typename Lambda>
static LambdaDelegateR<Lambda, typename Lambda::result_type> makeLambdaDelegateR(const Lambda& l)
{
    return LambdaDelegateR<Lambda, typename Lambda::result_type>(l);
}

template <typename A1, typename Lambda>
static LambdaDelegate1<Lambda, A1> makeLambdaDelegate1(const Lambda& l)
{
    return LambdaDelegate1<Lambda, A1>(l);
}

template <typename A1, typename Lambda>
static LambdaDelegate1R<Lambda, A1, typename Lambda::result_type> makeLambdaDelegate1R(const Lambda& l)
{
    return LambdaDelegate1R<Lambda, A1, typename Lambda::result_type>(l);
}

template <typename A1, typename A2, typename Lambda>
static LambdaDelegate2<Lambda, A1, A2> makeLambdaDelegate2(const Lambda& l)
{
    return LambdaDelegate2<Lambda, A1, A2>(l);
}

template <typename A1, typename A2, typename Lambda>
static LambdaDelegate2R<Lambda, A1, A2, typename Lambda::result_type> makeLambdaDelegate2R(const Lambda& l)
{
    return LambdaDelegate2R<Lambda, A1, A2, typename Lambda::result_type>(l);
}

#endif

#if defined(__cplusplus) && __cplusplus >= 201103L

namespace detail
{
class DummyClassForDelegate
{
};

template <typename Interface, typename AnyClass, size_t StorageSize>
class AnyDelegateImpl
{
public:
    AnyDelegateImpl() { new (&mStorage) typename AnyClass::UnbindDummy(); }

    template <typename DelegateType>
    AnyDelegateImpl(DelegateType other)
    {
        *this = other;
    }

    template <typename DelegateType>
    AnyDelegateImpl& operator=(DelegateType other)
    {
        this->mStorage = other;
        return *this;
    }

    template <typename... Args>
    typename Interface::R operator()(Args&&... args)
    {
        return getDelegate()->invoke(args...);
    }

    template <typename... Args>
    typename Interface::R operator()(Args&&... args) const
    {
        return getDelegate()->invoke(args...);
    }

#if SEAD_DELEGATE_ISNODUMMY
    explicit operator bool() const { return getDelegate()->isNoDummy(); }
#endif

    Interface* getDelegate() { return reinterpret_cast<Interface*>(&mStorage); }
    const Interface* getDelegate() const { return reinterpret_cast<const Interface*>(&mStorage); }

protected:
    typedef Interface Interface_;
    struct StorageType
    {
        unsigned char bytes[StorageSize];
    } mStorage;
};
}  // namespace detail

/// A type-erased delegate that can store either a Delegate or a LambdaDelegate
/// without heap allocations.
class AnyDelegate : public detail::AnyDelegateImpl<IDelegate, AnyDelegate,
                                                   sizeof(Delegate<detail::DummyClassForDelegate>)>
{
public:
    typedef detail::AnyDelegateImpl<IDelegate, AnyDelegate,
                                  sizeof(Delegate<detail::DummyClassForDelegate>)> Base;

    class UnbindDummy : public Base::Interface_
    {
    public:
        void invoke() {}
#if SEAD_DELEGATE_ISNODUMMY
        bool isNoDummy() const { return false; }
#endif
    };

    template <typename T>
    void bind(T* obj, typename Delegate<T>::PTMF method)
    {
        this->mStorage = Delegate<T>(obj, method);
    }
};

template <typename R>
class AnyDelegateR
    : public detail::AnyDelegateImpl<IDelegateR<R>, AnyDelegateR<R>,
                                     sizeof(DelegateR<detail::DummyClassForDelegate, R>)>
{
public:
    typedef detail::AnyDelegateImpl<IDelegateR<R>, AnyDelegateR<R>,
                                   sizeof(DelegateR<detail::DummyClassForDelegate, R>)> Base;

    class UnbindDummy : public Base::Interface_
    {
    public:
        UnbindDummy() {}
        R invoke() { return R(); }
#if SEAD_DELEGATE_ISNODUMMY
        bool isNoDummy() const { return false; }
#endif
    private:
        s32 mUnk;
    };

    AnyDelegateR() : Base() {}
};

template <typename A1>
class AnyDelegate1
    : public detail::AnyDelegateImpl<IDelegate1<A1>, AnyDelegate1<A1>,
                                     sizeof(Delegate1<detail::DummyClassForDelegate, A1>)>
{
public:
    typedef detail::AnyDelegateImpl<IDelegate1<A1>, AnyDelegate1<A1>,
                                   sizeof(Delegate1<detail::DummyClassForDelegate, A1>)> Base;

    class UnbindDummy : public Base::Interface_
    {
    public:
        void invoke(A1) {}
#if SEAD_DELEGATE_ISNODUMMY
        bool isNoDummy() const { return false; }
#endif
    };
};

template <typename A1, typename R>
class AnyDelegate1R
    : public detail::AnyDelegateImpl<IDelegate1R<A1, R>, AnyDelegate1R<A1, R>,
                                     sizeof(Delegate1R<detail::DummyClassForDelegate, A1, R>)>
{
public:
    typedef detail::AnyDelegateImpl<IDelegate1R<A1, R>, AnyDelegate1R<A1, R>,
                                   sizeof(Delegate1R<detail::DummyClassForDelegate, A1, R>)> Base;

    class UnbindDummy : public Base::Interface_
    {
    public:
        R invoke(A1) { return R(); }
#if SEAD_DELEGATE_ISNODUMMY
        bool isNoDummy() const { return false; }
#endif
    };
};

template <typename A1, typename A2>
class AnyDelegate2
    : public detail::AnyDelegateImpl<IDelegate2<A1, A2>, AnyDelegate2<A1, A2>,
                                     sizeof(Delegate2<detail::DummyClassForDelegate, A1, A2>)>
{
public:
    typedef detail::AnyDelegateImpl<IDelegate2<A1, A2>, AnyDelegate2<A1, A2>,
                                   sizeof(Delegate2<detail::DummyClassForDelegate, A1, A2>)> Base;

    class UnbindDummy : public Base::Interface_
    {
    public:
        void invoke(A1, A2) {}
#if SEAD_DELEGATE_ISNODUMMY
        bool isNoDummy() const { return false; }
#endif
    };
};

template <typename A1, typename A2, typename R>
class AnyDelegate2R
    : public detail::AnyDelegateImpl<IDelegate2R<A1, A2, R>, AnyDelegate2R<A1, A2, R>,
                                     sizeof(Delegate2R<detail::DummyClassForDelegate, A1, A2, R>)>
{
public:
    typedef detail::AnyDelegateImpl<IDelegate2R<A1, A2, R>, AnyDelegate2R<A1, A2, R>,
                                   sizeof(Delegate2R<detail::DummyClassForDelegate, A1, A2, R>)> Base;

    class UnbindDummy : public Base::Interface_
    {
    public:
        R invoke(A1, A2) { return R(); }
#if SEAD_DELEGATE_ISNODUMMY
        bool isNoDummy() const { return false; }
#endif
    };
};

#endif

}  // namespace sead
