#pragma once

#include "basis/seadTypes.h"
#include "container/seadListImpl.h"
#include "container/seadOffsetList.h"

namespace sead
{
class MemBlock
{
public:
    MemBlock(): 
        mListNode(), 
        mHeapCheckTag(0), 
        mOffset(0), 
        mSize(0)
    {
    }

    static MemBlock* FindManageArea(void* ptr);

    static u32 getOffset() { return offsetof(MemBlock, mListNode); }

protected:
    ListNode mListNode;
    u16 mHeapCheckTag;
    u16 mOffset;
    size_t mSize;
};

typedef OffsetList<MemBlock> MemBlockList;

}  // namespace sead
