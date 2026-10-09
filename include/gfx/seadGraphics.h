#pragma once

#include <gfx/seadColor.h>
#include <gfx/seadDrawLockContext.h>
#include <heap/seadDisposer.h>
#include <nn/gx.h>

namespace sead
{
class Graphics : public IDisposer
{
protected:
    typedef void (*UnknownCallback)(int);

public:
    typedef void (*LockFunc)(bool isLock);
    
    enum DevicePosture
    {
        cDevicePosture_Same = 0,
        cDevicePosture_RotateRight = 1,
        cDevicePosture_RotateLeft = 2,
        cDevicePosture_RotateHalfAround = 3,
        cDevicePosture_FlipX = 4,
        cDevicePosture_FlipY = 5,
        cDevicePosture_FlipXY = 3,
        cDevicePosture_Invalid = 4,
    };

    enum DepthFunc
    {
        cLessEqualDepth = GL_LEQUAL,
        cLessDepth = GL_LESS,
        cGreaterEqualDepth = GL_GEQUAL,
        cGreaterDepth = GL_GREATER,
        cAlwaysDepth = GL_ALWAYS,
        cNeverDepth = GL_NEVER,
        cEqualDepth = GL_EQUAL,
        cNotEqualDepth = GL_NOTEQUAL,
        cInvalidDepth = 0xFFFFFFFF
    };

    enum CullingMode
    {
        cFrontCulling = GL_FRONT,
        cBackCulling = GL_BACK,
        cNoneCulling = 0xFFFFFFF0,
        cAllCulling = 0xFFFFFFF1,
        cInvalidCulling = 0xFFFFFFFF
    };

    enum BlendFactor
    {
        cZeroBlend = GL_ZERO,
        cOneBlend = GL_ONE,
        cSrcColorBlend = GL_SRC_COLOR,
        cInvSrcColorBlend = GL_ONE_MINUS_SRC_COLOR,
        cSrcAlphaBlend = GL_SRC_ALPHA,
        cInvSrcAlphaBlend = GL_ONE_MINUS_SRC_ALPHA,
        cDstColorBlend = GL_DST_COLOR,
        cInvDstColorBlend = GL_ONE_MINUS_DST_COLOR,
        cDstAlphaBlend = GL_DST_ALPHA,
        cInvDstAlphaBlend = GL_ONE_MINUS_DST_ALPHA,
        cConstantColorBlend = GL_CONSTANT_COLOR,
        cInvConstantColorBlend = GL_ONE_MINUS_CONSTANT_COLOR,
        cConstantAlphaBlend = GL_CONSTANT_ALPHA,
        cInvConstantAlphaBlend = GL_ONE_MINUS_CONSTANT_ALPHA,
        cSrcAlphaSaturateBlend = GL_SRC_ALPHA_SATURATE,
        cInvalidBlend = 0xFFFFFFFF
    };

    enum BlendEquation
    {
        cAddBlendEquation = GL_FUNC_ADD,
        cSubBlendEquation = GL_FUNC_SUBTRACT,
        cReverseSubBlendEquation = GL_FUNC_REVERSE_SUBTRACT,
        cMinBlendEquation = GL_MIN,
        cMaxBlendEquation = GL_MAX,
        cInvalidBlendEquation = 0xFFFFFFFF
    };

    enum AlphaFunc
    {
        cLessEqualAlpha = GL_LEQUAL,
        cLessAlpha = GL_LESS,
        cGreaterEqualAlpha = GL_GEQUAL,
        cGreaterAlpha = GL_GREATER,
        cAlwaysAlpha = GL_ALWAYS,
        cNeverAlpha = GL_NEVER,
        cEqualAlpha = GL_EQUAL,
        cNotEqualAlpha = GL_NOTEQUAL,
        cInvalidAlpha = 0xFFFFFFFF
    };

    enum StencilFunc
    {
        cLessEqualStencil = GL_LEQUAL,
        cLessStencil = GL_LESS,
        cGreaterEqualStencil = GL_GEQUAL,
        cGreaterStencil = GL_GREATER,
        cAlwaysStencil = GL_ALWAYS,
        cNeverStencil = GL_NEVER,
        cEqualStencil = GL_EQUAL,
        cNotEqualStencil = GL_NOTEQUAL,
        cInvalidStencilFunc = 0xFFFFFFFF
    };

