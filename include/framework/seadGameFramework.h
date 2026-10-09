#pragma once

#include <framework/seadFramework.h>
#include <framework/seadProcessMeterBar.h>
#include <gfx/seadGraphics.h>

namespace sead
{
class InfLoopCheckerThread;

class GameFramework : public Framework
{
    SEAD_RTTI_OVERRIDE(GameFramework, Framework);

public:
    enum DisplayState
    {
        cHide,
        cReady,
        cShow
    };

    typedef void (*ProcDrawCallback)(bool);

    static void initialize(const Framework::InitializeArg&);

    GameFramework();
    
    virtual ~GameFramework();
    virtual void createSystemTasks(TaskBase* base, 
        const Framework::CreateSystemTaskArg& createSystemTaskArg);
    virtual void createControllerMgr(TaskBase* base);
    virtual void createHostIOMgr(TaskBase* base, HostIOMgr::Parameter* hostioParam, Heap* heap);
    virtual void createProcessMeter(TaskBase* base);
    virtual void createSeadMenuMgr(TaskBase* base);
    virtual void createInfLoopChecker(TaskBase* base, const TickSpan&, int);
    virtual float calcFps() = 0;
    virtual void saveScreenShot(const SafeString&) 
    {
        SEAD_ASSERT_MSG(false, "Not implement.");
    }
    virtual bool isScreenShotBusy() const { return false; }

    void startDisplay();
    void lockFrameDrawContext();
    void unlockFrameDrawContext();

    DisplayState getDisplayState() const { return mDisplayState; }
    MultiProcessMeterBar<32>& getCalcMeter() { return mCalcMeter; }
    MultiProcessMeterBar<32>& getDrawMeter() { return mDrawMeter; }
    MultiProcessMeterBar<32>& getGPUMeter() { return mGPUMeter; }

protected:
    virtual void waitStartDisplayLoop_();

    DisplayState mDisplayState;
    MultiProcessMeterBar<32> mCalcMeter;
    MultiProcessMeterBar<32> mDrawMeter;
    MultiProcessMeterBar<32> mGPUMeter;
    InfLoopCheckerThread* mCheckerThread;
    Graphics::LockFunc mFrameLockFunc;
    ProcDrawCallback mProcDrawCallback;
};

}  // namespace sead

