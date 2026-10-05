#pragma once

#include <nn/os.h>

namespace sead
{
class Heap;

class MessageQueue
{
public:
    typedef s64 Element;

    enum BlockType
    {
        cBlock,
        cNoBlock
    };

    MessageQueue();
    ~MessageQueue();

    void allocate(s32 size, Heap* heap);
    void free();
    bool push(Element message, BlockType block_type);
    Element pop(BlockType block_type);
    Element peek(BlockType block_type) const;
    bool jam(Element message, BlockType block_type);

    static const Element cNullElement = 0;

private:
    nn::os::BlockingQueue mMessageQueueInner;
    Element* mBuffer;
};
}  // namespace sead
