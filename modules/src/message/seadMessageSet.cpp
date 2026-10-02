// Filename: seadMessageSet.cpp
//
// Project: StandardEAD C++ Library for CTR

#include "message/seadMessageSet.h"
#include "basis/seadNew.h"

#include <LMS/libms.h>

namespace sead
{

Heap* MessageSetBase::sHeap = NULL;

MessageSetBase::~MessageSetBase()
{
}

s32 MessageSetBase::initialize(void* data, Heap* heap)
{
    SEAD_ASSERT(mMsgFile == nullptr);
    SEAD_ASSERT(sHeap == nullptr);

    sHeap = heap;

    /* Setup libms */
    LMS_SetMemFuncs(allocForLibms_, freeForLibms_);

    mMsgFile = LMS_InitMessage(data);

    s32 msgNum = LMS_GetTextNum(mMsgFile);

    if(msgNum < 0)
    {
        SEAD_ASSERT_MSG(false, "failed to init message.[%d]", msgNum);
        LMS_CloseMessage(mMsgFile);

        mMsgFile = NULL;
        mTextSize = 0;
    }
    else
    {
        mTextSize = 0;
    }

    LMS_SetMemFuncs(0, 0);
    sHeap = NULL;

    return msgNum >> 0x1F + 1;
}

void MessageSetBase::finalize()
{
    SEAD_ASSERT(mMsgFile);

    LMS_SetMemFuncs(0, freeForLibms_);
    LMS_CloseMessage(mMsgFile);

    mMsgFile = NULL;
    mTextSize = 0;

    LMS_SetMemFuncs(0, 0);
}

s32 MessageSetBase::calcTextSizeByIndex(s32 index) const
{
    if(index < mTextSize)
    {
        return LMS_GetTextSize(mMsgFile, index);
    }

    SEAD_ASSERT_MSG(false, "index exceeded. index = %d", index);
    return 0;
}

void* MessageSetBase::allocForLibms_(u32 size)
{
    return new(sHeap) u32[size];
}

s32 MessageSetBase::searchTextLabelByIndex(BufferedSafeString* string, s32 index) const
{
    if(string->getBufferSize() <= 256)
    {
        SEAD_ASSERT_MSG(false, "index exceeded. index = %d", index);
        return 0;
    }
    if(mTextSize <= index)
    {
        SEAD_ASSERT_MSG(false, "buffer size short. buffer size = %d, index = %d", string->getBufferSize(), index);
        LMS_GetLabelByTextIndex(mMsgFile, index, string->getBuffer());
        return 0;
    }

    LMS_GetLabelByTextIndex(mMsgFile, index, string->getBuffer());
    return 1;
}

void MessageSetBase::freeForLibms_(void* ptr)
{
    new(sHeap) void*[(s32)ptr];
}

}