#pragma once

#include "basis/seadTypes.h"
#include "heap/seadDisposer.h"
#ifdef SEAD_DEBUG
#include "prim/seadStorageFor.h"
#endif

namespace sead
{
namespace hostio
{
class NodeEvent;
class PropertyEvent;

// These classes always exist (even in release builds, as can be seen in SMO's RTTI) but
// their functions are only defined in debug or develop builds.

class LifeCheckable
{
#ifdef SEAD_DEBUG
public:
    LifeCheckable()
        : mCreateID(0)
        , mPrev(NULL)
        , mNext(NULL)
        , mDisposer(NULL)
    {
        mDisposer = mDisposerBuf.construct(this);
        initialize_();
    }

    LifeCheckable(Heap* disposer_heap, IDisposer::HeapNullOption option)
        : mCreateID(0)
        , mPrev(NULL)
        , mNext(NULL)
        , mDisposer(NULL)
    {
        mDisposer = mDisposerBuf.construct(this, disposer_heap, option);
        initialize_();
    }

    virtual ~LifeCheckable()
    {
        DisposeHostIOCaller* disposer = mDisposer;
        if (disposer && disposer->hasInstance())
        {
            mDisposer->clearInstance();
            mDisposer->~DisposeHostIOCaller();
            disposeHostIOImpl_();
        }

        mDisposer = NULL;
    }

    u32 getCreateID() const { return mCreateID; }

    static LifeCheckable* searchInstanceFromCreateID(u32 createID);

    LifeCheckable(const LifeCheckable&);
    LifeCheckable& operator=(const LifeCheckable&);

protected:
    virtual void disposeHostIO() { disposeHostIOImpl_(); }

private:
    class DisposeHostIOCaller : public IDisposer
    {
    public:
        explicit DisposeHostIOCaller(LifeCheckable* instance)
            : mInstance(instance)
        {
        }

        DisposeHostIOCaller(LifeCheckable* instance,
                            Heap* disposer_heap,
                            HeapNullOption option)
            : IDisposer(disposer_heap, option)
            , mInstance(instance)
        {
        }

        virtual ~DisposeHostIOCaller();

        bool hasInstance() const
        {
            return mInstance != NULL;
        }

        void clearInstance()
        {
            mInstance = NULL;
        }

    private:
        LifeCheckable* mInstance;
    };

    void initialize_();
    void disposeHostIOImpl_();

    u32 mCreateID;
    LifeCheckable* mPrev;
    LifeCheckable* mNext;
    DisposeHostIOCaller* mDisposer;
    StorageFor<DisposeHostIOCaller> mDisposerBuf;

    static u32 sCurrentCreateID;
    static LifeCheckable* sTopInstance;
#endif
};


class PropertyEventListener : public LifeCheckable
{
#ifdef SEAD_DEBUG
public:
    PropertyEventListener()
        : LifeCheckable()
    {
    }

    PropertyEventListener(Heap* disposer_heap,
                          IDisposer::HeapNullOption option)
        : LifeCheckable(disposer_heap, option)
    {
    }

    virtual void listenPropertyEvent(const PropertyEvent* event) = 0;
#endif
};


class NodeEventListener : public PropertyEventListener
{
#ifdef SEAD_DEBUG
public:
    NodeEventListener()
        : PropertyEventListener()
    {
    }

    NodeEventListener(Heap* disposer_heap,
                      IDisposer::HeapNullOption option)
        : PropertyEventListener(disposer_heap, option)
    {
    }

    virtual void listenPropertyEvent(const PropertyEvent* event) {}

    virtual void listenNodeEvent(const NodeEvent* event) {}
#endif
};
}  // namespace hostio
}  // namespace sead

