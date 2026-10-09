#ifndef SEAD_BITMAP_BUILDER_H_
#define SEAD_BITMAP_BUILDER_H_

#include "stream/seadStream.h"

namespace sead
{
class BitmapBuilder
{
public:
    BitmapBuilder(WriteStream* stream, s32 width, s32 height);
    virtual ~BitmapBuilder();

    void writeInfoHeader();
    void writeFileHeader();

    void writeImageLineA1BGR5(void* ptr, u32 width);
    void writeImageLineA1RGB5(void* ptr, u32 width);

    void writeImageLineABGR4(void* ptr, u32 width);
    void writeImageLineABGR8(void* ptr, u32 width);

    void writeImageLineBGR565(void* ptr, u32 width);
    void writeImageLineBGR8(void* ptr, u32 width);

    void writeImageLineRGB565(void* ptr, u32 width);
    void writeImageLineRGB8(void* ptr, u32 width);

    void writeImageLineRGBA4(void* ptr, u32 width);
    void writeImageLineRGBA8(void* ptr, u32 width);
private:
    WriteStream* mWriteStream;
    s32 mWidth;
    s32 mHeight;
};
}

#endif
