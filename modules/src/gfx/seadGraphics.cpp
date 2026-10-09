#include <gfx/seadGraphics.h>

#include <gfx/seadDrawLockContext.h>
#include <gfx/seadFrameBuffer.h>
#include <hostio/seadHostIOFramework.h>
#include <hostio/seadHostIOMgr.h>
#include <hostio/seadHostIORoot.h>

namespace sead {

Graphics* Graphics::sInstance = nullptr;

Graphics::DevicePosture Graphics::sDefaultDevicePosture = cDevicePosture_Same;
f32 Graphics::sDefaultDeviceZScale = 1.0f;
f32 Graphics::sDefaultDeviceZOffset = 0.0f;

Graphics::Graphics(): 
    IDisposer(), 
    mContextLockFunc(nullptr), 
    mDrawLockContext(nullptr)
{
}

Graphics::~Graphics()
{
}

void Graphics::initialize(Heap* heap)
{
    mDrawLockContext = new(heap) DrawLockContext();
    initializeDrawLockContext(heap);

    mDrawLockContext->lock();
    initializeImpl(heap);
    mDrawLockContext->unlock();
}

void Graphics::initHostIO()
{
#if defined(SEAD_TARGET_DEBUG)
    hostio::AddNode(HostIOMgr::instance()->getSeadRoot(), "DrawLock", mDrawLockContext, "Icon=NOTE");
#endif // SEAD_TARGET_DEBUG
}

void Graphics::lockDrawContext()
{
    if (mContextLockFunc)
        mContextLockFunc(true);
    else
        mDrawLockContext->lock();
}

void Graphics::unlockDrawContext()
{
    if (mContextLockFunc)
        mContextLockFunc(false);
    else
        mDrawLockContext->unlock();
}

void Graphics::initializeDrawLockContext(Heap* heap)
{
    mDrawLockContext->initialize(heap);
}

void Graphics::clear(u32 colorIdx, const Color4f& color, f32 depth, u32 stencil)
{
    FrameBuffer* fb = FrameBuffer::getBoundFrameBuffer();
    SEAD_ASSERT(fb);
    fb->clear(colorIdx, color, depth, stencil);
}

} // namespace sead
