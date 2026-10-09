// Filename: seadTextureCtrGL.cpp
//
// Project: StandardEAD C++ Library for CTR

#include "gfx/ctr/seadTextureCtrGL.h"
#include <nn/gx/CTR/gx_CommandAccess.h>

namespace
{
u32 convertTexFmtEnum2Reg_(GLenum fmt, GLenum type)
{
    switch (fmt)
    {
    case GL_RGB_NATIVE_DMP:
    {
        if (type == GL_BYTE)
            return 1;

        if (type == GL_UNSIGNED_SHORT_5_6_5)
            return 3;

        SEAD_ASSERT_MSG(false, "Undefined type");
        return 1;
    }

    case GL_ALPHA_NATIVE_DMP:
        return 8;

    case GL_LUMINANCE_NATIVE_DMP:
        return 7;

    case GL_LUMINANCE_ALPHA_NATIVE_DMP:
        return 5;

    case GL_HILO8_DMP_NATIVE_DMP:
        return 6;

    case GL_ETC1_RGB8_NATIVE_DMP:
        return 12;

    case GL_ETC1_ALPHA_RGB8_A4_NATIVE_DMP:
        return 13;

    case GL_ALPHA:
    {
        if (type == GL_BYTE)
            return 8;

        if (type == GL_UNSIGNED_SHORT_5_6_5)
            return 7;

        SEAD_ASSERT_MSG(false, "Undefined type");
        return 7;
    }

    default:
        SEAD_ASSERT_MSG(false, "Undefined type");
        return 13;
    }
}
}

namespace sead
{
TextureCtrGL::TextureCtrGL():
    mWidth(0),
    mHeight(0),
    mGlModeId(GL_RGB),
    mGlSize(GL_UNSIGNED_BYTE),
    mGlTextureId(GL_SAVE_PROGRAMS_DMP),
    mGlLevel(GL_SAVE_PROGRAMS_DMP)
{
}

TextureCtrGL::~TextureCtrGL()
{
    if(mGlTextureId != GL_SAVE_PROGRAMS_DMP)
    {
        glDeleteTextures(1, &mGlTextureId);
    }
}

u32 TextureCtrGL::getPhysicalAddress() const
{
    glBindTexture(GL_TEXTURE_2D, getTextureID());
    GLint t;
    glGetTexParameteriv(GL_TEXTURE_2D, GL_TEXTURE_DATA_ADDR_DMP, &t);
    SEAD_ASSERT_GL();
    return nngxGetPhysicalAddr(t);
}

u32 TextureCtrGL::getFormatGR() const
{
    return convertTexFmtEnum2Reg_(mGlModeId, mGlSize);
}

u32 TextureCtrGL::getMaxLodLevel() const
{
    return getLevel() & 0xFF;
}
}
