#pragma once

#include <algorithm>
#include "basis/seadNew.h"
#include "basis/seadAssert.h"
#include "basis/seadTypes.h"
#include "container/seadFreeList.h"
#include "container/seadListImpl.h"
#include "prim/seadPtrUtil.h"

namespace sead
{

template <typename T>
class ObjList : public ListImpl
{
public:
    ObjList()
        : ListImpl()
        , mFreeList()
        , mMaxNum(0)
    {
    }

    ObjList(s32 max_num, void* buf)
        : ListImpl()
        , mFreeList()
        , mMaxNum(0)
    {
        setBuffer(max_num, buf);
    }

    void allocBuffer(s32 capacity, Heap* heap, s32 alignment = sizeof(void*))
    {
        if (capacity < 1)
            return;

        setBuffer(capacity,
                  new (heap, alignment, std::nothrow) u8[calculateWorkBufferSize(capacity)]);
    }

    bool tryAllocBuffer(s32 capacity, Heap* heap, s32 alignment = sizeof(void*))
    {
        if (capacity < 1)
            return false;

        u8* buf = new (heap, alignment, std::nothrow) u8[calculateWorkBufferSize(capacity)];
        if (!buf)
            return false;

        setBuffer(capacity, buf);
        return true;
    }

    void setBuffer(s32 max_num, void* buf)
    {
        if (!buf)
        {
            SEAD_ASSERT_MSG(false, "buf is null");
            return;
        }

        mFreeList.setWork(buf, ElementSize, max_num);
        mMaxNum = max_num;
    }

    void freeBuffer()
    {
        if (!isBufferReady())
            return;

        clear();

        if (mFreeList.work())
            delete[] static_cast<u8*>(mFreeList.work());

        mMaxNum = 0;
        mFreeList.reset();
    }

    bool isBufferReady() const
    {
        return mFreeList.work() != NULL;
    }

    bool isFull() const
    {
        return size() >= mMaxNum;
    }

    T* front() const
    {
        return listNodeToObjWithNullCheck(ListImpl::front());
    }

    T* back() const
    {
        return listNodeToObjWithNullCheck(ListImpl::back());
    }

    T* birthBack()
    {
        if (isFull())
        {
            SEAD_ASSERT_MSG(false, "buffer full.");
            return NULL;
        }

        Node* node = new (mFreeList.getFree()) Node();
        ListImpl::pushBack(objToListNode(&node->item));

        return &node->item;
    }

    T popBack()
    {
        T* item = back();
        if (!item)
            return T();

        T copy = *item;
        erase(item);
        return copy;
    }

    T popFront()
    {
        T* item = front();
        if (!item)
            return T();

        T copy = *item;
        erase(item);
        return copy;
    }

    T* emplaceBack()
    {
        T* obj = allocObject();
        if (!obj)
            return NULL;

        new (obj) T();
        return obj;
    }

    template <class A1>
    T* emplaceBack(const A1& a1)
    {
        T* obj = allocObject();
        if (!obj)
            return NULL;

        new (obj) T(a1);
        return obj;
    }

    template <class A1, class A2>
    T* emplaceBack(const A1& a1, const A2& a2)
    {
        T* obj = allocObject();
        if (!obj)
            return NULL;

        new (obj) T(a1, a2);
        return obj;
    }

    void erase(T* item)
    {
        ListImpl::erase(objToListNode(item));
        item->~T();
        mFreeList.free(item);
    }

    void clear()
    {
        ListNode* node = mStartEnd.next();

        while (node != &mStartEnd)
        {
            ListNode* next = node->next();

            ListImpl::erase(node);

            T* item = listNodeToObj(node);
            item->~T();
            mFreeList.free(item);

            node = next;
        }
    }

    T* prev(const T* obj) const
    {
        ListNode* prev_node = objToListNode(obj)->prev();

        if (prev_node == &mStartEnd)
            return NULL;

        return listNodeToObj(prev_node);
    }

