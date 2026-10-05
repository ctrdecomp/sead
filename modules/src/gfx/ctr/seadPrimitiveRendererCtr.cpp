// Filename: seadPrimitiveRendererCtr.cpp
//
// Project: StandardEAD C++ Library for CTR

#include "gfx/ctr/seadPrimitiveRendererCtr.h"
#include "gfx/ctr/seadGraphicsCtr.h"
#include "gfx/ctr/seadTextureCtr.h"
#include "gfx/seadCamera.h"
#include "gfx/seadColor.h"
#include "gfx/seadProjection.h"
#include "filedevice/seadFileDeviceMgr.h"
#include "filedevice/seadFileDevice.h"

#include <nn/gx/CTR/gx_CommandAccess.h>

namespace
{
u32* makeUniformCommand_(u32* command, u32 symbol, sead::Color4f const& color)
{
    *command++ = symbol | 0x80000000;
    *command++ = 0x804F02C0;

    *command++ = nn::math::F32AsU32(color.cl.r);
    *command++ = nn::math::F32AsU32(color.cl.g);
    *command++ = nn::math::F32AsU32(color.cl.b);
    *command++ = nn::math::F32AsU32(color.cl.a);

    return command;
}
}

namespace sead
{
PrimitiveRendererCtr::Shape::Shape():
    Vertex(),
    mShapeIndex()
{
}

PrimitiveRendererCtr::PrimitiveRendererCtr(Heap* /* heap */):
    mCache3D(),
    mSymbolWVP_3D(),
    mSymbolUser_3D(),
    mSymbolColor0_3D(), 
    mSymbolColor1_3D(),
    mSymbolUvSrc_3D(), 
    mSymbolUvSize_3D(),
    mAttrVertexLoc_3D(),
    mAttrTexCoord0Loc_3D(),
    mAttrColorRateLoc_3D(),
    mCache2D(),
    mSymbolWVP_2D(),
    mSymbolUser_2D(),
    mSymbolColor0_2D(), 
    mSymbolColor1_2D(),
    mSymbolUvSrc_2D(), 
    mSymbolUvSize_2D(),
    mAttrVertexLoc_2D(),
    mAttrTexCoord0Loc_2D(),
    mAttrColorRateLoc_2D(),
    mDmpLineWidth(),

    mTexture3D(), mTexture2D(),
    mCombiner3D(), mCombiner2D(),

    mMode(cDrawMax),
    mBox(),
    mCube(),
    mLine(),
    mSphere4x8(),
    mSphere8x16(),
    mDisk16(),
    mDisk32(),
    mWireCubeIndex(),
    mBoxIndex(),
    mCircle16Index(),
    mCircle32Index(),
    mCylinder16(),
    mCylinder32(),

