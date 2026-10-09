#pragma once

#include <gfx/seadGraphics.h>
#include <nn/gr.h>

namespace sead 
{
class GraphicsContext
{
public:
    GraphicsContext();

    virtual ~GraphicsContext()
    {
    }

    void apply() const;
    void applyAlphaTest() const;
    void applyBlendAndConstantColor() const;
    void applyCulling() const;
    void applyDepthTest() const;
    void applyStencilTest() const;

    void setDepthEnable(bool testEnable, bool writeEnable)
    {
        setDepthTestEnable(testEnable);
        setDepthWriteEnable(writeEnable);
    }

    void setDepthTestEnable(bool testEnable)
    {
        mDepthTestEnable = testEnable;
    }

    void setDepthWriteEnable(bool writeEnable)
    {
        mDepthWriteEnable = writeEnable;
    }

    void setDepthFunc(Graphics::DepthFunc func)
    {
        mDepthFunc = func;
    }

    void setCullingMode(Graphics::CullingMode mode)
    {
        mCullingMode = mode;
    }

    void setBlendEnable(bool blend)
    {
        mBlendEnable = blend;
    }

    void setBlendFactor(Graphics::BlendFactor srcFactor, Graphics::BlendFactor dstFactor)
    {
        setBlendFactorSrc(srcFactor);
        setBlendFactorDst(dstFactor);
    }

    void setBlendFactorSeparate(Graphics::BlendFactor srcFactorRgb, Graphics::BlendFactor dstFactorRgb, Graphics::BlendFactor srcFactorA, Graphics::BlendFactor dstFactorA)
    {
        setBlendFactorSrcRGB(srcFactorRgb);
        setBlendFactorDstRGB(dstFactorRgb);
        setBlendFactorSrcAlpha(srcFactorA);
        setBlendFactorDstAlpha(dstFactorA);
    }

    void setBlendFactorSrc(Graphics::BlendFactor factor)
    {
        setBlendFactorSrcRGB(factor);
        setBlendFactorSrcAlpha(factor);
    }

    void setBlendFactorDst(Graphics::BlendFactor factor)
    {
        setBlendFactorDstRGB(factor);
        setBlendFactorDstAlpha(factor);
    }

    void setBlendFactorSrcRGB(Graphics::BlendFactor factor)
    {
        mBlendFactorSrcRGB = factor;
    }

    void setBlendFactorSrcAlpha(Graphics::BlendFactor factor)
    {
        mBlendFactorSrcA = factor;
    }

    void setBlendFactorDstRGB(Graphics::BlendFactor factor)
    {
        mBlendFactorDstRGB = factor;
    }

    void setBlendFactorDstAlpha(Graphics::BlendFactor factor)
    {
        mBlendFactorDstA = factor;
    }

    void setBlendEquation(Graphics::BlendEquation equation)
    {
        setBlendEquationRGB(equation);
        setBlendEquationAlpha(equation);
    }

    void setBlendEquationSeparate(Graphics::BlendEquation equationRgb, Graphics::BlendEquation equationA)
    {
        setBlendEquationRGB(equationRgb);
        setBlendEquationAlpha(equationA);
    }

    void setBlendEquationRGB(Graphics::BlendEquation equation)
    {
        mBlendEquationRGB = equation;
    }

    void setBlendEquationAlpha(Graphics::BlendEquation equation)
    {
        mBlendEquationA = equation;
    }

    void setBlendConstantColor(const Color4f& color)
    {
        mBlendConstantColor = color;
    }

    void setAlphaTestEnable(bool enable)
    {
        mAlphaTestEnable = enable;
    }

    void setAlphaTestFunc(Graphics::AlphaFunc func, f32 ref)
    {
        mAlphaTestFunc = func;
        mAlphaTestRef = ref;
    }

    void setColorMask(bool r, bool g, bool b, bool a)
    {
        mColorMaskR = r;
        mColorMaskG = g;
        mColorMaskB = b;
        mColorMaskA = a;
    }

    void setStencilTestEnable(bool enable)
    {
        mStencilTestEnable = enable;
    }

    void setStencilTestFunc(Graphics::StencilFunc func, s32 ref, u32 mask)
    {
        mStencilTestFunc = func;
        mStencilTestRef = ref;
        mStencilTestMask = mask;
    }

