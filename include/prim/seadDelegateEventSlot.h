
#pragma once

// DelegateEvent is used to implement a Qt-style signal/slot mechanism.

#include <container/seadTList.h>
#include <heap/seadDisposer.h>
#include <prim/seadDelegate.h>
#include <prim/seadStorageFor.h>

namespace sead
{

/// Manages signal and slots for an event.
template <typename T>
class DelegateEvent
{
public:
    class Slot;
    typedef TList<Slot*> SlotList;
    typedef TListNode<Slot*> SlotListNode;

    /// A Slot is a wrapper around a Delegate that is invoked when a signal is emitted.
    class Slot : public IDisposer
    {
    public:
        template <typename TDelegate>
        Slot(TDelegate delegate)
            : mNode(this)
            , mDelegatePtr(NULL)
            , mConnectedToDelegateEvent(false)
        {
            mDelegate.construct(delegate);
            mDelegatePtr = mDelegate->getDelegate();
        }

        template <typename C>
        Slot(C* instance, void (C::*func)(T))
            : mNode(this)
            , mDelegatePtr(NULL)
            , mConnectedToDelegateEvent(false)
        {
            mDelegate.construct(Delegate1<C, T>(instance, func));
            mDelegatePtr = mDelegate->getDelegate();
        }

        virtual ~Slot()
        {
            release();
        }

        void release()
        {
            if (mConnectedToDelegateEvent)
            {
                mNode.erase();
                mConnectedToDelegateEvent = false;
            }
        }

    private:
        friend class DelegateEvent<T>;

        void invoke_(T arg)
        {
            if (mDelegatePtr)
                (*mDelegatePtr)(arg);
        }

        SlotListNode mNode;
        IDelegate1<T>* mDelegatePtr;
        StorageFor<AnyDelegate1<T>, true> mDelegate;
        bool mConnectedToDelegateEvent;
    };

    virtual ~DelegateEvent()
    {
        for (typename SlotList::iterator it = mList.begin(); it != mList.end(); )
        {
            Slot* s = *it;
            ++it;
            s->release();
        }
    }

    DelegateEvent& operator+=(Slot& slot)
    {
        connect(slot);
        return *this;
    }

    void connect(Slot& slot)
    {
        slot.release();
        mList.pushBack(&slot.mNode);
        slot.mConnectedToDelegateEvent = true;
    }

    void disconnect(Slot& slot)
    {
        slot.release();
    }

    void emit(T arg)
    {
        for (typename SlotList::robustIterator it = mList.robustBegin(); it != mList.robustEnd(); )
        {
            Slot* s = it->mData;
            ++it;
            s->release();
        }
    }

    void fire(T arg)
    {
        emit(arg);
    }

    int getNumSlots() const
    {
        return mList.size();
    }

protected:
    SlotList mList;
};

} // namespace sead