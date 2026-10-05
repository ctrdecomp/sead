#include "gfx/seadTextureUtil.h"
#include "gfx/seadFrameBuffer.h"
#include "gfx/seadTexture.h"

namespace sead
{
void TextureUtil::copyOutFrameBufferToTexture(Texture* texture, const FrameBuffer* frameBuffer)
{
    BoundBox2f box;
    box.setFromCenterAndXY(Vector2f::zero, texture->getWidth(), texture->getHeight());
    copyOutFrameBufferToTexture(texture, frameBuffer, box, Vector2f::zero);
}

void TextureUtil::copyOutFrameBufferToTexture(Texture* texture, const FrameBuffer* frameBuffer, const BoundBox2f& texX, const Vector2f& texY)
{
    SEAD_ASSERT_MSG(false, "Not implement.");
}
}