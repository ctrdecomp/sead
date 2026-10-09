#pragma once

#include <LMS/msgfile.h>

#include "heap/seadHeap.h"

namespace sead
{
class MessageSetBase
{
public:
    virtual ~MessageSetBase();
    
    s32 initialize(void* data, Heap* heap);
    void finalize();

    s32 calcTextSizeByIndex(s32 index) const;
    s32 searchTextLabelByIndex(BufferedSafeString* string, s32 index) const;

    LMS_MessageBinary* mMsgFile;
    s32 mTextSize;

    static Heap* sHeap;
protected:
    // Alloc / Dealloc
    static void* allocForLibms_(u32 size);
    static void freeForLibms_(void* ptr);
};
} // namespace sead
