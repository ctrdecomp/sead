// Filename: seadTextureCtrGR.cpp
//
// Project: StandardEAD C++ Library for CTR

#include "gfx/ctr/seadTextureCtrGR.h"

namespace sead
{
TextureCtrGR::TextureCtrGR():
    mWidth(1),
    mHeight(1),
    mPhysicalAddr(0),
    mFormat(0),
    mMaxLodLevel(0)
{
}

TextureCtrGR::~TextureCtrGR()
{
}
}
