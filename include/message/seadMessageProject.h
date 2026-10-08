#pragma once

#include <LMS/projfile.h>
#include "heap/seadHeap.h"
#include "container/seadBuffer.h"
#include "gfx/seadColor.h"

namespace sead
{

class MessageProject
{
public:
    typedef LMS_Style Style;
    typedef LMS_AttrInfo AttributeInfo;
    
    virtual ~MessageProject();
    s32 initialize(void* data, Heap* heap);
    void finalize();

    LMS_ProjectBinary* mProjectFile;
    Buffer<Color4u8> mColorBuffer;
    Buffer<Style> mStyleBuffer;
    Buffer<AttributeInfo> mAttributeBuffer;
    s32 mProjContents;

    static Heap* sHeap;
protected:
    // Alloc / Dealloc
    static void* allocForLibms_(u32 size);
    static void freeForLibms_(void* ptr);
};
} // namespace sead
