#pragma once

#include <math/seadMatrix.h>
#include <math/seadVector.h>
#include <prim/seadRuntimeTypeInfo.h>

namespace sead
{
class OrthoProjection;
class Projection;
class Viewport;
template <typename T>
class Ray;

class Camera
{
    SEAD_RTTI_BASE(Camera)

public:
    Camera(){ }
    virtual ~Camera();
    virtual void doUpdateMatrix(Matrix34f* dst) const = 0;

    void getWorldPosByMatrix(Vector3f* dst) const;
    void getLookVectorByMatrix(Vector3f* dst) const;
    void getRightVectorByMatrix(Vector3f* dst) const;
    void getUpVectorByMatrix(Vector3f* dst) const;

    void worldPosToCameraPosByMatrix(Vector3f* dst, const Vector3f& world_pos) const;
    void cameraPosToWorldPosByMatrix(Vector3f* dst, const Vector3f& camera_pos) const;

    void projectByMatrix(Vector2f* dst, const Vector3f& world_pos, const Projection& projection,
                         const Viewport& viewport) const;
    void unprojectRayByMatrix(Ray<Vector3f>* dst, const Vector3f& camera_pos) const;

    Matrix34f& getMatrix() { return mMatrix; }
    const Matrix34f& getMatrix() const { return mMatrix; }

    void updateViewMatrix() { doUpdateMatrix(&mMatrix); }

    const Matrix34f& getViewMatrix() const
    {
        return mMatrix;
    }

    void updateMatrix()
    {
        doUpdateMatrix(&mMatrix);
    }

    void unprojectByMatrix(Vector3f* dst, const Vector3f& cameraPos) const
    {
        cameraPosToWorldPosByMatrix(dst, cameraPos);
    }

private:
    Matrix34f mMatrix;
};

class LookAtCamera : public Camera
{
    SEAD_RTTI_OVERRIDE(LookAtCamera, Camera)
public:
    LookAtCamera::LookAtCamera(): 
        mPos(0.0f, 0.0f, 10.0f),
        mAt(0.0f, 0.0f, 0.0f),
        mUp(0.0f, 1.0f, 0.0f)
    {
    }

    LookAtCamera(const Vector3f& pos, const Vector3f& at, const Vector3f& up);
    virtual ~LookAtCamera();
    virtual void doUpdateMatrix(Matrix34f* dst) const;

    const Vector3f& getPos() const { return mPos; }
    const Vector3f& getAt() const { return mAt; }
    const Vector3f& getUp() const { return mUp; }

    void setPos(const Vector3f& pos) { mPos = pos; }
    void setAt(const Vector3f& at) { mAt = at; }
    void setUp(const Vector3f& up) { mUp = up; }

    void normalizeUp() { mUp.normalize(); }
    void addPos(const Vector3f& pos) { mPos += pos; }
    void addAt(const Vector3f& at) { mAt += at; }

private:
    Vector3f mPos;
    Vector3f mAt;
    Vector3f mUp;
};

class DirectCamera : public Camera
{
    SEAD_RTTI_OVERRIDE(DirectCamera, Camera)
public:
    DirectCamera(): 
        Camera(), 
        mDirectMatrix(Matrix34f::ident)
    {
    }

    virtual ~DirectCamera();
    virtual void doUpdateMatrix(Matrix34f* dst) const;

    void setViewMatrix(const Matrix34f& matrix)
    {
        mDirectMatrix = matrix;
    }
private:
    Matrix34f mDirectMatrix;
};

class OrthoCamera : public LookAtCamera
{
    SEAD_RTTI_OVERRIDE(OrthoCamera, LookAtCamera)
public:
    OrthoCamera();
    OrthoCamera(const Vector2f&, float);
    OrthoCamera(const OrthoProjection&);
    virtual ~OrthoCamera();

    void setByOrthoProjection(const OrthoProjection&);
    void setRotation(float rotation);
};
}  // namespace sead

