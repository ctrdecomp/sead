#include <framework/seadProcessMeter.h>

#include <framework/seadFramework.h>
#include <framework/seadProcessMeterBar.h>
#include <gfx/seadCamera.h>
#include <gfx/seadPrimitiveDrawer.h>
#include <gfx/seadProjection.h>
#include <gfx/seadTextWriter.h>

namespace sead {

SEAD_TASK_SINGLETON_DISPOSER_IMPL(ProcessMeter);

ProcessMeter::ProcessMeter(const TaskConstructArg& arg): 
    Task(arg, "sead::ProcessMeter"), 
    mViewport(*getFramework()->getMethodLogicalFrameBuffer(2)), 
    mSectionTime(TickSpan::makeFromMicroSeconds(16666)), 
    mSectionNum(0), 
    mFrameBegin(), 
    mFrameSpan(0), 
    mCurFrameBegin(), 
    mBarList(), 
    mVisible(false), 
    mTextVisible(true), 
    mVerticalMode(false), 
    mShowQuaterMark(true), 
    mBarHeight(16.0f), 
    mTextFix(true), 
    mBorderColor(Color4f::cWhite), 
    mBackColor(0.0f, 0.0f, 0.0f, 0.5f), 
    mTextColor(Color4f::cWhite), 
    mCurSectionNum(mSectionNum), 
    mAdjustTime(60), 
    mAdjustCounter(mAdjustTime), 
    mMinSectionNum(1), 
    mMaxSectionSeparatorNum(3)
{
    mViewport.getMax().y = 32.0f;
    mBarList.initOffset(ProcessMeterBarBase::getListNodeOffset());
}

ProcessMeter::~ProcessMeter()
{
    while (!mBarList.isEmpty())
    {
        detachProcessMeterBar(mBarList.front());
    }
}

void ProcessMeter::calc()
{
    if (mSectionNum != 0)
        return;

    u32 curNum = calcMaxSectionNum_();
    if (curNum >= mCurSectionNum)
    {
        mAdjustCounter = mAdjustTime;
        mCurSectionNum = curNum;
    }
    else
    {
        if (mAdjustCounter > 0)
            mAdjustCounter--;
        else
            mCurSectionNum = curNum;
    }
}

void ProcessMeter::draw()
{
    if (!mVisible)
        return;

    SEAD_ASSERT_MSG(PrimitiveRenderer::instance(), "PrimitiveRenderer must be prepared.");

    mGraphicsContext.apply();
    
    Viewport vp(mViewport);

    if (mBarHeight > 0.0f)
    {
        f32 height = mBarList.size() > 0 ? mBarHeight * mBarList.size() : mBarHeight;

        if (mVerticalMode)
        {
            if (mViewport.getMax().x < mViewport.getMin().x + height)
                vp.setMax(Vector2f(mViewport.getMin().x + height, mViewport.getMax().y));
        }
        else
        {
            if (mViewport.getMax().y < mViewport.getMin().y + height)
                vp.setMax(Vector2f(mViewport.getMax().x, mViewport.getMin().y + height));
        }
    }
    PrimitiveRenderer* renderer = PrimitiveRenderer::instance();
    vp.apply(*getFramework()->getMethodLogicalFrameBuffer(2));

    OrthoProjection proj(0.0f, 1.0f, vp);
    OrthoCamera camera(proj);
    camera.updateMatrix();

    renderer->setProjection(proj);
    renderer->setCamera(camera);
    renderer->setModelMatrix(Matrix34f::ident);

    if (mVerticalMode)
    {
        drawVerticalMode_(vp);
    }
    else
    {
        drawHorizontalMode_(vp);
    }
}

void ProcessMeter::measureBeginFrame()
{
    mCurFrameBegin.setNow();
}

void ProcessMeter::measureEndFrame()
{
    for (OffsetList<ProcessMeterBarBase>::iterator it = mBarList.begin(); it != mBarList.end(); ++it)
    {
        ProcessMeterBarBase& bar = *it;
        bar.onEndFrame();
    }

    mFrameSpan = mCurFrameBegin.diffToNow();
    mFrameBegin = mCurFrameBegin;
}

void ProcessMeter::attachProcessMeterBar(ProcessMeterBarBase* meter)
{
    SEAD_ASSERT(meter);

    if (!meter->mParent)
    {
        mBarList.pushBack(meter);
        meter->setParentProcessMeter(this);
    }
}

void ProcessMeter::detachProcessMeterBar(ProcessMeterBarBase* meter)
{
    SEAD_ASSERT(meter);

    mBarList.erase(meter);
    meter->setParentProcessMeter(nullptr);
}

void ProcessMeter::setVisible(bool visible)
{
    mVisible = visible;
}

f32 ProcessMeter::calcTimeLinePos_(const Viewport& vp, u32 sectionNum, TickTime t)
{
    return calcTimeLineWidth_(vp, sectionNum, t.diff(mFrameBegin));
}

f32 ProcessMeter::calcTimeLineWidth_(const Viewport& vp, u32 sectionNum, TickSpan t)
{
    SEAD_ASSERT(sectionNum > 0);

    TickSpan totalSpan = mSectionTime * static_cast<f32>(sectionNum);

    f32 size = mVerticalMode ? vp.getSizeY() : vp.getSizeX();

    return static_cast<f32>(t.toS64()) * size / static_cast<f32>(totalSpan.toS64());
}

u32 ProcessMeter::calcMaxSectionNum_()
{
    u32 sectionNum = static_cast<u32>(mFrameSpan.toS64() / mSectionTime.toS64()) + 1;

    for (OffsetList<ProcessMeterBarBase>::iterator it = mBarList.begin(); it != mBarList.end(); ++it)
    {
        ProcessMeterBarBase& bar = *it;
        TickSpan span = bar.getLastFinalEnd() - mFrameBegin;

        if (span > 0)
        {
            u32 secnum = static_cast<u32>(span.toS64() / mSectionTime.toS64()) + 1;
            if (sectionNum < secnum)
                sectionNum = secnum;
        }
    }

    return sectionNum;
}

void ProcessMeter::drawHorizontalMode_(const Viewport& viewport)
{
    // TODO
}

void ProcessMeter::drawVerticalMode_(const Viewport& viewport)
{
    // TODO
}

} // namespace sead