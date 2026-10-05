#ifndef SEAD_PRIMITIVE_RENDERER_CTR_H_
#define SEAD_PRIMITIVE_RENDERER_CTR_H_

#include "gfx/seadPrimitiveRenderer.h"
#include "gfx/seadPrimitiveRendererUtil.h"
#include "gfx/ctr/seadRamCommandCacheCtr.h"
#include <nn/gr.h>

namespace sead
{
class TextureCtr;

class PrimitiveRendererCtr : public PrimitiveRendererBase
{
public:
    enum Mode
    {
        cDraw3D,
        cDrawTexture,
        cDraw2D,
        cDrawMax
    };

    struct Shape : public Vertex
    {
        struct Index
        {
            Index():
                mIndexStream()
            {
            }
            
            u16* mTextureIndex;
            nn::gr::CTR::Vertex::IndexStream mIndexStream;
        };

        Shape();

        Index mShapeIndex;
        Vector3f* mShapePos;
        Vector2f* mShapeUV;
        f32*  mShapeColor;
    };

    PrimitiveRendererCtr(Heap* heap);

    virtual void prepareFromBinaryImpl(Heap* heap, const void* bin_data, u32 bin_size);
    virtual void prepareImpl(Heap* heap, const SafeString& path);
    virtual void setCameraImpl(const Camera& camera);
    virtual void setProjectionImpl(const Projection& projection);
    virtual void beginImpl();
    virtual void endImpl();
    virtual void drawQuadImpl(const Matrix34f& model_mtx, const Color4f& colorL,
                              const Color4f& colorR);
    virtual void drawQuadImpl(const Matrix34f& model_mtx, const Texture& texture,
                              const Color4f& colorL, const Color4f& colorR, const Vector2f& uv_src,
                              const Vector2f& uv_size);
    virtual void drawBoxImpl(const Matrix34f& model_mtx, const Color4f& colorL, const Color4f& colorR);
    virtual void drawCubeImpl(const Matrix34f& model_mtx, const Color4f& c0, const Color4f& c1);
    virtual void drawWireCubeImpl(const Matrix34f& model_mtx, const Color4f& c0, const Color4f& c1);
    virtual void drawLineImpl(const Matrix34f& model_mtx, const Color4f& c0, const Color4f& c1);
    virtual void drawSphere4x8Impl(const Matrix34f& model_mtx, const Color4f& north, const Color4f& south);
    virtual void drawSphere8x16Impl(const Matrix34f& model_mtx, const Color4f& north, const Color4f& south);
    virtual void drawDisk16Impl(const Matrix34f& model_mtx, const Color4f& center, const Color4f& edge);
    virtual void drawDisk32Impl(const Matrix34f& model_mtx, const Color4f& center, const Color4f& edge);
    virtual void drawCircle16Impl(const Matrix34f& model_mtx, const Color4f& edge);
    virtual void drawCircle32Impl(const Matrix34f& model_mtx, const Color4f& edge);
    virtual void drawCylinder16Impl(const Matrix34f& model_mtx, const Color4f& top, const Color4f& btm);
    virtual void drawCylinder32Impl(const Matrix34f& model_mtx, const Color4f& top, const Color4f& btm);
    virtual ~PrimitiveRendererCtr();
protected:
    void copyAndSetupVtx_(PrimitiveRendererCtr::Shape* shape, PrimitiveRendererUtil::Vertex const* vtx, size_t size3D, size_t size2D);
    void createBuffer_(Heap* heap, Shape* shape, size_t size3D, size_t size2D);
    void setup_(Mode mode, TextureCtr const* pTexture);
    void loadBoxIndex_(Heap* heap);
    void loadCircleIndex_(Heap* heap, Shape::Index* index, s32 size);
    void loadCubeVertex_(Heap* heap);
    void loadCylinderVertex_(Heap* heap, Shape* shape, s32 size);
    void loadDiskVertex_(Heap* heap, Shape* index, s32 size);
    void loadLineVertex_(Heap* heap);
    void loadQuadVertex_(Heap* heap);
    void loadSphereVertex_(Heap* heap, Shape* index, s32 height, s32 width);
    void loadWireCubeIndex_(Heap* heap);
    void drawShape_(const Shape& shape);
    void drawShape_(const nn::gr::CTR::Vertex& vert, const nn::gr::CTR::Vertex::IndexStream& vertIndex);
    void checkCmdlist_();


private:
    RawCommandCacheCtr mCache3D;
    BindSymbolVSFloat mSymbolWVP_3D;
    BindSymbolVSFloat mSymbolUser_3D;
    BindSymbolVSFloat mSymbolColor0_3D;
    BindSymbolVSFloat mSymbolColor1_3D;
    BindSymbolVSFloat mSymbolUvSrc_3D;
    BindSymbolVSFloat mSymbolUvSize_3D;
    BindSymbolVSInput mAttrVertexLoc_3D;
    BindSymbolVSInput mAttrTexCoord0Loc_3D;
    BindSymbolVSInput mAttrColorRateLoc_3D;

    /* CTR Device VRAM-B*/

    RawCommandCacheCtr mCache2D;
    BindSymbolVSFloat mSymbolWVP_2D;
    BindSymbolVSFloat mSymbolUser_2D;
    BindSymbolVSFloat mSymbolColor0_2D;
    BindSymbolVSFloat mSymbolColor1_2D;
    BindSymbolVSFloat mSymbolUvSrc_2D;
    BindSymbolVSFloat mSymbolUvSize_2D;
    BindSymbolVSInput mAttrVertexLoc_2D;
    BindSymbolVSInput mAttrTexCoord0Loc_2D;
    BindSymbolVSInput mAttrColorRateLoc_2D;

    /* dmp_Line.width */

    BindSymbolGSFloat mDmpLineWidth;

    /* Texture */
    
    nn::gr::CTR::Texture mTexture3D;
    nn::gr::CTR::Texture mTexture2D;

    /* Combiners */

    Combiner mCombiner3D;
    Combiner mCombiner2D;

    /* Utils */

    Mode mMode;
    const TextureCtr* mCtrTexture;

    /* Shapes */
    
    Shape mBox;
    Shape mCube;
    Shape mLine;
    Shape mSphere4x8; 
    Shape mSphere8x16;
    Shape mDisk16;
    Shape mDisk32;
    Shape::Index mWireCubeIndex; 
    Shape::Index mBoxIndex; 
    Shape::Index mCircle16Index; 
    Shape::Index mCircle32Index;
    Shape mCylinder16;
    Shape mCylinder32;

    /* Vertex Util */

    const Vertex* mCurrentVertex;
    u32* mListCommand;

    /* Matrixes */

    Matrix34f mViewMtx;
    Matrix44f mProjectionMtx;

    void* mCmdlistBufSize;
};
}

#endif