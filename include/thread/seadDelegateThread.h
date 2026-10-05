#pragma once

#include <thread/seadMessageQueue.h>
#include <thread/seadThread.h>

namespace sead
{
template <typename A1, typename A2>
class IDelegate2;

class DelegateThread : public Thread
{
public:
    DelegateThread(const SafeString& name, IDelegate2<Thread*, MessageQueue::Element>* deleg, Heap* heap, s32 platformPriority = cDefaultPriority,
                   MessageQueue::BlockType blockType = MessageQueue::cBlock, MessageQueue::Element quitMsg = cDefaultQuitMsg,
                   s32 stackSize = cDefaultStackSize, s32 msgQueueSize = cDefaultMsgQueueSize);
    virtual ~DelegateThread();

protected:
    virtual void calc_(MessageQueue::Element msg);

    IDelegate2<Thread*, MessageQueue::Element>* mDelegate;
};

}  // namespace sead