    void setStencilTestOp(Graphics::StencilOp fail, Graphics::StencilOp zfail, Graphics::StencilOp zpass)
    {
        mStencilOpFail = fail;
        mStencilOpZFail = zfail;
        mStencilOpZPass = zpass;
    }

    bool getDepthTestEnable() const
    {
        return mDepthTestEnable;
    }

    bool getDepthWriteEnable() const
    {
        return mDepthWriteEnable;
    }

    Graphics::DepthFunc getDepthFunc() const
    {
        return mDepthFunc;
    }

    Graphics::CullingMode getCullingMode() const
    {
        return mCullingMode;
    }

    bool getBlendEnable() const
    {
        return mBlendEnable;
    }

    Graphics::BlendFactor getBlendFactorSrcRGB() const
    {
        return mBlendFactorSrcRGB;
    }

    Graphics::BlendFactor getBlendFactorSrcAlpha() const
    {
        return mBlendFactorSrcA;
    }

    Graphics::BlendFactor getBlendFactorDstRGB() const
    {
        return mBlendFactorDstRGB;
    }

    Graphics::BlendFactor getBlendFactorDstAlpha() const
    {
        return mBlendFactorDstA;
    }

    Graphics::BlendEquation getBlendEquationRGB() const
    {
        return mBlendEquationRGB;
    }

    Graphics::BlendEquation getBlendEquationAlpha() const
    {
        return mBlendEquationA;
    }

    const Color4f& getBlendConstantColor() const
    {
        return mBlendConstantColor;
    }

    bool getAlphaTestEnable() const
    {
        return mAlphaTestEnable;
    }

    Graphics::AlphaFunc getAlphaTestFunc() const
    {
        return mAlphaTestFunc;
    }

    f32 getAlphaTestRef() const
    {
        return mAlphaTestRef;
    }

    bool getColorMaskR() const
    {
        return mColorMaskR;
    }

    bool getColorMaskG() const
    {
        return mColorMaskG;
    }

    bool getColorMaskB() const
    {
        return mColorMaskB;
    }

    bool getColorMaskA() const
    {
        return mColorMaskA;
    }

    bool getStencilTestEnable() const
    {
        return mStencilTestEnable;
    }

    Graphics::StencilFunc getStencilTestFunc() const
    {
        return mStencilTestFunc;
    }

    s32 getStencilTestRef() const
    {
        return mStencilTestRef;
    }

    u32 getStencilTestMask() const
    {
        return mStencilTestMask;
    }

    Graphics::StencilOp getStencilTestOpFail() const
    {
        return mStencilOpFail;
    }

    Graphics::StencilOp getStencilTestOpZFail() const
    {
        return mStencilOpZFail;
    }

    Graphics::StencilOp getStencilTestOpZPass() const
    {
        return mStencilOpZPass;
    }

protected:
    bool mDepthTestEnable;
    bool mDepthWriteEnable;
    Graphics::DepthFunc mDepthFunc;
    Graphics::CullingMode mCullingMode;
    bool mBlendEnable;
    Graphics::BlendFactor mBlendFactorSrcRGB;
    Graphics::BlendFactor mBlendFactorSrcA;
    Graphics::BlendFactor mBlendFactorDstRGB;
    Graphics::BlendFactor mBlendFactorDstA;
    Graphics::BlendEquation mBlendEquationRGB;
    Graphics::BlendEquation mBlendEquationA;
    Color4f mBlendConstantColor;
    bool mAlphaTestEnable;
    Graphics::AlphaFunc mAlphaTestFunc;
    f32 mAlphaTestRef;
    bool mColorMaskR;
    bool mColorMaskG;
    bool mColorMaskB;
    bool mColorMaskA;
    bool mStencilTestEnable;
    Graphics::StencilFunc mStencilTestFunc;
    s32 mStencilTestRef;
    u32 mStencilTestMask;
    Graphics::StencilOp mStencilOpFail;
    Graphics::StencilOp mStencilOpZFail;
    Graphics::StencilOp mStencilOpZPass;
    RenderState mRenderState;
    PicaDataDepthTest mPicaDepthTest;
    PicaDataAlphaTest mPicaAlphaTest;
    PicaDataStencilTest mPicaStencilTest;
    u32 mPicaDataMask;
};

} // namespace sead
