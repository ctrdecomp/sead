#pragma once

#include <math/seadVector.h>
#include <math/seadBoundBox.h>
#include <gfx/seadColor.h>
#include <prim/seadSafeString.h>

#include <cstdlib>

namespace sead
{
class DirectPrintCtr
{
public:
    DirectPrintCtr();
    virtual ~DirectPrintCtr();

    void changeDisplayBuffer(void* stringTop);
    void changeDisplayBuffer(void* stringTop, u32 byte, Vector2i const& bufSize, BoundBox2i const& box);
    void checkBufferIsNotOnVRAM_();
    void clear(BoundBox2i const& box);
    void convertColorFormat_(unsigned char* fmt, Color4u8 const& color, u32 byte);
    void convertPositionUserOriginToDeviceOrigin_(Vector2i* org, Vector2i const& bufSize);
    void flush();
    void printf(Vector2i const& bufSize, const char* msg, ...);
    void printf(Vector2i const& bufSize, bool autoWrap, bool drawDot, const char* msg,  ...);
    void putChar(Vector2i const& bufSize, bool drawDot, char c);
    void putChar(Vector2i const& bufSize, char c);
    void* putDot_(Vector2i const& bufSize, const u8* src, s32 size);

    void putString(Vector2i const& bufSize, bool autoWrap, bool drawDot, SafeString const& string);
    void putString(Vector2i const& bufSize, SafeString const& string);

    void vprintf(Vector2i const& bufSize, SafeString const& msg, std::__va_list list);
    void vprintf(Vector2i const& bufSize, bool autoWrap, bool drawDot, SafeString const& msg, std::__va_list list);

    BoundBox2i getBoundBox() const{ return mDispBox; }
    Color4u8 getBGColor() const{ return mBGColor; }
    Color4u8 getCharColor() const{ return mCharColor; }
    Vector2i getLastCursorPos() const{ return mCursorPos; }
    void setCharColor(const Color4u8& color){ mCharColor = color; }
private:
    void* mStringTop;
    u32 mStringByte;
    Vector2i mBufferSize;
    BoundBox2i mDispBox;
    Color4u8 mBGColor;
    Color4u8 mCharColor;
    Vector2i mCursorPos;
    u8 mCharSize;
};
}
