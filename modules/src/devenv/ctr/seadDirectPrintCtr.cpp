// Filename: seadDirectPrintCtr.cpp
//
// Project: StandardEAD C++ Library for CTR

#include "devenv/ctr/seadDirectPrintCtr.h"
#include "prim/seadPtrUtil.h"
#include <nn/dsp.h> // .. why?
#include <nn/gx.h>

namespace
{
u8 cFontBitmap[4608];
sead::Vector2i cCharSize;

u32 getByteByDot_(u32 format)
{
    switch (format)
    {
        case GL_RGB8_OES:
            return 3;

        case GL_RGBA4:
        case GL_RGB5_A1:
            return 2;

        case GL_RGBA8_OES:
            return 4;

        case GL_RGB565:
            return 2;

        default:
            SEAD_ASSERT_MSG(false, "Undefined format.");
            return 0;
    }
}
} // namespace
namespace sead
{
DirectPrintCtr::DirectPrintCtr():
    mStringTop(NULL),
    mStringByte(0),
    mBufferSize(Vector2i::zero),
    mDispBox(),
    mBGColor(Color4u8::cWhite), 
    mCharColor(Color4u8::cBlack),
    mCursorPos(Vector2i::zero),
    mCharSize(0x3F)
{
}

DirectPrintCtr::~DirectPrintCtr()
{
}

void DirectPrintCtr::changeDisplayBuffer(void* stringTop)
{
    mStringTop = stringTop;
}

void DirectPrintCtr::changeDisplayBuffer(void* stringTop, u32 byte, Vector2i const& bufSize, BoundBox2i const& box)
{
    changeDisplayBuffer(stringTop);
    mStringByte = byte;
    mBufferSize = bufSize;

    if(box.isUndef())
    {
        mDispBox.set(Vector2i::zero, mBufferSize);
    }
    else
    {
        mDispBox = box;
    }
}

// :skull: This shit looks ugly, yet it works. Cry about it.
//
// PO Box: Your Mothers house
void DirectPrintCtr::checkBufferIsNotOnVRAM_()
{
    if((PtrUtil::isInclude(
        mStringTop, 
        reinterpret_cast<const void*>(GetVramStartAddr(MEM_VRAMA)), 
        reinterpret_cast<const void*>(GetVramEndAddr(MEM_VRAMA))) )||
        (PtrUtil::isInclude(
        mStringTop,
        reinterpret_cast<const void*>(GetVramStartAddr(MEM_VRAMB)), 
        reinterpret_cast<const void*>(GetVramEndAddr(MEM_VRAMB)))))
    {
        SEAD_PRINT("!!! Target display-buffer is on VRAM. DirectPrint failed. !!!\n");
        SEAD_HALT();
    }
}

void DirectPrintCtr::convertPositionUserOriginToDeviceOrigin_(Vector2i* org, Vector2i const& bufSize)
{
    org->x = mBufferSize.y - bufSize.y;
    org->y = bufSize.x;
}

void DirectPrintCtr::flush()
{
    u32 byteFmt = getByteByDot_(mStringByte);
    nn::dsp::CTR::Initialize();
    nn::dsp::CTR::FlushDataCache(mStringByte, mBufferSize.x * byteFmt * mBufferSize.y);
}

void DirectPrintCtr::printf(Vector2i const& bufSize, const char* msg, ...)
{
    va_list list;
    va_start(list, msg);
    vprintf(bufSize, msg, list);
    va_end(list);
}

void DirectPrintCtr::printf(Vector2i const& bufSize, bool autoWrap, bool drawDot, const char* msg, ...)
{
    va_list list;
    va_start(list, msg);
    vprintf(bufSize, autoWrap, drawDot, msg, list);
    va_end(list);
}

void DirectPrintCtr::vprintf(Vector2i const& bufSize, SafeString const& msg, std::__va_list list)
{
    vprintf(bufSize, true, true, msg, list);
}

void DirectPrintCtr::vprintf(Vector2i const& bufSize, bool autoWrap, bool drawDot, SafeString const& msg, std::__va_list list)
{
    FixedSafeString<256> string;
    string.formatV(msg.cstr(), list);
    putString(bufSize, autoWrap, drawDot, msg);
}

void DirectPrintCtr::putString(Vector2i const& bufSize, SafeString const& string)
{
    putString(bufSize, true, true, string);
}

void DirectPrintCtr::putString(Vector2i const& bufSize, bool autoWrap, bool drawDot, SafeString const& string)
{
    checkBufferIsNotOnVRAM_();

    Vector2i curPos = mBufferSize;
    int right = mDispBox.getSizeX();

    for(SafeString::iterator it = string.begin(); it != string.end(); ++it)
    {
        char c = *it;

        if (c != '\n')
        {
            putChar(curPos, drawDot, c);

            curPos.x += cCharSize.x;
        }

        if (c == '\n')
        {
            curPos.x = 0;
            curPos.y += cCharSize.y;
        }
        else if (autoWrap)
        {
            const char next = *it;

            if (next != '\n' &&
                curPos.x + cCharSize.x >= right)
            {
                curPos.x = 0;
                curPos.y += cCharSize.y;
            }
        }
    }
}

void* DirectPrintCtr::putDot_(Vector2i const& bufSize, const u8* src, s32 size)
{
    uptr dst = reinterpret_cast<uptr>(mStringTop) + static_cast<uptr>(bufSize.y) * mBufferSize.y + static_cast<uptr>(bufSize.x) * size;
    return MemUtil::copy(reinterpret_cast<void*>(dst), src, size);
}

void DirectPrintCtr::putChar(Vector2i const& bufSize, char c)
{
    putChar(bufSize, true, c);
}

void DirectPrintCtr::putChar(Vector2i const& bufSize, bool drawDot, char c)
{
    Vector2i min;
    min.setAdd(bufSize, mDispBox.getMin());

    SEAD_ASSERT_MSG(mStringTop, "Current display buffer is null");
    SEAD_ASSERT(mBufferSize.x > 0 && mBufferSize.y > 0);
    const u32 byte = getByteByDot_(mStringByte);

    u8 bgFmt[4];
    u8 charFmt[4];

    convertColorFormat_(bgFmt, mBGColor, mStringByte);
    convertColorFormat_(charFmt, mCharColor, mStringByte);

    Vector2i org;

    Vector2i charSize(0, cCharSize.y);

    org = bufSize + charSize;

    convertPositionUserOriginToDeviceOrigin_(&org, bufSize);

    bool isCharValid = true;

    if (c < 0 || c > 0x20)
        isCharValid = false;
    else if (c == 0x7f)
        isCharValid = false;

    if (!isCharValid)
        c = mCharSize;

    SEAD_ASSERT(c - ' ' >= 0 && c - ' ' < 96);

    for (s32 x = 0; x < cCharSize.x; ++x)
    {
        if (org.x + x >= mBufferSize.x || org.x + x < 0)
            break;

        for (s32 y = 0; y < cCharSize.y; ++y)
        {
            if (org.y + y >= mBufferSize.y || org.y + y < 0)
                break;

            if (isCharValid)
            {
                const u8* src = &cFontBitmap[(c - ' ') * 48 + (static_cast<s32>(cCharSize.y) - y - 1) * 6 + x];

                if (*src != 0)
                {
                    Vector2i pos(org.x + x, org.y + y);
                    putDot_(pos, charFmt, byte);
                }
            }

            if (drawDot)
            {
                Vector2i pos(org.x + x, org.y + y);
                putDot_(pos, bgFmt, byte);
            }
        }
    }
}

void DirectPrintCtr::clear(BoundBox2i const& box)
{
    Vector2i min;
    Vector2i size(box.getSizeX(), box.getSizeY());
    min.setAdd(box.getMin(), box.getMin());
    SEAD_ASSERT_MSG(mStringTop, "Current display buffer is null");
    SEAD_ASSERT(mBufferSize.x > 0 && mBufferSize.y > 0);

    s32 byte = getByteByDot_(mStringByte);

    u8 bgColor[4];
    convertColorFormat_(bgColor, mBGColor, mStringByte);

    Vector2i dotSize;
    dotSize.set(0, cCharSize.y);

    Vector2i bufSize;
    bufSize = min + dotSize;

    convertPositionUserOriginToDeviceOrigin_(&dotSize, bufSize);
    for (s32 x = 0; x < dotSize.x; ++x)
    {
        for (s32 y = 0; y < dotSize.y; ++y)
        {
            Vector2i pos(min.x + x, min.y + y);

            putDot_(pos, bgColor, byte);
        }
    }
}

void DirectPrintCtr::convertColorFormat_(u8* dst, const Color4u8& color, u32 format)
{
    switch (format)
    {
    case GL_RGBA8_OES:
        dst[0] = color.r;
        dst[1] = color.g;
        dst[2] = color.b;
        dst[3] = color.a;
        break;

    case GL_RGB8_OES:
        dst[0] = (color.g & 0xf0) | (color.r >> 4);
        dst[1] = (color.a & 0xf0) | (color.b >> 4);
        break;

    case GL_RGBA4:
        dst[0] = color.g;
        dst[1] = color.b;
        dst[2] = color.a;
        break;

    case GL_RGB5_A1:
        dst[0] = ((color.r & 0x80) >> 7)
               | ((color.g & 0xf8) >> 2)
               | ((color.b & 0x03) << 6);

        dst[1] = (color.a & 0xf8)
               | ((color.b & 0x38) >> 3);
        break;

    case GL_RGB565:
        dst[0] = ((color.b & 0x1c) << 5)
               | (color.g >> 3);

        dst[1] = (color.a & 0xf8)
               | ((color.b & 0xe0) >> 5);
        break;

    default:
        SEAD_ASSERT_MSG(false, "Undefined format.");
    }
}
}