    mViewMtx(),
    mProjectionMtx()
{
}

PrimitiveRendererCtr::~PrimitiveRendererCtr()
{
}

void PrimitiveRendererCtr::prepareFromBinaryImpl(Heap* heap, const void* bin_data, u32 bin_size)
{
    /* Setup Global Shader Object */

    Shader* shader = const_cast<Shader*>(reinterpret_cast<const Shader*>(bin_data));
    new(shader) Shader();

    /* Setup 3D */

    shader->SetupBinary(bin_data, 0, -1);
    bool result = shader->SearchBindSymbol(&mSymbolWVP_3D, "wvp");
    SEAD_ASSERT(result);
    result = shader->SearchBindSymbol(&mSymbolUser_3D, "user");
    SEAD_ASSERT(result);
    result = shader->SearchBindSymbol(&mSymbolColor0_3D, "color0");
    SEAD_ASSERT(result);
    result = shader->SearchBindSymbol(&mSymbolColor1_3D, "color1");
    SEAD_ASSERT(result);
    result = shader->SearchBindSymbol(&mSymbolUvSrc_3D, "uv_src");
    SEAD_ASSERT(result);
    result = shader->SearchBindSymbol(&mSymbolUvSize_3D, "uv_size");
    SEAD_ASSERT(result);
    result = shader->SearchBindSymbol(&mAttrVertexLoc_3D, "Vertex");
    SEAD_ASSERT(result);
    result = shader->SearchBindSymbol(&mAttrTexCoord0Loc_3D, "TexCoord0");
    SEAD_ASSERT(result);
    result = shader->SearchBindSymbol(&mAttrColorRateLoc_3D, "ColorRate");
    SEAD_ASSERT(result);

    /* Cache Ram for 3D */

    mCache3D.initialize(heap, 0);
    u32* command = reinterpret_cast<u32*>(mCache3D.getTopPtr());
    size_t cache = *shader->MakeFullCommand(command);
    mCache3D.adjust(heap, cache);
    
    /* Setup 2D */

    shader->SetupBinary(bin_data, 0, -1);
    result = shader->SearchBindSymbol(&mSymbolWVP_2D, "wvp");
    SEAD_ASSERT(result);
    result = shader->SearchBindSymbol(&mSymbolUser_2D, "user");
    SEAD_ASSERT(result);
    result = shader->SearchBindSymbol(&mSymbolColor0_2D, "color0");
    SEAD_ASSERT(result);
    result = shader->SearchBindSymbol(&mSymbolColor1_2D, "color1");
    SEAD_ASSERT(result);
    result = shader->SearchBindSymbol(&mSymbolUvSrc_2D, "uv_src");
    SEAD_ASSERT(result);
    result = shader->SearchBindSymbol(&mSymbolUvSize_2D, "uv_size");
    SEAD_ASSERT(result);
    result = shader->SearchBindSymbol(&mAttrVertexLoc_2D, "Vertex");
    SEAD_ASSERT(result);
    result = shader->SearchBindSymbol(&mAttrTexCoord0Loc_2D, "TexCoord0");
    SEAD_ASSERT(result);
    result = shader->SearchBindSymbol(&mAttrColorRateLoc_2D, "ColorRate");
    SEAD_ASSERT(result);
    result = shader->SearchBindSymbol(&mDmpLineWidth, "dmp_Line.width");
    SEAD_ASSERT(result);

    /* Cache Ram for 2D */

    mCache2D.initialize(heap, 0);
    command = reinterpret_cast<u32*>(mCache2D.getTopPtr());
    cache = *shader->MakeFullCommand(command);
    mCache2D.adjust(heap, cache);

    loadQuadVertex_(heap);
    loadLineVertex_(heap);
    loadBoxIndex_(heap);
    loadWireCubeIndex_(heap);

    loadSphereVertex_(heap, &mSphere4x8, 8, 4);
    loadSphereVertex_(heap, &mSphere8x16, 16, 8);

    loadDiskVertex_(heap, &mDisk16, 16);
    loadDiskVertex_(heap, &mDisk32, 32);

    loadCircleIndex_(heap, &mCircle16Index, 16);
    loadCircleIndex_(heap, &mCircle32Index, 32);

    loadCylinderVertex_(heap, &mCylinder16, 16);
    loadCylinderVertex_(heap, &mCylinder32, 32);

    mTexture3D.unit0.texType   = PICA_DATA_TEXTURE0_SAMPLER_TYPE_FALSE;
    mTexture3D.unit0.minFilter = PICA_DATA_TEXTURE_MIN_FILTER_LINEAR;
    mTexture3D.unit0.magFilter = PICA_DATA_TEXTURE_MAG_FILTER_LINEAR;
    mTexture2D.unit0.texType   = PICA_DATA_TEXTURE0_SAMPLER_TYPE_TEXTURE_2D;

    mCombiner3D.stage->SetupPrimary();
    mCombiner2D.stage->SetupPrimaryModulateTexture0();
}

void PrimitiveRendererCtr::prepareImpl(Heap* heap, SafeString const& path)
{
    {
        ScopedCurrentHeapSetter chs(heap);
        FileDevice::LoadArg arg;
        arg.path = path;
        arg.alignment = 32;
        arg.heap = heap;

        const void* file = FileDeviceMgr::instance()->tryLoad(arg);
        prepareFromBinaryImpl(heap, file, arg.read_size);
    }
}

void PrimitiveRendererCtr::setCameraImpl(const Camera& camera)
{
    mViewMtx = camera.getViewMatrix();
}

void PrimitiveRendererCtr::setProjectionImpl(const Projection& projection)
{
    mViewMtx = projection.getDeviceProjectionMatrix();
}

void PrimitiveRendererCtr::beginImpl()
{
    mMode = cDrawMax;
    mCurrentVertex = NULL;
    mCtrTexture = nullptr;

    nngxGetCmdlistParameteri(NN_GX_CMDLIST_CURRENT_BUFADDR, (GLint*)mListCommand);
    mListCommand[1] = mListCommand[0];
    mListCommand = nn::gr::CTR::Vertex::MakeDisableCommand(mListCommand);
    s32 ctop;
    nngxGetCmdlistParameteri(NN_GX_CMDLIST_TOP_BUFADDR, &ctop);
    s32 caddr = 0;
    nngxGetCmdlistParameteri(NN_GX_CMDLIST_CURRENT_BUFADDR, &caddr);
    mCmdlistBufSize = PtrUtil::addOffset(reinterpret_cast<const void*>(ctop), caddr);
}

void PrimitiveRendererCtr::endImpl()
{
    nngxMoveCommandbufferPointer((mListCommand[0] - mListCommand[1] >> 2) << 2);
}

void PrimitiveRendererCtr::drawQuadImpl(const Matrix34f& model_mtx, const Color4f& colorL, const Color4f& colorR)
{
    setup_(cDraw3D, nullptr);
    mListCommand = mSymbolUser_3D.MakeUniformCommand(mListCommand, model_mtx);
    mListCommand = makeUniformCommand_(mListCommand, mSymbolColor0_3D.symbolType, colorL);
    mListCommand = makeUniformCommand_(mListCommand, mSymbolColor1_3D.symbolType, colorR);
    drawShape_(mSphere4x8);
}

void PrimitiveRendererCtr::drawQuadImpl(const Matrix34f& model_mtx, Texture const& texture, const Color4f& colorL, const Color4f& colorR,
    const Vector2f& uv_src, const Vector2f& uv_size)
{
    const TextureCtr* ctrTex = DynamicCast<TextureCtr const>(&texture);
    setup_(cDrawTexture, nullptr);
    mListCommand = mSymbolUser_3D.MakeUniformCommand(mListCommand, model_mtx);
    mListCommand = makeUniformCommand_(mListCommand, mSymbolColor0_3D.symbolType, colorL);
    mListCommand = makeUniformCommand_(mListCommand, mSymbolColor1_3D.symbolType, colorR);

    mListCommand = mSymbolUvSrc_3D.MakeUniformCommand(mListCommand, Vector4f(colorL.cl.r, colorL.cl.g, 0.0f, 0.0f));
    mListCommand = mSymbolUvSize_3D.MakeUniformCommand(mListCommand, Vector4f(colorR.cl.r, colorR.cl.g, 0.0f, 0.0f));

    drawShape_(mBox);
}

void PrimitiveRendererCtr::drawBoxImpl(const Matrix34f& model_mtx, const Color4f& colorL, const Color4f& colorR)
{
    setup_(cDraw2D, nullptr);
    mListCommand = mSymbolUser_3D.MakeUniformCommand(mListCommand, model_mtx);
    mListCommand = makeUniformCommand_(mListCommand, mSymbolColor0_2D.symbolType, colorL);
    mListCommand = makeUniformCommand_(mListCommand, mSymbolColor1_2D.symbolType, colorR);
    drawShape_(mBox);
}

void PrimitiveRendererCtr::drawCubeImpl(const Matrix34f& model_mtx, const Color4f& c0, const Color4f& c1)
{
    setup_(cDraw3D, nullptr);
    mListCommand = mSymbolUser_3D.MakeUniformCommand(mListCommand, model_mtx);
    mListCommand = makeUniformCommand_(mListCommand, mSymbolColor0_3D.symbolType, c0);
    mListCommand = makeUniformCommand_(mListCommand, mSymbolColor1_3D.symbolType, c1);
    drawShape_(mBox);
}

void PrimitiveRendererCtr::drawWireCubeImpl(const Matrix34f& model_mtx, const Color4f& c0, const Color4f& c1)
{
    setup_(cDraw2D, nullptr);
    mListCommand = mSymbolUser_3D.MakeUniformCommand(mListCommand, model_mtx);
    mListCommand = makeUniformCommand_(mListCommand, mSymbolColor0_3D.symbolType, c0);
    mListCommand = makeUniformCommand_(mListCommand, mSymbolColor1_3D.symbolType, c1);
    drawShape_(mBox);
}

void PrimitiveRendererCtr::drawLineImpl(const Matrix34f& model_mtx, const Color4f& c0, const Color4f& c1)
{
    setup_(cDraw2D, nullptr);
    mListCommand = mSymbolUser_3D.MakeUniformCommand(mListCommand, model_mtx);
    mListCommand = makeUniformCommand_(mListCommand, mSymbolColor0_2D.symbolType, c0);
    mListCommand = makeUniformCommand_(mListCommand, mSymbolColor1_2D.symbolType, c1);
    drawShape_(mLine);
}

void PrimitiveRendererCtr::drawSphere4x8Impl(const Matrix34f& model_mtx, const Color4f& north, const Color4f& south)
{
    setup_(cDraw3D, nullptr);
    mListCommand = mSymbolUser_3D.MakeUniformCommand(mListCommand, model_mtx);
    mListCommand = makeUniformCommand_(mListCommand, mSymbolColor0_3D.symbolType, north);
    mListCommand = makeUniformCommand_(mListCommand, mSymbolColor0_3D.symbolType, south);
    drawShape_(mSphere4x8);
}

void PrimitiveRendererCtr::drawSphere8x16Impl(const Matrix34f& model_mtx, const Color4f& north, const Color4f& south)
{
    setup_(cDraw3D, nullptr);
    mListCommand = mSymbolUser_3D.MakeUniformCommand(mListCommand, model_mtx);
    mListCommand = makeUniformCommand_(mListCommand, mSymbolColor0_3D.symbolType, north);
    mListCommand = makeUniformCommand_(mListCommand, mSymbolColor1_3D.symbolType, south);
    drawShape_(mSphere8x16);
}

void PrimitiveRendererCtr::drawDisk16Impl(const Matrix34f& model_mtx, const Color4f& center, const Color4f& edge)
{
    setup_(cDraw3D, nullptr);
    mListCommand = mSymbolUser_3D.MakeUniformCommand(mListCommand, model_mtx);
    mListCommand = makeUniformCommand_(mListCommand, mSymbolColor0_3D.symbolType, center);
    mListCommand = makeUniformCommand_(mListCommand, mSymbolColor1_3D.symbolType, edge);
    drawShape_(mDisk16);
}

void PrimitiveRendererCtr::drawDisk32Impl(const Matrix34f& model_mtx, const Color4f& center, const Color4f& edge)
{
    setup_(cDraw3D, nullptr);
    mListCommand = mSymbolUser_3D.MakeUniformCommand(mListCommand, model_mtx);
    mListCommand = makeUniformCommand_(mListCommand, mSymbolColor0_3D.symbolType, center);
    mListCommand = makeUniformCommand_(mListCommand, mSymbolColor1_3D.symbolType, edge);
    drawShape_(mDisk32);
}

void PrimitiveRendererCtr::drawCircle16Impl(const Matrix34f& model_mtx, const Color4f& edge)
{
    setup_(cDraw2D, nullptr);
    mListCommand = mSymbolUser_3D.MakeUniformCommand(mListCommand, model_mtx);
    mListCommand = makeUniformCommand_(mListCommand, mSymbolColor0_2D.symbolType, edge);
    mListCommand = makeUniformCommand_(mListCommand, mSymbolColor1_2D.symbolType, edge);
    drawShape_(mDisk16, mCircle16Index.mIndexStream);
}

void PrimitiveRendererCtr::drawCircle32Impl(const Matrix34f& model_mtx, const Color4f& edge)
{
    setup_(cDraw2D, nullptr);
    mListCommand = mSymbolUser_3D.MakeUniformCommand(mListCommand, model_mtx);
    mListCommand = makeUniformCommand_(mListCommand, mSymbolColor0_2D.symbolType, edge);
    mListCommand = makeUniformCommand_(mListCommand, mSymbolColor1_2D.symbolType, edge);
    drawShape_(mDisk32, mCircle32Index.mIndexStream);
}

void PrimitiveRendererCtr::drawCylinder16Impl(const Matrix34f& model_mtx, const Color4f& top, const Color4f& btm)
{
    setup_(cDraw3D, nullptr);
    mListCommand = mSymbolUser_3D.MakeUniformCommand(mListCommand, model_mtx);
    mListCommand = makeUniformCommand_(mListCommand, mSymbolColor0_3D.symbolType, top);
    mListCommand = makeUniformCommand_(mListCommand, mSymbolColor1_3D.symbolType, btm);
    drawShape_(mCylinder16);
}

void PrimitiveRendererCtr::drawCylinder32Impl(const Matrix34f& model_mtx, const Color4f& top, const Color4f& btm)
{
    setup_(cDraw3D, nullptr);
    mListCommand = mSymbolUser_3D.MakeUniformCommand(mListCommand, model_mtx);
    mListCommand = makeUniformCommand_(mListCommand, mSymbolColor0_3D.symbolType, top);
    mListCommand = makeUniformCommand_(mListCommand, mSymbolColor1_3D.symbolType, btm);
    drawShape_(mCylinder32);
}

void PrimitiveRendererCtr::copyAndSetupVtx_(PrimitiveRendererCtr::Shape* shape, PrimitiveRendererUtil::Vertex const* vtx, size_t size3D, size_t size2D)
{
    for(u32 i = 0; i < size3D; i++)
    {
        shape->mShapePos[i]   = vtx[i].pos;
        shape->mShapeUV[i]    = vtx[i].uv;
        shape->mShapeColor[i] = vtx->color.c.c[i];
    }

    nngxUpdateBuffer(&shape->mShapePos,   size3D * sizeof(Vector3f));
    nngxUpdateBuffer(&shape->mShapeUV,    size3D * sizeof(Vector2f));
    nngxUpdateBuffer(&shape->mShapeColor, size3D * sizeof(Color4f));
    nngxUpdateBuffer(&shape->mShapeIndex.mTextureIndex, size2D * sizeof(ushort));

    shape->EnableAttrAsArray(mAttrVertexLoc_3D, nngxGetPhysicalAddr(reinterpret_cast<u32>(&shape->mShapePos)), PICA_DATA_SIZE_3_FLOAT);
    shape->EnableAttrAsArray(mAttrTexCoord0Loc_3D, nngxGetPhysicalAddr(reinterpret_cast<u32>(&shape->mShapeUV)), PICA_DATA_SIZE_2_FLOAT);
    shape->EnableAttrAsArray(mAttrColorRateLoc_3D, nngxGetPhysicalAddr(reinterpret_cast<u32>(&shape->mShapeColor)),  PICA_DATA_SIZE_1_FLOAT);

    shape->mShapeIndex.mIndexStream.physicalAddr  = nngxGetPhysicalAddr(reinterpret_cast<u32>(&shape->mShapeColor));
    shape->mShapeIndex.mIndexStream.drawVtxNum    = size2D;
    shape->mShapeIndex.mIndexStream.isUnsignedByte = false;
}

void PrimitiveRendererCtr::createBuffer_(Heap* heap, Shape* shape, size_t size3D, size_t size2D)
{
    shape->mShapePos = new(heap)   Vector3f[size3D];
    shape->mShapeUV = new(heap)    Vector2f[size3D];
    shape->mShapeColor = new(heap) float[size3D];
    shape->mShapeIndex.mTextureIndex = new u16[size2D];
}

void PrimitiveRendererCtr::setup_(Mode mode, const TextureCtr* pTexture)
{

}

void PrimitiveRendererCtr::loadBoxIndex_(Heap* heap)
{
    mBoxIndex.mTextureIndex = new u16[5];

    mBoxIndex.mTextureIndex[0] = 0;
    mBoxIndex.mTextureIndex[1] = 1;
    mBoxIndex.mTextureIndex[2] = 3;
    mBoxIndex.mTextureIndex[3] = 2;
    mBoxIndex.mTextureIndex[4] = 0;

    mBoxIndex.mIndexStream.physicalAddr = nngxGetPhysicalAddr(reinterpret_cast<u32>(mBoxIndex.mTextureIndex));
    mBoxIndex.mIndexStream.drawVtxNum = 5;
    mBoxIndex.mIndexStream.isUnsignedByte = false;
}

void PrimitiveRendererCtr::loadCircleIndex_(Heap* heap, Shape::Index* index, s32 size)
{
    index->mTextureIndex = new u16[size + 1];

    for (s32 i = 0; i < size; i++)
    {
        index->mTextureIndex[i] = i;
    }

    index->mTextureIndex[size] = 0;

    index->mIndexStream.physicalAddr = nngxGetPhysicalAddr(reinterpret_cast<u32>(index->mTextureIndex));

    index->mIndexStream.drawVtxNum = size + 1;
    index->mIndexStream.isUnsignedByte = false;
}

void PrimitiveRendererCtr::loadCubeVertex_(Heap* heap)
{
    createBuffer_(heap, &mCube, 8, 36);
    PrimitiveRendererUtil::Vertex vtx[2];
    PrimitiveRendererUtil::setCubeVertex(vtx, mCube.mShapeIndex.mTextureIndex);
    copyAndSetupVtx_(&mCube, vtx, 8, 36);
}

void PrimitiveRendererCtr::loadCylinderVertex_(Heap* heap, Shape* shape, s32 size)
{
    s32 vert = PrimitiveRendererUtil::calcCylinderVertexNum(size);
    s32 index = PrimitiveRendererUtil::calcCylinderIndexNum(size);
    createBuffer_(heap, shape, vert, index);
    PrimitiveRendererUtil::Vertex vtx[66];
    PrimitiveRendererUtil::setCylinderVertex(vtx, shape->mShapeIndex.mTextureIndex, size);
    copyAndSetupVtx_(shape, vtx, vert, index);
}

void PrimitiveRendererCtr::loadDiskVertex_(Heap* heap, Shape* shape, s32 size)
{
    s32 vert = PrimitiveRendererUtil::calcDiskVertexNum(size);
    s32 index = PrimitiveRendererUtil::calcDiskIndexNum(size);
    createBuffer_(heap, shape, vert, index);
    PrimitiveRendererUtil::Vertex vtx[66];
    PrimitiveRendererUtil::setDiskVertex(vtx, shape->mShapeIndex.mTextureIndex, size);
    copyAndSetupVtx_(shape, vtx, vert, index);
}

void PrimitiveRendererCtr::loadLineVertex_(Heap* heap)
{
    createBuffer_(heap, &mLine, 2, 2);
    mLine.mShapePos->set(0xbf000000, 0, 0);
    mLine.mShapePos[1].set(0x3f000000, 0, 0);
    mLine.mShapeUV->set(0, 0x3f000000);
    mLine.mShapeUV[1].set(0x3f800000, 0x3f000000);

    mLine.mShapeColor = NULL;
    mLine.mShapeColor[1] = 0x3f800000;

    mLine.mShapeIndex.mTextureIndex = 0;
    mLine.mShapeIndex.mTextureIndex[1] = 0;

    mLine.EnableAttrAsArray(mAttrVertexLoc_3D, nngxGetPhysicalAddr(reinterpret_cast<u32>(&mLine.mShapePos)), PICA_DATA_SIZE_3_FLOAT);
    mLine.EnableAttrAsArray(mAttrTexCoord0Loc_3D, nngxGetPhysicalAddr(reinterpret_cast<u32>(&mLine.mShapeUV)), PICA_DATA_SIZE_2_FLOAT);
    mLine.EnableAttrAsArray(mAttrColorRateLoc_3D, nngxGetPhysicalAddr(reinterpret_cast<u32>(&mLine.mShapeColor)),  PICA_DATA_SIZE_1_FLOAT);

    mLine.mShapeIndex.mIndexStream.physicalAddr   = nngxGetPhysicalAddr(reinterpret_cast<u32>(&mLine.mShapeColor));
    mLine.mShapeIndex.mIndexStream.drawVtxNum     = 2;
    mLine.mShapeIndex.mIndexStream.isUnsignedByte = false;
}

void PrimitiveRendererCtr::loadQuadVertex_(Heap* heap)
{
    createBuffer_(heap, &mBox, 4, 6);
    PrimitiveRendererUtil::Vertex vtx[4];
    PrimitiveRendererUtil::setQuadVertex(vtx, mBox.mShapeIndex.mTextureIndex);
    copyAndSetupVtx_(&mBox, vtx, 8, 36);
}

void PrimitiveRendererCtr::loadSphereVertex_(Heap* heap, Shape* shape, s32 x, s32 y)
{
    s32 vert = PrimitiveRendererUtil::calcSphereVertexNum(x,y);
    s32 index = PrimitiveRendererUtil::calcSphereIndexNum(x,y);
    createBuffer_(heap, shape, vert, index);
    PrimitiveRendererUtil::Vertex vtx[130];
    PrimitiveRendererUtil::setSphereVertex(vtx, shape->mShapeIndex.mTextureIndex, x, y);
    copyAndSetupVtx_(shape, vtx, 8, 36);
}

void PrimitiveRendererCtr::loadWireCubeIndex_(Heap* heap)
{
    mWireCubeIndex.mTextureIndex = new(heap) u16[17];

    mWireCubeIndex.mTextureIndex[0]  = 0;
    mWireCubeIndex.mTextureIndex[1]  = 1;
    mWireCubeIndex.mTextureIndex[2]  = 2;
    mWireCubeIndex.mTextureIndex[3]  = 3;
    mWireCubeIndex.mTextureIndex[4]  = 0;
    mWireCubeIndex.mTextureIndex[5]  = 7;
    mWireCubeIndex.mTextureIndex[6]  = 6;
    mWireCubeIndex.mTextureIndex[7]  = 1;
    mWireCubeIndex.mTextureIndex[8]  = 2;
    mWireCubeIndex.mTextureIndex[9]  = 5;
    mWireCubeIndex.mTextureIndex[10] = 6;
    mWireCubeIndex.mTextureIndex[11] = 7;
    mWireCubeIndex.mTextureIndex[12] = 4;
    mWireCubeIndex.mTextureIndex[13] = 5;
    mWireCubeIndex.mTextureIndex[14] = 4;
    mWireCubeIndex.mTextureIndex[15] = 3;
    mWireCubeIndex.mTextureIndex[16] = 0;

    mWireCubeIndex.mIndexStream.physicalAddr = nngxGetPhysicalAddr(reinterpret_cast<u32>(&mWireCubeIndex.mTextureIndex));

    mWireCubeIndex.mIndexStream.drawVtxNum     = 17;
    mWireCubeIndex.mIndexStream.isUnsignedByte = false;
}

void PrimitiveRendererCtr::checkCmdlist_()
{
    SEAD_ASSERT_MSG(PtrUtil::diff((u32*)mListCommand, mCmdlistBufSize) < 0, "cmdlist buffer overrun.");
}

void PrimitiveRendererCtr::drawShape_(const Shape& shape)
{
    drawShape_(shape, shape.mShapeIndex.mIndexStream);
}

void PrimitiveRendererCtr::drawShape_(const nn::gr::CTR::Vertex& vert, const nn::gr::CTR::Vertex::IndexStream& vertIndex){ 
    if(mCurrentVertex != &vert)
    {
        if(mCurrentVertex != NULL)
        {
            mListCommand = mCurrentVertex->MakeDisableCommand(mListCommand);
        }

        mListCommand = vert.MakeEnableAttrCommand(mListCommand);
    }

    mListCommand = vert.MakeDrawCommand(mListCommand, vertIndex);

    mCurrentVertex = &vert;

    checkCmdlist_();
}
}