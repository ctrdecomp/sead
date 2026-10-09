#pragma once

#include <controller/ctr/seadCtrHidDeviceCtr.h>

#ifdef SEAD_DEBUG
namespace sead
{
class CtrDebugController : public Controller{
    SEAD_RTTI_OVERRIDE(CtrDebugController, Controller);
public:
    CtrDebugController(ControllerMgr* mgr);
    virtual ~CtrDebugController(){ }
    virtual bool isConnected() const;
    virtual void calcImpl_();
};

}
#endif
