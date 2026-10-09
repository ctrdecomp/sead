#pragma once

#include "gfx/ctr/seadTextureCtr.h"

namespace sead
{
class TextureCtrGR : public TextureCtr
{
    SEAD_RTTI_OVERRIDE(TextureCtrGR, Texture)
public:
    TextureCtrGR();

    virtual ~TextureCtrGR();
    virtual u32 getWidth() const { return mWidth; }
    virtual u32 getHeight() const{ return mHeight; }
    virtual u32 getPhysicalAddress() const{ return mPhysicalAddr; }
    virtual u32 getFormatGR() const{ return mFormat; }
    virtual u32 getMaxLodLevel() const{ return mMaxLodLevel; }
private:
    u32 mWidth;
    u32 mHeight;
    u32 mPhysicalAddr;
    u8  mFormat;
    u8  mMaxLodLevel;
};
}
