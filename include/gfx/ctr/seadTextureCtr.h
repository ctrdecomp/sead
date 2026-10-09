#pragma once

#include "gfx/seadTexture.h"
#include "gfx/seadTextureUtil.h"

namespace sead
{
class TextureCtr : public Texture
{
    SEAD_RTTI_OVERRIDE(TextureCtr, Texture)
public:
    TextureCtr();

    virtual ~TextureCtr();
    virtual u32 getWidth() const = 0;
    virtual u32 getHeight() const = 0;
    virtual u32 getPhysicalAddress() const = 0;
    virtual u32 getFormatGR() const = 0;
    virtual u32 getMaxLodLevel() const = 0;
};
}
