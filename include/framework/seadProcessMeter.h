#pragma once

#include <container/seadOffsetList.h>
#include <framework/seadTask.h>
#include <framework/seadTaskMgr.h>
#include <gfx/seadColor.h>
#include <gfx/seadGraphicsContext.h>
#include <gfx/seadPrimitiveRenderer.h>
#include <gfx/seadTextWriter.h>
#include <gfx/seadViewport.h>
#include <time/seadTickTime.h>

namespace sead
{
class ProcessMeterBarBase;

class ProcessMeter : public Task
{
    SEAD_RTTI_OVERRIDE(ProcessMeter, Task);
    SEAD_TASK_SINGLETON_DISPOSER(ProcessMeter);

public:
    typedef OffsetList<ProcessMeterBarBase> BarList;

public:
    explicit ProcessMeter(const TaskConstructArg& arg);
    virtual ~ProcessMeter();

    virtual void calc();
    virtual void draw();

    void measureBeginFrame();
    void measureEndFrame();
    void attachProcessMeterBar(ProcessMeterBarBase* meter);
    void detachProcessMeterBar(ProcessMeterBarBase* meter);
    void setVisible(bool visible);
    bool isVisible() const { return mVisible; }

protected:
    f32 calcTimeLinePos_(const Viewport& vp, u32 sectionNum, TickTime t);
    f32 calcTimeLineWidth_(const Viewport& vp, u32 sectionNum, TickSpan t);
    u32 calcMaxSectionNum_();

    void drawHorizontalMode_(const Viewport& viewport);
    void drawVerticalMode_(const Viewport& viewport);

protected:
    Viewport mViewport;
    TickSpan mSectionTime;
    u32 mSectionNum;
    TickTime mFrameBegin;
    TickSpan mFrameSpan;
    TickTime mCurFrameBegin;
    GraphicsContext mGraphicsContext;
    BarList mBarList;
    bool mVisible;
    bool mTextVisible;
    bool mVerticalMode;
    bool mShowQuaterMark;
    f32 mBarHeight;
    bool mTextFix;
    Color4f mBorderColor;
    Color4f mBackColor;
    Color4f mTextColor;
    u32 mCurSectionNum;
    s32 mAdjustTime;
    s32 mAdjustCounter;
    u32 mMinSectionNum;
    u32 mMaxSectionSeparatorNum;
};
}  // namespace sead

