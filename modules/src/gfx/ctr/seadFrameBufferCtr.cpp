// Filename: seadFrameBufferCtr.cpp
//
// Project: StandardEAD C++ Library for CTR

#include "gfx/ctr/seadFrameBufferCtr.h"

namespace sead
{
nn::gr::CTR::FrameBuffer FrameBufferCtr::sCurrentBuffer;

FrameBufferCtr::~FrameBufferCtr()
{
}

void FrameBufferCtr::clear(u32 clr_flag, const Color4f& color, f32 depth, u32 stencil) const
{
    u32 buffer_bit = 0;

    if (clr_flag & FrameBuffer::cColor)
        buffer_bit |= nn::gr::CTR::FrameBuffer::COLOR_BUFFER_BIT;

    if (clr_flag & FrameBuffer::cDepth)
        buffer_bit |= nn::gr::CTR::FrameBuffer::DEPTH_BUFFER_BIT;

    if (clr_flag & FrameBuffer::cStencil)
        buffer_bit |= nn::gr::CTR::FrameBuffer::STENCIL_BUFFER_BIT;

    MemUtil::copy(nn_buffer.colorBuffer.clearColor, color.c.c, sizeof(sizeof(color.c.c[4])));

    nn_buffer.depthStencilBuffer.clearDepth = depth;
    nn_buffer.depthStencilBuffer.clearStencil = stencil;

    nngxFlush3DCommand();
    nn_buffer.MakeClearRequest(buffer_bit);
}

void FrameBufferCtr::bindImpl_() const
{
    if(isSameFrameBuffer_(&sCurrentBuffer, nn_buffer) == false)
    {
        GLint commandBuffer;
        nngxGetCmdlistParameteri(NN_GX_CMDLIST_CURRENT_BUFADDR, &commandBuffer);
        bit32 commandStart = reinterpret_cast<bit32>(&commandBuffer);
        commandBuffer = (s32)nn_buffer.MakeCommand(reinterpret_cast<bit32*>(&commandBuffer), 7, true);
        nngxMoveCommandbufferPointer(static_cast<s32>((commandBuffer - commandStart) * sizeof(bit32)));
        copyFrameBuffer_(&sCurrentBuffer, nn_buffer);
    }
}

}
