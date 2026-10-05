// Filename: seadMessageQueueCtr.cpp
//
// Project: StandardEAD C++ Library for CTR

#include "thread/seadMessageQueue.h"
#include "basis/seadAssert.h"

namespace sead
{
MessageQueue::MessageQueue()
{
}

MessageQueue::~MessageQueue()
{
}

void MessageQueue::allocate(s32 size, Heap* heap)
{
    if (size <= 0)
    {
        SEAD_ASSERT_MSG(false, "MessageQueue size must not be zero");
        return;
    }

    mBuffer = new (heap) Element[size];
    mMessageQueueInner.Initialize(reinterpret_cast<uptr*>(mBuffer), size);
}

void MessageQueue::free()
{
    mMessageQueueInner.Finalize();
    if (mBuffer)
    {
        delete[] mBuffer;
        mBuffer = nullptr;
    }
}

bool MessageQueue::push(MessageQueue::Element message, MessageQueue::BlockType block_type)
{
    if (block_type == cBlock)
    {
        mMessageQueueInner.Enqueue(message);
        return true;
    }

    return mMessageQueueInner.TryEnqueue(message);
}

MessageQueue::Element MessageQueue::pop(MessageQueue::BlockType block_type)
{
    u32 message;

    if (block_type == cBlock)
    {
        mMessageQueueInner.Dequeue();
        return message;
    }

    if (mMessageQueueInner.TryDequeue(&message))
    {
        return message;
    }
    return 0;
}

MessageQueue::Element MessageQueue::peek(MessageQueue::BlockType block_type) const
{
    u32 message;

    if (block_type == cBlock)
    {
        mMessageQueueInner.GetFront();
        return message;
    }

    if (mMessageQueueInner.TryGetFront(&message))
    {
        return message;
    }
    return 0;
}

bool MessageQueue::jam(MessageQueue::Element message, MessageQueue::BlockType block_type)
{
    if (block_type == cBlock)
    {
        mMessageQueueInner.Jam(message);
        return true;
    }

    return mMessageQueueInner.TryJam(message);
}
} // namespace sead