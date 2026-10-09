#pragma once

#include "gfx/seadFrameBuffer.h"
#include "prim/seadMemUtil.h"
#include <nn/gr.h>

namespace sead
{
class FrameBufferCtr : public sead::FrameBuffer
{
public:
    FrameBufferCtr(nn::gr::CTR::FrameBuffer const& nn_buffer, Vector2<float> const& virtual_size, 
        f32 physical_x, f32 physical_y, f32 physical_w, f32 physical_h):
        FrameBuffer(virtual_size, physical_x, physical_y, physical_w, physical_h),
        nn_buffer()
    {
        copyFrameBuffer_(&this->nn_buffer, nn_buffer);
    }

    virtual ~FrameBufferCtr();
    virtual void clear(u32 clr_flag, const Color4f& color, f32 depth, u32 stencil) const;
    virtual void bindImpl_() const;

    static void copyFrameBuffer_(nn::gr::CTR::FrameBuffer* src, const nn::gr::CTR::FrameBuffer& dst);
    static bool isSameFrameBuffer_(nn::gr::CTR::FrameBuffer* src, const nn::gr::CTR::FrameBuffer& dst);

    mutable nn::gr::CTR::FrameBuffer nn_buffer;

    static nn::gr::CTR::FrameBuffer sCurrentBuffer;

    static nn::gr::CTR::FrameBuffer* getBoundFrameBufferGR() { return &sCurrentBuffer; }
};

inline bool FrameBufferCtr::isSameFrameBuffer_(nn::gr::CTR::FrameBuffer* src, const nn::gr::CTR::FrameBuffer& dst)
{
    if(src->colorBuffer.virtualAddr == dst.colorBuffer.virtualAddr)
    {
        if(src->colorBuffer.format == dst.colorBuffer.format)
        {
            if(src->colorBuffer.blockSize == dst.colorBuffer.blockSize)
            {
                if(src->colorBuffer.width == dst.colorBuffer.width)
                {
                    if(src->colorBuffer.height == dst.colorBuffer.height)
                    {
                        if(src->colorBuffer.clearColor[0] == dst.colorBuffer.clearColor[0])
                        {
                            // all of this shit just to return true
                            return true;
                        }
                    }
                }
            }
        }
    }
    return false;
}

inline void FrameBufferCtr::copyFrameBuffer_(nn::gr::CTR::FrameBuffer* src, const nn::gr::CTR::FrameBuffer& dst)
{
    src->colorBuffer.virtualAddr = dst.colorBuffer.virtualAddr;
    src->colorBuffer.format = dst.colorBuffer.format;
    src->colorBuffer.blockSize = dst.colorBuffer.blockSize;
    src->colorBuffer.width = dst.colorBuffer.width;
    src->colorBuffer.height = dst.colorBuffer.height;
    MemUtil::copy(src->colorBuffer.clearColor, dst.colorBuffer.clearColor, sizeof(dst.colorBuffer.clearColor));

    src->depthStencilBuffer.virtualAddr = dst.depthStencilBuffer.virtualAddr;
    src->depthStencilBuffer.format = dst.depthStencilBuffer.format;
    src->depthStencilBuffer.width = dst.depthStencilBuffer.width;
    src->depthStencilBuffer.height = dst.depthStencilBuffer.height;
    src->depthStencilBuffer.clearStencil = dst.depthStencilBuffer.clearStencil;

    src->width = dst.width;
    src->height = dst.height;
}
}
