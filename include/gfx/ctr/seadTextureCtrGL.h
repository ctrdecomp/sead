#pragma once

#include "gfx/ctr/seadTextureCtr.h"

namespace sead
{
class TextureCtrGL : public TextureCtr
{
    SEAD_RTTI_OVERRIDE(TextureCtrGL, Texture)
public:
    TextureCtrGL();

    virtual ~TextureCtrGL();
    virtual u32 getWidth() const { return mWidth; }
    virtual u32 getHeight() const{ return mHeight; }
    virtual u32 getPhysicalAddress() const;
    virtual u32 getFormatGR() const;
    virtual u32 getMaxLodLevel() const;

    u32 getTextureID() const{ return mGlTextureId; }
    u32 getLevel() const{ return mGlLevel; }
private:
    u32 mWidth;
    u32 mHeight;
    GLenum mGlModeId;
    GLenum mGlSize;
    GLenum mGlTextureId;
    GLenum mGlLevel;
};
}
