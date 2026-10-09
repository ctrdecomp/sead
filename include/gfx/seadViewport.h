#pragma once

#include <gfx/seadGraphics.h>
#include <math/seadBoundBox.h>
#include <math/seadVector.h>

namespace sead
{
template <typename T>
class Ray;
class LogicalFrameBuffer;
class Projection;
class Camera;

class Viewport : public BoundBox2f
{
public:
    Viewport();
    Viewport(float left, float top, float right, float bottom);
    explicit Viewport(const BoundBox2f& parent);
    explicit Viewport(const LogicalFrameBuffer& buffer);
    virtual ~Viewport(){ };

    void setByFrameBuffer(const LogicalFrameBuffer& buffer);
    void apply(const LogicalFrameBuffer& buffer) const;
    void apply() const;
    void getOnFrameBufferPos(Vector2f* out, const LogicalFrameBuffer& buffer) const;
    void getOnFrameBufferSize(Vector2f* out, const LogicalFrameBuffer& buffer) const;
    void applyViewport(const LogicalFrameBuffer& buffer) const;
    void applyScissor(const LogicalFrameBuffer& buffer) const;
    void project(Vector2f*, const Vector3f&) const;
    void project(Vector2f*, const Vector2f&) const;
    void unproject(Vector3f*, const Vector2f&, const Projection&, const Camera&) const;
    void unproject(Ray<Vector3f>*, const Vector2f&, const Projection&, const Camera&) const;

    Graphics::DevicePosture getDevicePosture() const{ return mDevicePosture; }

private:
    Graphics::DevicePosture mDevicePosture;
};
}  // namespace sead

