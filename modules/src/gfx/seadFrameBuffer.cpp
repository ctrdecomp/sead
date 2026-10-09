#include <gfx/seadFrameBuffer.h>

namespace sead
{
FrameBuffer* FrameBuffer::sBoundFrameBuffer;

LogicalFrameBuffer::~LogicalFrameBuffer()
{
}

FrameBuffer::~FrameBuffer()
{ 
}

void FrameBuffer::clearMRT(u32, const Color4f&) const 
{
}

void FrameBuffer::bind() const
{
    bindImpl_();
}
}  // namespace sead