    enum StencilOp
    {
        cKeepStencil = GL_KEEP,
        cZeroStencil = GL_ZERO,
        cReplaceStencil = GL_REPLACE,
        cIncrementStencil = GL_INCR,
        cDecrementStencil = GL_DECR,
        cInvertStencil = GL_INVERT,
        cIncrementWrapStencil = GL_INCR_WRAP,
        cDecrementWrapStencil = GL_DECR_WRAP,
        cInvalidStencilOp = 0xFFFFFFFF
    };

    enum PolygonMode
    {
        cInvalidPolygon = 0xFFFFFFFF
    };

    Graphics();
    virtual ~Graphics();

    void initialize(Heap* heap);
    void initializeDrawLockContext(Heap* heap);
    void lockDrawContext();
    void unlockDrawContext();
    void initHostIO();

    void setViewportRealPosition(f32 x, f32 y, f32 w, f32 h)
    {
        setViewportImpl(x, y, w, h);
    }

    void setScissorRealPosition(f32 x, f32 y, f32 w, f32 h)
    {
        setScissorImpl(x, y, w, h);
    }

    void setDepthEnable(bool testEnable, bool writeEnable)
    {
        setDepthTestEnable(testEnable);
        setDepthWriteEnable(writeEnable);
    }

    void setDepthTestEnable(bool enable)
    {
        setDepthTestEnableImpl(enable);
    }

    void setDepthWriteEnable(bool enable)
    {
        setDepthWriteEnableImpl(enable);
    }

    void setDepthFunc(DepthFunc func)
    {
        setDepthFuncImpl(func);
    }

    bool setVBlankWaitInterval(u32 interval)
    {
        return setVBlankWaitIntervalImpl(interval);
    }

    void setCullingMode(CullingMode mode)
    {
        setCullingModeImpl(mode);
    }

    void setBlendEnable(bool enable)
    {
        setBlendEnableImpl(enable);
    }

    void setBlendEnableMRT(u32 target, bool enable)
    {
        setBlendEnableMRTImpl(target, enable);
    }

    void setBlendFactor(BlendFactor srcFactor, BlendFactor dstFactor)
    {
        setBlendFactorImpl(srcFactor, dstFactor, srcFactor, dstFactor);
    }

    void setBlendFactorSeparate(BlendFactor srcFactorRgb, BlendFactor dstFactorRgb, BlendFactor srcFactorA, BlendFactor dstFactorA)
    {
        setBlendFactorImpl(srcFactorRgb, dstFactorRgb, srcFactorA, dstFactorA);
    }

    void setBlendFactorMRT(u32 target, BlendFactor srcFactor, BlendFactor dstFactor)
    {
        setBlendFactorMRTImpl(target, srcFactor, dstFactor, srcFactor, dstFactor);
    }

    void setBlendFactorMRTSeparate(u32 target, BlendFactor srcFactorRgb, BlendFactor dstFactorRgb, BlendFactor srcFactorA, BlendFactor dstFactorA)
    {
        setBlendFactorMRTImpl(target, srcFactorRgb, dstFactorRgb, srcFactorA, dstFactorA);
    }

    void setBlendEquation(BlendEquation equation)
    {
        setBlendEquationImpl(equation, equation);
    }

    void setBlendEquationSeparate(BlendEquation equationRgb, BlendEquation equationA)
    {
        setBlendEquationImpl(equationRgb, equationA);
    }

    void setBlendEquationMRT(u32 target, BlendEquation equation)
    {
        setBlendEquationMRTImpl(target, equation, equation);
    }

    void setBlendEquationMRTSeparate(u32 target, BlendEquation equationRgb, BlendEquation equationA)
    {
        setBlendEquationMRTImpl(target, equationRgb, equationA);
    }

    void setBlendConstantColor(const Color4f& color)
    {
        setBlendConstantColorImpl(color);
    }

    void waitForVBlank()
    {
        waitForVBlankImpl();
    }

    void setColorMask(bool r, bool g, bool b, bool a)
    {
        setColorMaskImpl(r, g, b, a);
    }

    void setColorMaskMRT(u32 target, bool r, bool g, bool b, bool a)
    {
        setColorMaskMRTImpl(target, r, g, b, a);
    }

    void setAlphaTestEnable(bool enable)
    {
        setAlphaTestEnableImpl(enable);
    }

