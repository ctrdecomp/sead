#include <gfx/seadGraphicsContext.h>

namespace sead {

GraphicsContext::GraphicsContext(): 
    mDepthTestEnable(true), 
    mDepthWriteEnable(true), 
    mDepthFunc(Graphics::cLessEqualDepth), 

    mCullingMode(Graphics::cBackCulling), 

    mBlendEnable(true), 
    mBlendFactorSrcRGB(Graphics::cSrcAlphaBlend), 
    mBlendFactorSrcA(Graphics::cSrcAlphaBlend), 
    mBlendFactorDstRGB(Graphics::cInvSrcAlphaBlend),
    mBlendFactorDstA(Graphics::cInvSrcAlphaBlend), 
    mBlendEquationRGB(Graphics::cAddBlendEquation), 
    mBlendEquationA(Graphics::cAddBlendEquation), 
    mBlendConstantColor(1.0f, 1.0f, 1.0f, 1.0f), 

    mAlphaTestEnable(false), 
    mAlphaTestFunc(Graphics::cGreaterAlpha),
    mAlphaTestRef(0.0f), 

    mColorMaskR(true), 
    mColorMaskG(true), 
    mColorMaskB(true), 
    mColorMaskA(true), 

    mStencilTestEnable(false), 
    mStencilTestFunc(Graphics::cNeverStencil), 
    mStencilTestRef(0), 
    mStencilTestMask(0xFFFFFFFF), 
    mStencilOpFail(Graphics::cKeepStencil), 
    mStencilOpZFail(Graphics::cKeepStencil), 
    mStencilOpZPass(Graphics::cKeepStencil),
    mRenderState(),
    mPicaDepthTest(PICA_DATA_DEPTH_TEST_LEQUAL),
    mPicaAlphaTest(PICA_DATA_ALPHA_TEST_GREATER),
    mPicaStencilTest(PICA_DATA_STENCIL_TEST_NEVER),
    mPicaDataMask(0xFFFFFFFF)
{
}

void GraphicsContext::apply() const
{
    s32 list;
    nngxGetCmdlistParameteri(NN_GX_CMDLIST_CURRENT_BUFADDR, &list);

    bit32* start = reinterpret_cast<bit32*>(list);

    bit32* command = mRenderState.MakeCommand(start);

    nngxMoveCommandbufferPointer((command - start) * sizeof(bit32));
}

void GraphicsContext::applyAlphaTest() const
{
    s32 list;
    nngxGetCmdlistParameteri(NN_GX_CMDLIST_CURRENT_BUFADDR, &list);

    bit32* start = reinterpret_cast<bit32*>(list);

    bit32* command = mRenderState.alphaTest.MakeCommand(start);

    nngxMoveCommandbufferPointer((command - start) * sizeof(bit32));
}

void GraphicsContext::applyBlendAndConstantColor() const
{
    s32 list;
    nngxGetCmdlistParameteri(NN_GX_CMDLIST_CURRENT_BUFADDR, &list);

    bit32* start = reinterpret_cast<bit32*>(list);

    bit32* command = mRenderState.blend.MakeCommand(start);

    nngxMoveCommandbufferPointer((command - start) * sizeof(bit32));
}

void GraphicsContext::applyCulling() const
{
    s32 list;
    nngxGetCmdlistParameteri(NN_GX_CMDLIST_CURRENT_BUFADDR, &list);

    bit32* start = reinterpret_cast<bit32*>(list);

    bit32* command = mRenderState.cullingTest.MakeCommand(start);

    nngxMoveCommandbufferPointer((command - start) * sizeof(bit32));
}

void GraphicsContext::applyDepthTest() const
{
    s32 list;
    nngxGetCmdlistParameteri(NN_GX_CMDLIST_CURRENT_BUFADDR, &list);

    bit32* start = reinterpret_cast<bit32*>(list);

    bit32* command = mRenderState.depthTest.MakeCommand(start);

    nngxMoveCommandbufferPointer((command - start) * sizeof(bit32));
}

void GraphicsContext::applyStencilTest() const
{
    s32 list;
    nngxGetCmdlistParameteri(NN_GX_CMDLIST_CURRENT_BUFADDR, &list);

    bit32* start = reinterpret_cast<bit32*>(list);

    bit32* command = mRenderState.stencilTest.MakeCommand(start);

    nngxMoveCommandbufferPointer((command - start) * sizeof(bit32));
}
} // namespace sead