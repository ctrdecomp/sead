#ifndef SEAD_PROJECTION_H_
#define SEAD_PROJECTION_H_

#include <basis/seadTypes.h>
#include <gfx/seadGraphics.h>
#include <geom/seadLine.h>
#include <math/seadBoundBox.h>
#include <math/seadMatrix.h>
#include <math/seadVector.h>
#include <prim/seadRuntimeTypeInfo.h>

namespace sead
{
class Camera;
template <typename T>
class Ray;
class Viewport;

class Projection
{
    SEAD_RTTI_BASE(Projection)

public:
    Projection();
    virtual ~Projection();

    virtual u32 getProjectionType() const = 0;
    virtual void doUpdateMatrix(Matrix44f* mtx) const = 0;
    virtual void doUpdateDeviceMatrix(Matrix44f*, const Matrix44f&, Graphics::DevicePosture) const;
    virtual void doScreenPosToCameraPosTo(Vector3f* camera_pos,
                                          const Vector3f& screen_pos) const = 0;

    const Matrix44f& getProjectionMatrix() const;
    void updateMatrixImpl_() const;
    Matrix44f& getProjectionMatrixMutable();
    const Matrix44f& getDeviceProjectionMatrix() const;
    void cameraPosToScreenPos(Vector3f* screen_pos, const Vector3f& camera_pos) const;
    void screenPosToCameraPos(Vector3f* camera_pos, const Vector3f& screen_pos) const;
    void screenPosToCameraPos(Vector3f* camera_pos, const Vector2f& screen_pos) const;
    void project(Vector2f* dst, const Vector3f& camera_pos, const Viewport& viewport) const;
    void unproject(Vector3f* world_pos, const Vector3f& screen_pos, const Camera& camera) const;
    void unprojectRay(Ray<Vector3f>* dst, const Vector3f& screen_pos, const Camera& camera) const;

    void setDirty() { mDirty = true; }
    void setDeviceDirty() { mDeviceDirty = true; }

    void setDevicePosture(Graphics::DevicePosture pose)
    {
        mDevicePosture = pose;
        setDeviceDirty();
    }

    enum Type
    {
        cPerspective,
        cOrtho,
        cDirect,
        cFrustum
    };

private:
    mutable bool mDirty;
    mutable bool mDeviceDirty;
    Matrix44f mMatrix;
    Matrix44f mDeviceMatrix;
    Graphics::DevicePosture mDevicePosture;
    f32 mDeviceZScale;
    f32 mDeviceZOffset;
};

class PerspectiveProjection : public Projection
{
    SEAD_RTTI_OVERRIDE(PerspectiveProjection, Projection)

public:
    PerspectiveProjection();
    PerspectiveProjection(f32 near, f32 far, f32 fovy_rad, f32 aspect);
    virtual ~PerspectiveProjection();

    virtual u32 getProjectionType() const{ return cPerspective; }
    virtual void doScreenPosToCameraPosTo(Vector3f* cameraPos, const Vector3f& screenPos) const;

    void set(f32 near, f32 far, f32 fovy_rad, f32 aspect);
    virtual void doUpdateMatrix(Matrix44f* mtx) const;
    void setFovx(f32);
    void createDividedProjection(PerspectiveProjection* dst, s32 partnoX, s32 partnoY, s32 divnumX, s32 divnumY) const;
    f32 getTop() const;
    f32 getBottom() const;
    f32 getLeft() const;
    f32 getRight() const;
    void setTBLR(f32 top, f32 bottom, f32 left, f32 right);

    void setNear(f32 near)
    {
        mNear = near;
        setDirty();
    }

    void setFar(f32 far)
    {
        mFar = far;
        setDirty();
    }

    void setFovy(f32 fovyRad)
    {
        setFovy_(fovyRad);
    }

    void setAspect(f32 aspect)
    {
        mAspect = aspect;
        setDirty();
    }

    void setOffset(const Vector2f& offset)
    {
        mOffset = offset;
        setDirty();
    }

    const Vector2f& getOffset() const
    {
        return mOffset;
    }

    f32 getNear() const { return mNear; }
    f32 getFar() const { return mFar; }
    f32 getFovy() const { return mAngle; }
    f32 getAspect() const { return mAspect; }
protected:
    void setFovy_(f32 fovy);

    f32 calcNearClipHeight_() const
    {
        return mNear * 2.0f * mFovyTan;
    }