    void setAlphaTestFunc(AlphaFunc func, f32 ref)
    {
        setAlphaTestFuncImpl(func, ref);
    }

    void setStencilTestEnable(bool enable)
    {
        setStencilTestEnableImpl(enable);
    }

    void setStencilTestFunc(StencilFunc func, s32 ref, u32 mask)
    {
        setStencilTestFuncImpl(func, ref, mask);
    }

    void setStencilTestOp(StencilOp fail, StencilOp zfail, StencilOp zpass)
    {
        setStencilTestOpImpl(fail, zfail, zpass);
    }

    void setPolygonMode(PolygonMode front, PolygonMode back)
    {
        setPolygonModeImpl(front, back);
    }

    void setPolygonOffsetEnable(bool fillFrontEnable, bool fillBackEnable, bool pointLineEnable)
    {
        setPolygonOffsetEnableImpl(fillFrontEnable, fillBackEnable, pointLineEnable);
    }
    
    static DevicePosture getDefaultDevicePosture() { return sDefaultDevicePosture; }
    static f32 getDefaultDeviceZScale() { return sDefaultDeviceZScale; }
    static f32 getDefaultDeviceZOffset() { return sDefaultDeviceZOffset; }
    static Graphics* instance(){ return sInstance; }
    static void setInstance(Graphics* inst){ sInstance = inst; }

    virtual void initializeImpl(Heap* heap) = 0;
    virtual void setViewportImpl(f32 x, f32 y, f32 w, f32 h) = 0;
    virtual void setScissorImpl(f32 x, f32 y, f32 w, f32 h) = 0;
    virtual void setDepthTestEnableImpl(bool enable) = 0;
    virtual void setDepthWriteEnableImpl(bool enable) = 0;
    virtual void setDepthFuncImpl(DepthFunc func) = 0;
    virtual bool setVBlankWaitIntervalImpl(u32 interval) = 0;
    virtual void setCullingModeImpl(CullingMode mode) = 0;
    virtual void setBlendEnableImpl(bool enable) = 0;
    virtual void setBlendEnableMRTImpl(u32 target, bool enable) = 0;
    virtual void setBlendFactorImpl(BlendFactor srcFactorRgb, BlendFactor dstFactorRgb, BlendFactor srcFactorA, BlendFactor dstFactorA) = 0;
    virtual void setBlendFactorMRTImpl(u32 target, BlendFactor srcFactorRgb, BlendFactor dstFactorRgb, BlendFactor srcFactorA, BlendFactor dstFactorA) = 0;
    virtual void setBlendEquationImpl(BlendEquation equationRgb, BlendEquation equationA) = 0;
    virtual void setBlendEquationMRTImpl(u32 target, BlendEquation equationRgb, BlendEquation equationA) = 0;
    virtual void setBlendConstantColorImpl(const Color4f& color) = 0;
    virtual void lockDrawContextImpl() = 0;
    virtual void unlockDrawContextImpl() = 0;
    virtual void waitForVBlankImpl() = 0;
    virtual void setColorMaskImpl(bool r, bool g, bool b, bool a) = 0;
    virtual void setColorMaskMRTImpl(u32 target, bool r, bool g, bool b, bool a) = 0;
    virtual void setAlphaTestEnableImpl(bool enable) = 0;
    virtual void setAlphaTestFuncImpl(AlphaFunc func, f32 ref) = 0;
    virtual void setStencilTestEnableImpl(bool enable) = 0;
    virtual void setStencilTestFuncImpl(StencilFunc func, s32 ref, u32 mask) = 0;
    virtual void setStencilTestOpImpl(StencilOp fail, StencilOp zfail, StencilOp zpass) = 0;
    virtual void setPolygonModeImpl(PolygonMode front, PolygonMode back) = 0;
    virtual void setPolygonOffsetEnableImpl(bool fillFrontEnable, bool fillBackEnable, bool pointLineEnable) = 0;

    void clear(u32 colorIdx, const Color4f& color, f32 depth, u32 stencil);

protected:
    static Graphics* sInstance;

    static DevicePosture sDefaultDevicePosture;
    static f32 sDefaultDeviceZScale;
    static f32 sDefaultDeviceZOffset;
private:
    LockFunc mContextLockFunc;
    DrawLockContext* mDrawLockContext;
};

}  // namespace sead

