#ifndef SEAD_TEXTURE_UTIL_H_
#define SEAD_TEXTURE_UTIL_H_

#include <nn/gx.h>
#include <math/seadBoundBox.h>
#include <math/seadVector.h>

namespace sead {

class Heap;
class ReadStream;

class Texture;
class FrameBuffer;

class TextureUtil
{
public:
    static void copyOutFrameBufferToTexture(Texture* texture, const FrameBuffer* frameBuffer);
    static void copyOutFrameBufferToTexture(Texture* texture, const FrameBuffer* frameBuffer, const BoundBox2f& texX, const Vector2f& texY);
};

}

#endif
