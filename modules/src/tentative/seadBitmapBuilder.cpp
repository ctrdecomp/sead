// Filename: seadBitmapBuilder.cpp
//
// Project: StandardEAD C++ Library for CTR

#include "tentative/seadBitmapBuilder.h"

namespace
{
int calcPaddingSize_(int size)
{
    return (4 - (size * 3) % 4) % 4;
}

u8 expand5bitTo8bit_(u16 value)
{
    return (value * 255) / 31;
}

u8 expand6bitTo8bit_(u16 value)
{
    return (value * 255) / 63;
}
}
namespace sead
{
BitmapBuilder::BitmapBuilder(WriteStream* stream, s32 width, s32 height):
    mWriteStream(stream),
    mWidth(width),
    mHeight(height)
{
    mWriteStream->setBinaryEndian(Endian::cLittle);
}

BitmapBuilder::~BitmapBuilder()
{
}

void BitmapBuilder::writeInfoHeader()
{
    mWriteStream->writeU32(40);

    SEAD_ASSERT(mWidth >= 1);
    mWriteStream->writeS32(mWidth);

    SEAD_ASSERT(mHeight != 0);
    mWriteStream->writeS32(mHeight);

    mWriteStream->writeU16(1);
    mWriteStream->writeU16(24);

    mWriteStream->writeU32(0);
    mWriteStream->writeU32(0);
    mWriteStream->writeU32(0);
    mWriteStream->writeU32(0);
    mWriteStream->writeU32(0);
    mWriteStream->writeU32(0);
}

void BitmapBuilder::writeFileHeader()
{
    mWriteStream->writeU8('B');
    mWriteStream->writeU8('M');
    mWriteStream->writeU32(0);
    mWriteStream->writeU16(0);
    mWriteStream->writeU16(0);
    mWriteStream->writeU32(54);
}

void BitmapBuilder::writeImageLineA1BGR5(void* ptr, u32 width)
{
    u8* data = static_cast<u8*>(ptr);
    u32 size = 0;
    if(mWidth < width)
    {
        width = mWidth;
    }

    for(; size < width; size++)
    {
        u16 pixel = *reinterpret_cast<u16*>(data[size]);
        mWriteStream->writeU8(expand5bitTo8bit_(pixel & 0x1F));

        mWriteStream->writeU8(expand5bitTo8bit_((pixel >> 5) & 0x1F));

        mWriteStream->writeU8(expand5bitTo8bit_((pixel >> 11) & 0x1F));
    }

    for(; size < mWidth; size++)
    {
        mWriteStream->writeU8(0);
        mWriteStream->writeU8(0);
        mWriteStream->writeU8(0);
    }

    for (int i = 0; i < calcPaddingSize_(mWidth); ++i)
    {
        mWriteStream->writeU8(0);
    }
}

void BitmapBuilder::writeImageLineA1RGB5(void* ptr, u32 width)
{
    u8* data = static_cast<u8*>(ptr);
    u32 size = 0;

    if (mWidth < width)
    {
        width = mWidth;
    }

    for (; size < width; size++)
    {
        u8 lo = data[size];
        u8 hi = data[size + 1];

        mWriteStream->writeU8(
            expand5bitTo8bit_(hi >> 3));

        mWriteStream->writeU8(expand5bitTo8bit_(((hi & 0x07) << 2) | (lo >> 6)));

        mWriteStream->writeU8(expand5bitTo8bit_((lo >> 1) & 0x1F));
    }

    for (; size < mWidth; size++)
    {
        mWriteStream->writeU8(0);
        mWriteStream->writeU8(0);
        mWriteStream->writeU8(0);
    }

    for (int i = 0; i < calcPaddingSize_(mWidth); ++i)
    {
        mWriteStream->writeU8(0);
    }
}

void BitmapBuilder::writeImageLineABGR4(void* ptr, u32 width)
{
    u8* data = static_cast<u8*>(ptr);
    u32 size = 0;

    if (mWidth < width)
    {
        width = mWidth;
    }

    for (; size < width; size++)
    {
        u8 lo = data[size];
        u8 hi = data[size + 1];

        mWriteStream->writeU8(lo & 0xF0);
        mWriteStream->writeU8((hi & 0x0F) << 4);
        mWriteStream->writeU8(hi & 0xF0);
    }

    for (; size < mWidth; size++)
    {
        mWriteStream->writeU8(0);
        mWriteStream->writeU8(0);
        mWriteStream->writeU8(0);
    }

    for (int i = 0; i < calcPaddingSize_(mWidth); ++i)
    {
        mWriteStream->writeU8(0);
    }
}

void BitmapBuilder::writeImageLineABGR8(void* ptr, u32 width)
{
    u8* data = static_cast<u8*>(ptr);
    u32 size = 0;

    if (mWidth < width)
    {
        width = mWidth;
    }

    for (; size < width; size++)
    {
        mWriteStream->writeMemBlock(reinterpret_cast<void*>(data[size + 1]), 3);
    }

    for (; size < mWidth; size++)
    {
        mWriteStream->writeU8(0);
        mWriteStream->writeU8(0);
        mWriteStream->writeU8(0);
    }

    for (int i = 0; i < calcPaddingSize_(mWidth); ++i)
    {
        mWriteStream->writeU8(0);
    }
}

void BitmapBuilder::writeImageLineBGR565(void* ptr, u32 width)
{
    u8* data = static_cast<u8*>(ptr);
    u32 size = 0;

    if (mWidth < width)
    {
        width = mWidth;
    }

    for (; size < width; size++)
    {
        u8 lo = data[size];
        u8 hi = data[size + 1];

        mWriteStream->writeU8(expand5bitTo8bit_(lo & 0x1F));
        mWriteStream->writeU8(expand6bitTo8bit_((lo >> 5) | ((hi & 0x07) << 3)));
        mWriteStream->writeU8(expand5bitTo8bit_(hi >> 3));
    }

    for (; size < mWidth; size++)
    {
        mWriteStream->writeU8(0);
        mWriteStream->writeU8(0);
        mWriteStream->writeU8(0);
    }

    for (int i = 0; i < calcPaddingSize_(mWidth); ++i)
    {
        mWriteStream->writeU8(0);
    }
}

void BitmapBuilder::writeImageLineBGR8(void* ptr, u32 width)
{
    if (mWidth < width)
    {
        width = mWidth;
    }

    mWriteStream->writeMemBlock(ptr, width * 3);

    for (u32 size = width; size < mWidth; size++)
    {
        mWriteStream->writeU8(0);
        mWriteStream->writeU8(0);
        mWriteStream->writeU8(0);
    }

    for (int i = 0; i < calcPaddingSize_(mWidth); ++i)
    {
        mWriteStream->writeU8(0);     
    }
}

void BitmapBuilder::writeImageLineRGB565(void* ptr, u32 width)
{
    u8* data = static_cast<u8*>(ptr);
    u32 size = 0;

    if (mWidth < width)
    {
        width = mWidth;
    }

    for (; size < width; size++)
    {
        u8 lo = data[size];
        u8 hi = data[size + 1];

        mWriteStream->writeU8(expand5bitTo8bit_(hi >> 3));
        mWriteStream->writeU8(expand6bitTo8bit_((lo >> 5) | ((hi & 0x07) << 3)));
        mWriteStream->writeU8(expand5bitTo8bit_(lo & 0x1F));
    }

    for (; size < mWidth; size++)
    {
        mWriteStream->writeU8(0);
        mWriteStream->writeU8(0);
        mWriteStream->writeU8(0);
    }

    for (int i = 0; i < calcPaddingSize_(mWidth); ++i)
    {
        mWriteStream->writeU8(0);
    }
}

void BitmapBuilder::writeImageLineRGB8(void* ptr, u32 width)
{
    u8* data = static_cast<u8*>(ptr);
    u32 size = 0;

    if (mWidth < width)
    {
        width = mWidth;
    }

    for (; size < width; size++)
    {
        mWriteStream->writeU8(data[size + 2]);
        mWriteStream->writeU8(data[size + 1]);
        mWriteStream->writeU8(data[size]);
    }

    for (; size < mWidth; size++)
    {
        mWriteStream->writeU8(0);
        mWriteStream->writeU8(0);
        mWriteStream->writeU8(0);
    }

    for (int i = 0; i < calcPaddingSize_(mWidth); ++i)
    {
        mWriteStream->writeU8(0);
    }
}

void BitmapBuilder::writeImageLineRGBA4(void* ptr, u32 width)
{
    u8* data = static_cast<u8*>(ptr);
    u32 size = 0;

    if (mWidth < width)
    {
        width = mWidth;
    }

    for (; size < width; size++)
    {
        u8 hi = data[size];
        u8 lo = data[size + 1];

        mWriteStream->writeU8((lo << 4) & 0xF0);
        mWriteStream->writeU8(hi & 0xF0);
        mWriteStream->writeU8((hi << 4) & 0xF0);
    }

    for (; size < mWidth; size++)
    {
        mWriteStream->writeU8(0);
        mWriteStream->writeU8(0);
        mWriteStream->writeU8(0);
    }

    for (int i = 0; i < calcPaddingSize_(mWidth); ++i)
    {
        mWriteStream->writeU8(0);
    }
}

void BitmapBuilder::writeImageLineRGBA8(void* ptr, u32 width)
{
    u8* data = static_cast<u8*>(ptr);
    u32 size = 0;

    if (mWidth < width)
    {
        width = mWidth;
    }

    for (; size < width; size++)
    {
        mWriteStream->writeU8(data[size + 2]);
        mWriteStream->writeU8(data[size + 1]);
        mWriteStream->writeU8(data[size]);
    }

    for (; size < mWidth; size++)
    {
        mWriteStream->writeU8(0);
        mWriteStream->writeU8(0);
        mWriteStream->writeU8(0);
    }

    for (int i = 0; i < calcPaddingSize_(mWidth); ++i)
    {
        mWriteStream->writeU8(0);
    }
}
}
