// Filename: seadSeriesGameFrameworkCtrNw4c.cpp
//
// Project: StandardEAD C++ Library for CTR

#include <framework/ctr_nw4c/seadSeriesGameFrameworkCtrNw4c.h>
#include <framework/seadProcessMeter.h>

namespace sead
{
SeriesGameFrameworkCtrNw4c::SeriesGameFrameworkCtrNw4c(const CreateArg& arg):
    GameFrameworkCtrNw4c(arg)
{
}

SeriesGameFrameworkCtrNw4c::~SeriesGameFrameworkCtrNw4c()
{
}

void SeriesGameFrameworkCtrNw4c::initialize(const Framework::InitializeArg& arg)
{
    GameFrameworkCtrNw4c::initialize(arg);
}

void SeriesGameFrameworkCtrNw4c::mainLoop_()
{
    mVblinkBuf = nngxCheckVSync(NN_GX_DISPLAY_BOTH);
    mFrameNow.setNow();
    mLastDiffTime.setNow();

    for(;;)
    {
        procFrame_();
    }
}

void SeriesGameFrameworkCtrNw4c::procFrame_()
{
    ProcessMeter* proc = ProcessMeter::instance();
    if (proc)
        proc->measureBeginFrame();

    Graphics::instance()->lockDrawContext();
    {
        nngxStopCmdlist();
        procDraw_();
        procCalc_();
        procReset_();
        swapBuffer_();
    }
    Graphics::instance()->unlockDrawContext();

    if (proc)
        proc->measureEndFrame();

    mLastUpdateTime = mFrameNow.diffToNow();
    mFrameNow.setNow();
    waitForVBlank_();

    if(getDisplayState() == cReady)
    {
        nngxStartLcdDisplay();

        mDisplayState = cShow;
    }
}
} // namespace sead