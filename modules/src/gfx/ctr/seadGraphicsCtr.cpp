// Filename: seadGraphicsCtr.cpp
//
// Project: StandardEAD C++ Library for CTR

#include "gfx/ctr/seadGraphicsCtr.h"
#include "gfx/ctr/seadFrameBufferCtr.h"

namespace sead
{
GfxMemoryMgrCtr* GraphicsCtr::sMemoryMgr = NULL;

GraphicsCtr::GraphicsCtr():
    mRenderState(),
    mViewportCtr(),
    mScissor()
{
}

GraphicsCtr::~GraphicsCtr()
{
}

void GraphicsCtr::setViewportImpl(f32 x, f32 y, f32 w, f32 h)
{
    mViewportCtr.Set(x, y, w, h);
    s32 list;
    nngxGetCmdlistParameteri(NN_GX_CMDLIST_CURRENT_BUFADDR, &list);

    bit32* start = reinterpret_cast<bit32*>(list);
    bit32* command = mViewportCtr.MakeCommand(start);

    nngxMoveCommandbufferPointer((command - start) * sizeof(bit32));
}

void GraphicsCtr::setScissorImpl(f32 x, f32 y, f32 w, f32 h)
{
    s32 list;
    nngxGetCmdlistParameteri(NN_GX_CMDLIST_CURRENT_BUFADDR, &list);

    s32 dWidth = FrameBufferCtr::getBoundFrameBufferGR()->depthStencilBuffer.width;
    s32 dHeight = FrameBufferCtr::getBoundFrameBufferGR()->depthStencilBuffer.height;
    mScissor.Set(x, y, w, h);
    mScissor.SetBufferSize(dWidth, dHeight);

    bit32* start = reinterpret_cast<bit32*>(list);
    bit32* command = mScissor.MakeCommand(start);

    nngxMoveCommandbufferPointer((command - start) * sizeof(bit32));
}

void GraphicsCtr::setDepthTestEnableImpl(bool enable)
{
    s32 list;
    nngxGetCmdlistParameteri(NN_GX_CMDLIST_CURRENT_BUFADDR, &list);

    bit32* start = reinterpret_cast<bit32*>(list);

    mRenderState.depthTest.isEnable = enable;
    bit32* command = mRenderState.depthTest.MakeCommand(start);

    nngxMoveCommandbufferPointer((command - start) * sizeof(bit32));
}

void GraphicsCtr::setDepthWriteEnableImpl(bool enable)
{
    s32 list;
    nngxGetCmdlistParameteri(NN_GX_CMDLIST_CURRENT_BUFADDR, &list);

    bit32* start = reinterpret_cast<bit32*>(list);

    mRenderState.depthTest.isEnableWrite = enable;
    bit32* command = mRenderState.depthTest.MakeCommand(start);

    nngxMoveCommandbufferPointer((command - start) * sizeof(bit32));
}

void GraphicsCtr::setDepthFuncImpl(DepthFunc func)
{
    s32 list;
    nngxGetCmdlistParameteri(NN_GX_CMDLIST_CURRENT_BUFADDR, &list);

    bit32* start = reinterpret_cast<bit32*>(list);

    mRenderState.depthTest.func = static_cast<PicaDataDepthTest>(func);
    bit32* command = mRenderState.depthTest.MakeCommand(start);

    nngxMoveCommandbufferPointer((command - start) * sizeof(bit32));
}

void GraphicsCtr::setCullingModeImpl(Graphics::CullingMode mode)
{
    GLint list;
    nngxGetCmdlistParameteri(NN_GX_CMDLIST_CURRENT_BUFADDR, &list);

    switch (mode)
    {
    case cNoneCulling:
        mRenderState.cullingTest.isEnable = false;
        break;

    case cFrontCulling:
    case cBackCulling:
        mRenderState.cullingTest.isEnable = true;

        mRenderState.cullingTest.cullFace = static_cast<RenderState::Culling::CullFace>(mode);
        break;

    case cAllCulling:
        SEAD_ASSERT_MSG(false, "Unsupported culling mode.");
        break;
    }

    bit32* start = reinterpret_cast<bit32*>(list);

    bit32* command = mRenderState.cullingTest.MakeCommand(start);

    nngxMoveCommandbufferPointer((command - reinterpret_cast<bit32*>(list)) * sizeof(bit32));
}

void GraphicsCtr::setBlendEnableImpl(bool enable)
{
    s32 list;
    nngxGetCmdlistParameteri(NN_GX_CMDLIST_CURRENT_BUFADDR, &list);

    bit32* start = reinterpret_cast<bit32*>(list);

    mRenderState.blend.isEnable = enable;
    bit32* command = mRenderState.blend.MakeCommand(start);

    nngxMoveCommandbufferPointer((command - start) * sizeof(bit32));
}

void GraphicsCtr::setBlendFactorImpl(BlendFactor srcFactorRgb, BlendFactor dstFactorRgb, BlendFactor srcFactorA, BlendFactor dstFactorA)
{
    s32 list;
    nngxGetCmdlistParameteri(NN_GX_CMDLIST_CURRENT_BUFADDR, &list);

    bit32* start = reinterpret_cast<bit32*>(list);

    mRenderState.blend.srcRgb = static_cast<PicaDataBlendFunc>(srcFactorRgb);
    mRenderState.blend.dstRgb = static_cast<PicaDataBlendFunc>(dstFactorRgb);
    mRenderState.blend.srcAlpha = static_cast<PicaDataBlendFunc>(srcFactorA);
    mRenderState.blend.dstAlpha = static_cast<PicaDataBlendFunc>(dstFactorA);
    bit32* command = mRenderState.blend.MakeCommand(start);

    nngxMoveCommandbufferPointer((command - start) * sizeof(bit32));
}

void GraphicsCtr::setBlendEquationImpl(BlendEquation equationRgb, BlendEquation equationA)
{
    s32 list;
    nngxGetCmdlistParameteri(NN_GX_CMDLIST_CURRENT_BUFADDR, &list);

    bit32* start = reinterpret_cast<bit32*>(list);

    mRenderState.blend.eqRgb = static_cast<PicaDataBlendEquation>(equationRgb);
    mRenderState.blend.eqAlpha = static_cast<PicaDataBlendEquation>(equationA);

    bit32* command = mRenderState.blend.MakeCommand(start);

    nngxMoveCommandbufferPointer((command - start) * sizeof(bit32));
}

void GraphicsCtr::setBlendConstantColorImpl(const Color4f& color)
{
    s32 list;
    nngxGetCmdlistParameteri(NN_GX_CMDLIST_CURRENT_BUFADDR, &list);

    bit32* start = reinterpret_cast<bit32*>(list);

    Color4u8 bColor; 
    bColor = color;

    mRenderState.blend.colorR = bColor.r;
    mRenderState.blend.colorG = bColor.g;
    mRenderState.blend.colorB = bColor.b;
    mRenderState.blend.colorA = bColor.a;

    bit32* command = mRenderState.blend.MakeCommand(start);

    nngxMoveCommandbufferPointer((command - start) * sizeof(bit32));
}

void GraphicsCtr::setColorMaskImpl(bool r, bool g, bool b, bool a)
{
    s32 list;
    nngxGetCmdlistParameteri(NN_GX_CMDLIST_CURRENT_BUFADDR, &list);

    bit32* start = reinterpret_cast<bit32*>(list);

    u8 mask;
    mask |= r;
    mask |= g ? 2 : 0;
    mask |= b ? 4 : 0;
    mask |= a ? 8 : 0;

    mRenderState.colorMask = static_cast<RenderState::ColorMask>(mask);

    bit32* command = mRenderState.fbAccess.MakeCommand(start, 1);

    nngxMoveCommandbufferPointer((command - start) * sizeof(bit32));
}

void GraphicsCtr::setAlphaTestEnableImpl(bool enable)
{
    s32 list;
    nngxGetCmdlistParameteri(NN_GX_CMDLIST_CURRENT_BUFADDR, &list);

    bit32* start = reinterpret_cast<bit32*>(list);

    mRenderState.alphaTest.isEnable = enable;
    bit32* command = mRenderState.alphaTest.MakeCommand(start);

    nngxMoveCommandbufferPointer((command - start) * sizeof(bit32));
}

void GraphicsCtr::setAlphaTestFuncImpl(AlphaFunc func, f32 ref)
{
    s32 list;
    nngxGetCmdlistParameteri(NN_GX_CMDLIST_CURRENT_BUFADDR, &list);

    bit32* start = reinterpret_cast<bit32*>(list);

    mRenderState.alphaTest.func = static_cast<PicaDataAlphaTest>(func);
    FloatToUnsignedByte(ref);

    bit32* command = mRenderState.alphaTest.MakeCommand(start);

    nngxMoveCommandbufferPointer((command - start) * sizeof(bit32));
}

void GraphicsCtr::setStencilTestEnableImpl(bool enable)
{
    s32 list;
    nngxGetCmdlistParameteri(NN_GX_CMDLIST_CURRENT_BUFADDR, &list);

    bit32* start = reinterpret_cast<bit32*>(list);

    mRenderState.stencilTest.isEnable = enable;
    bit32* command = mRenderState.stencilTest.MakeCommand(start);

    nngxMoveCommandbufferPointer((command - start) * sizeof(bit32));
}

void GraphicsCtr::setStencilTestFuncImpl(StencilFunc func, s32 ref, u32 mask)
{
    s32 list;
    nngxGetCmdlistParameteri(NN_GX_CMDLIST_CURRENT_BUFADDR, &list);

    bit32* start = reinterpret_cast<bit32*>(list);

    mRenderState.stencilTest.func = static_cast<PicaDataStencilTest>(func);
    mRenderState.stencilTest.ref = ref;
    mRenderState.stencilTest.mask = mask;

    bit32* command = mRenderState.alphaTest.MakeCommand(start);

    nngxMoveCommandbufferPointer((command - start) * sizeof(bit32));
}

void GraphicsCtr::setStencilTestOpImpl(StencilOp fail, StencilOp zfail, StencilOp zpass)
{
    s32 list;
    nngxGetCmdlistParameteri(NN_GX_CMDLIST_CURRENT_BUFADDR, &list);

    bit32* start = reinterpret_cast<bit32*>(list);

    mRenderState.stencilTest.opFail = static_cast<PicaDataStencilOp>(fail);
    mRenderState.stencilTest.opZFail = static_cast<PicaDataStencilOp>(zfail);
    mRenderState.stencilTest.opZPass = static_cast<PicaDataStencilOp>(zpass);

    bit32* command = mRenderState.alphaTest.MakeCommand(start);

    nngxMoveCommandbufferPointer((command - start) * sizeof(bit32));
}

s32 GraphicsCtr::allocate(size_t area, u32 alignment, u32 size, s32 heap)
{
    sMemoryMgr->allocate(area, alignment, size, reinterpret_cast<Heap*>(heap));
}

void GraphicsCtr::deallocate(size_t area, u32 alignment, u32 size, void* obj)
{
    sMemoryMgr->deallocate(area, alignment, size, obj);
}
}
