#include <gfx/seadViewport.h>
#include <gfx/seadFrameBuffer.h>
#include <gfx/seadProjection.h>

namespace sead 
{
Viewport::Viewport(const LogicalFrameBuffer& frame_buffer): 
    mDevicePosture(Graphics::getDefaultDevicePosture())
{
    setByFrameBuffer(frame_buffer);
}

void Viewport::setByFrameBuffer(const LogicalFrameBuffer& frame_buffer)
{
    switch (mDevicePosture)
    {
    case Graphics::cDevicePosture_Same:
    case Graphics::cDevicePosture_FlipX:
    case Graphics::cDevicePosture_FlipY:
    case Graphics::cDevicePosture_FlipXY:
        set(0.0f, 0.0f, frame_buffer.getVirtualSize().x, frame_buffer.getVirtualSize().y);
        break;
    case Graphics::cDevicePosture_RotateRight:
    case Graphics::cDevicePosture_RotateLeft:
        set(0.0f, 0.0f, frame_buffer.getVirtualSize().y, frame_buffer.getVirtualSize().x);
        break;
    default:
        SEAD_ASSERT_MSG(false, "Undefined DevicePosture(%d)", (s32)mDevicePosture);
    }
}

void Viewport::getOnFrameBufferPos(Vector2f* dst, const LogicalFrameBuffer& fb) const
{
    *dst = getMin();

    switch (mDevicePosture)
    {
    case Graphics::cDevicePosture_Same:
        break;
    case Graphics::cDevicePosture_RotateRight:
        {
            f32 y = (fb.getVirtualSize().y - getSizeX()) - dst->x;
            dst->set(dst->y, y);
        }
        break;
    case Graphics::cDevicePosture_RotateLeft:
        {
            f32 x = (fb.getVirtualSize().x - getSizeY()) - dst->y;
            dst->set(x, dst->x);
        }
        break;
    case Graphics::cDevicePosture_FlipXY:
        {
            f32 y = (fb.getVirtualSize().y - getSizeY()) - dst->y;
            f32 x = (fb.getVirtualSize().x - getSizeX()) - dst->x;
            dst->set(x, y);
        }
        break;
    case Graphics::cDevicePosture_FlipX:
        {
            f32 x = (fb.getVirtualSize().x - getSizeX()) - dst->x;
            dst->set(x, dst->y);
        }
        break;
    case Graphics::cDevicePosture_FlipY:
        {
            f32 y = (fb.getVirtualSize().y - getSizeY()) - dst->y;
            dst->set(dst->x, y);
        }
        break;
    default:
        SEAD_ASSERT_MSG(false, "Undefined DevicePosture(%d)", s32(mDevicePosture));
    }

    dst->div(fb.getVirtualSize());
    dst->x *= fb.getPhysicalArea().getSizeX();
    dst->y *= fb.getPhysicalArea().getSizeY();
    dst->add(fb.getPhysicalArea().getMin());
}

void Viewport::getOnFrameBufferSize(Vector2f* dst, const LogicalFrameBuffer& fb) const
{
    dst->set(getSizeX(), getSizeY());

    switch (mDevicePosture)
    {
    case Graphics::cDevicePosture_Same:
    case Graphics::cDevicePosture_FlipX:
    case Graphics::cDevicePosture_FlipY:
    case Graphics::cDevicePosture_FlipXY:
        break;
    case Graphics::cDevicePosture_RotateRight:
    case Graphics::cDevicePosture_RotateLeft:
        dst->set(dst->y, dst->x);
        break;
    default:
        SEAD_ASSERT_MSG(false, "Undefined DevicePosture(%d)", s32(mDevicePosture));
    }

    dst->div(fb.getVirtualSize());
    dst->x *= fb.getPhysicalArea().getSizeX();
    dst->y *= fb.getPhysicalArea().getSizeY();
}

void Viewport::apply(const LogicalFrameBuffer& frame_buffer) const
{
    Vector2f real_pos;
    getOnFrameBufferPos(&real_pos, frame_buffer);

    Vector2f real_size;
    getOnFrameBufferSize(&real_size, frame_buffer);

    SEAD_ASSERT(frame_buffer.getPhysicalArea().isInside(real_pos) && frame_buffer.getPhysicalArea().isInside(real_pos + real_size));

    real_pos.y = (frame_buffer.getPhysicalArea().getSizeY() - real_size.y) - real_pos.y;

    Graphics::instance()->setViewportRealPosition(real_pos.x, real_pos.y, real_size.x, real_size.y);
    Graphics::instance()->setScissorRealPosition(real_pos.x, real_pos.y, real_size.x, real_size.y);
}

void Viewport::project(Vector2f* dst, const Vector3f& screenPos) const
{
    Vector2f center = getCenter();

    dst->x = getHalfSizeX() * screenPos.x;
    dst->y = getHalfSizeY() * screenPos.y;
}

void Viewport::project(Vector2f* dst, const Vector2f& screenPos) const
{
    Vector2f center = getCenter();

    dst->x = getHalfSizeX() * screenPos.x;
    dst->y = getHalfSizeY() * screenPos.y;
}

void Viewport::unproject(Vector3f* dst, const Vector2f& canvasPos, const Projection& projection, const Camera& camera) const
{
    Vector3f screenPos;
    screenPos.x = canvasPos.x / getHalfSizeX();
    screenPos.y = canvasPos.y / getHalfSizeY();
    screenPos.z = 0.0f;

    projection.unproject(dst, screenPos, camera);
}
}  // namespace sead
