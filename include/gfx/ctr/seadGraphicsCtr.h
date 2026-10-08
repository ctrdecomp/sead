#pragma once

#include "gfx/seadGraphics.h"
#include "gfx/ctr/seadGfxMemoryMgrCtr.h"

#include <nn/gr.h>

namespace sead
{
class GraphicsCtr : public Graphics
{
public:
    GraphicsCtr();

    virtual ~GraphicsCtr();
    virtual void initializeImpl(Heap* heap){ };
    virtual void setViewportImpl(f32 x, f32 y, f32 w, f32 h);
    virtual void setScissorImpl(f32 x, f32 y, f32 w, f32 h);
    virtual void setDepthTestEnableImpl(bool enable);
    virtual void setDepthWriteEnableImpl(bool enable);
    virtual void setDepthFuncImpl(DepthFunc func);
    virtual bool setVBlankWaitIntervalImpl(u32 interval){ return false; };
    virtual void setCullingModeImpl(CullingMode mode);
    virtual void setBlendEnableImpl(bool enable);
    virtual void setBlendEnableMRTImpl(u32 target, bool enable){ };
    virtual void setBlendFactorImpl(BlendFactor srcFactorRgb, BlendFactor dstFactorRgb, BlendFactor srcFactorA, BlendFactor dstFactorA);
    virtual void setBlendFactorMRTImpl(u32 target, BlendFactor srcFactorRgb, BlendFactor dstFactorRgb, BlendFactor srcFactorA, BlendFactor dstFactorA){ }
    virtual void setBlendEquationImpl(BlendEquation equationRgb, BlendEquation equationA);
    virtual void setBlendEquationMRTImpl(u32 target, BlendEquation equationRgb, BlendEquation equationA){ };
    virtual void setBlendConstantColorImpl(const Color4f& color);
    virtual void lockDrawContextImpl(){}
    virtual void unlockDrawContextImpl(){}
    virtual void waitForVBlankImpl(){ nngxWaitVSync(NN_GX_DISPLAY_BOTH); }
    virtual void setColorMaskImpl(bool r, bool g, bool b, bool a);
    virtual void setColorMaskMRTImpl(u32 target, bool r, bool g, bool b, bool a){ }
    virtual void setAlphaTestEnableImpl(bool enable);
    virtual void setAlphaTestFuncImpl(AlphaFunc func, f32 ref);
    virtual void setStencilTestEnableImpl(bool enable);
    virtual void setStencilTestFuncImpl(StencilFunc func, s32 ref, u32 mask);
    virtual void setStencilTestOpImpl(StencilOp fail, StencilOp zfail, StencilOp zpass);
    virtual void setPolygonModeImpl(PolygonMode front, PolygonMode back){}
    virtual void setPolygonOffsetEnableImpl(bool fillFrontEnable, bool fillBackEnable, bool pointLineEnable){}

    static GfxMemoryMgrCtr* sMemoryMgr;
    static s32 allocate(size_t area, u32 alignment, u32 size, s32 heap);
    static void deallocate(size_t area, u32 alignment, u32 size, void* obj);

    nn::gr::CTR::Viewport const& getGrViewport() const { return mViewportCtr; }
private:
    RenderState mRenderState;
    nn::gr::CTR::Viewport mViewportCtr;
    Scissor mScissor;
};
}
