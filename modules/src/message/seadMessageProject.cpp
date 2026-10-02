// Filename: seadMessageProject.cpp
//
// Project: StandardEAD C++ Library for CTR

#include "message/seadMessageProject.h"
#include "basis/seadNew.h"

#include <LMS/libms.h>
#include <LMS/commonbin.h>

namespace sead
{

Heap* MessageProject::sHeap = NULL;

MessageProject::~MessageProject()
{
}

s32 MessageProject::initialize(void* data, Heap* heap)
{
    SEAD_ASSERT(mProjectFile == nullptr);
    SEAD_ASSERT(sHeap == nullptr);

    sHeap = heap;

    /* Setup libms */
    LMS_SetMemFuncs(allocForLibms_, freeForLibms_);

    mProjectFile = LMS_InitProject(data);

    s32 index = LMS_GetColorNum(mProjectFile);

    /* LMS Colors */
    if(index > 0)
    {
        for(Buffer<Color4u8>::iterator itr = mColorBuffer.begin(); itr != mColorBuffer.end(); ++itr)
        {
            LMS_Color color;
            LMS_GetColor(mProjectFile, itr.getIndex(), &color);

            itr->set(color.r, color.g, color.b, color.a);
        }
    }

    index = LMS_GetStyleNum(mProjectFile);

    /* LMS Proj General */
    if(index > 0)
    {
        for(Buffer<Style>::iterator itr = mStyleBuffer.begin(); itr != mStyleBuffer.end(); ++itr)
        {
            itr->regionWidth = LMS_GetRegionWidth(mProjectFile, itr.getIndex());
            itr->lineNum = LMS_GetLineNum(mProjectFile, itr.getIndex());
            s32 fontIndex = LMS_GetFontIndex(mProjectFile, itr.getIndex());
            if(fontIndex < 0)
            {
                itr->fontIndex = -1;
            }
            else
            {
                itr->fontIndex = fontIndex;
            }

            s32 baseIndex = LMS_GetBaseColorIndex(mProjectFile, itr.getIndex());
            if(baseIndex < 0)
            {
                itr->baseColorIndex = -1;
            }
            else
            {
                itr->baseColorIndex = baseIndex;
            }
        }
    }

    index = LMS_GetAttrNum(mProjectFile);
    /* LMS Attributes */
    if(index > 0)
    {
        for(Buffer<AttributeInfo>::iterator itr = mAttributeBuffer.begin(); itr != mAttributeBuffer.end(); ++itr)
        {
            itr->offset = LMS_GetAttrOffset(mProjectFile, itr.getIndex());
        }
    }

    s32 contentsIdx = LMS_GetContentsNum(mProjectFile);
    /* LMS Proj Binary Content */
    if(contentsIdx > 0)
    {
        mProjContents = 0;
    }
    else
    {
        mProjContents = contentsIdx;
    }
    return true;
}

void MessageProject::finalize()
{
    SEAD_ASSERT(mProjectFile);

    mColorBuffer.freeBuffer();
    mStyleBuffer.freeBuffer();

    LMS_SetMemFuncs(0, freeForLibms_);
    LMS_CloseProject(mProjectFile);

    mProjectFile = NULL;
    mProjContents = 0;

    LMS_SetMemFuncs(0, 0);
}

void* MessageProject::allocForLibms_(u32 size)
{
    return new(sHeap) u32[size];
}

void MessageProject::freeForLibms_(void* ptr)
{
    new(sHeap) void*[(s32)ptr];
}

}