    T* next(const T* obj) const
    {
        ListNode* next_node = objToListNode(obj)->next();

        if (next_node == &mStartEnd)
            return NULL;

        return listNodeToObj(next_node);
    }

    T* nth(s32 n) const
    {
        return listNodeToObjWithNullCheck(ListImpl::nth(n));
    }

    s32 indexOf(const T* obj) const
    {
        return ListImpl::indexOf(objToListNode(obj));
    }

    bool isNodeLinked(const T* obj) const
    {
        return objToListNode(obj)->isLinked();
    }

    class iterator
    {
    public:
        explicit iterator(T* ptr)
            : mPtr(ptr)
        {
        }

        bool operator==(const iterator& other) const
        {
            return mPtr == other.mPtr;
        }

        bool operator!=(const iterator& other) const
        {
            return !operator==(other);
        }

        iterator& operator++()
        {
            const s32 offset = Node::getListNodeOffset();

            ListNode* node =
                static_cast<ListNode*>(PtrUtil::addOffset(mPtr, offset))->next();

            mPtr = static_cast<T*>(PtrUtil::addOffset(node, -offset));
            return *this;
        }

        T& operator*() const
        {
            return *mPtr;
        }

        T* operator->() const
        {
            return mPtr;
        }

    private:
        T* mPtr;
    };

    iterator begin() const
    {
        return iterator(listNodeToObj(mStartEnd.next()));
    }

    iterator end() const
    {
        return iterator(listNodeToObj(const_cast<ListNode*>(&mStartEnd)));
    }

    iterator begin(T* ptr) const
    {
        return iterator(ptr);
    }

    static size_t calculateWorkBufferSize(size_t n)
    {
        return n * ElementSize;
    }

private:
    struct Node
    {
        static s32 getListNodeOffset()
        {
            return offsetof(Node, node);
        }

        T item;
        ListNode node;
    };

    T* allocObject()
    {
        if (isFull())
        {
            SEAD_ASSERT_MSG(false, "buffer full.");
            return NULL;
        }

        Node* node = static_cast<Node*>(mFreeList.getFree());

        ListImpl::pushBack(objToListNode(&node->item));

        return &node->item;
    }

    ListNode* objToListNode(T* obj) const
    {
        return static_cast<ListNode*>(
            PtrUtil::addOffset(obj, Node::getListNodeOffset()));
    }

    const ListNode* objToListNode(const T* obj) const
    {
        return static_cast<const ListNode*>(
            PtrUtil::addOffset(obj, Node::getListNodeOffset()));
    }

    T* listNodeToObj(ListNode* node) const
    {
        return static_cast<T*>(
            PtrUtil::addOffset(node, -Node::getListNodeOffset()));
    }

    const T* listNodeToObj(const ListNode* node) const
    {
        return static_cast<const T*>(
            PtrUtil::addOffset(node, -Node::getListNodeOffset()));
    }

    T* listNodeToObjWithNullCheck(ListNode* node) const
    {
        return node ? listNodeToObj(node) : NULL;
    }

    const T* listNodeToObjWithNullCheck(const ListNode* node) const
    {
        return node ? listNodeToObj(node) : NULL;
    }

public:
    static const size_t ElementSize = sizeof(Node) > FreeList::cPtrSize ? sizeof(Node) : FreeList::cPtrSize;

private:

    sead::FreeList mFreeList;
    s32 mMaxNum;
};

template <typename T, s32 N>
class FixedObjList : public ObjList<T>
{
public:
    FixedObjList()
        : ObjList<T>(N, &mWork)
    {
    }

    void setBuffer(s32 ptrNumMax, void* buf);
    void allocBuffer(s32 ptrNumMax, Heap* heap,
                     s32 alignment = sizeof(void*));
    bool tryAllocBuffer(s32 ptrNumMax, Heap* heap,
                        s32 alignment = sizeof(void*));
    void freeBuffer();

private:
    union WorkBuffer
    {
        u8 data[N * ObjList<T>::ElementSize];
        T alignT;
        T* alignPtr;
    };

    WorkBuffer mWork;
};

} // namespace sead