    f32 calcNearClipWidth_() const
    {
        return calcNearClipHeight_() * mAspect;
    }
private:
    f32 mNear;
    f32 mFar;
    f32 mAngle;
    f32 mFovyRad;
    f32 mFovySin;
    f32 mFovyCos;
    f32 mFovyTan;
    f32 mAspect;
    Vector2f mOffset;
};

class OrthoProjection : public Projection
{
    SEAD_RTTI_OVERRIDE(OrthoProjection, Projection);

public:
    OrthoProjection();
    OrthoProjection(f32 near, f32 far, f32 top, f32 bottom, f32 left, f32 right);
    OrthoProjection(f32 near, f32 far, const BoundBox2f& boundBox);
    OrthoProjection(f32 near, f32 far, const Viewport& viewport);
    virtual ~OrthoProjection();

    virtual u32 getProjectionType() const{ return cOrtho; }
    virtual void doUpdateMatrix(Matrix44f* mtx) const;
    virtual void doScreenPosToCameraPosTo(Vector3f* cameraPos, const Vector3f& screenPos) const;

    void createDividedProjection(OrthoProjection*, s32, s32, s32, s32) const;
    void setBoundBox(const BoundBox2f& boundBox);
    void setByViewport(const Viewport& viewport);
    void setTBLR(f32 top, f32 bottom, f32 left, f32 right);

    void setNear(f32 _near)
    {
        mNear = _near;
        setDirty();
    }

    void setFar(f32 _far)
    {
        mFar = _far;
        setDirty();
    }

    void setTop(f32 top)
    {
        mTop = top;
        setDirty();
    }

    void setBottom(f32 bottom)
    {
        mBottom = bottom;
        setDirty();
    }

    void setLeft(f32 left)
    {
        mLeft = left;
        setDirty();
    }

    void setRight(f32 right)
    {
        mRight = right;
        setDirty();
    }

    f32 getTop() const { return mTop; }
    f32 getBottom() const { return mBottom; }
    f32 getLeft() const { return mLeft; }
    f32 getRight() const { return mRight; }

    f32 getNear() const { return mNear; }
    f32 getFar() const { return mFar; }
    f32 getFovy() const { return 0.0f; }
    f32 getAspect() const { return (mRight - mLeft) / (mTop - mBottom);  }

private:
    f32 mNear;
    f32 mFar;
    f32 mTop;
    f32 mBottom;
    f32 mLeft;
    f32 mRight;
};

class FrustumProjection : public Projection
{
    SEAD_RTTI_OVERRIDE(FrustumProjection, Projection)

public:
    FrustumProjection();
    FrustumProjection(f32 near, f32 far, f32 top, f32 bottom, f32 left, f32 right);
    FrustumProjection(f32 near, f32 far, const BoundBox2f& boundBox);
    virtual ~FrustumProjection();

    f32 getOffsetX() const;
    f32 getOffsetY() const;
    virtual u32 getProjectionType() const{ return cFrustum; }

    virtual void doUpdateMatrix(Matrix44f* mtx) const;
    virtual void doScreenPosToCameraPosTo(Vector3f* cameraPos, const Vector3f& screenPos) const;

    void setTBLR(f32 top, f32 bottom, f32 left, f32 right);

    void setBoundBox(BoundBox2f& boundBox);
    void createDividedProjection(FrustumProjection* out, s32, s32, s32, s32) const;
    void setFovyAspectOffset(f32 fovy, f32 aspect, const Vector2f& offset);

private:
    f32 mNear;
    f32 mFar;
    f32 mTop;
    f32 mBottom;
    f32 mLeft;
    f32 mRight;
};

class DirectProjection : public Projection
{
    SEAD_RTTI_OVERRIDE(DirectProjection, Projection)

public:
    DirectProjection();
    DirectProjection(const Matrix44f& mtx, Graphics::DevicePosture posture);
    virtual ~DirectProjection();
    virtual u32 getProjectionType() const{ return cDirect; }
    virtual void doUpdateMatrix(Matrix44f* mtx) const;
    virtual void doScreenPosToCameraPosTo(Vector3f* cameraPos, const Vector3f& screenPos) const;

    void setProjectionMatrix(const Matrix44f& mtx, Graphics::DevicePosture posture);

private:
    Matrix44f mProjectionMatrix;
};

}  // namespace sead

#endif  // SEAD_PROJECTION_H_

