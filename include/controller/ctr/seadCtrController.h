#pragma once

#include <controller/ctr/seadCtrHidDeviceCtr.h>

namespace sead
{
class CtrController : public Controller{
    SEAD_RTTI_OVERRIDE(CtrController, Controller);
public:
    CtrController(ControllerMgr* mgr);
    virtual ~CtrController(){ }
    virtual void calcImpl_();
    
    void setStickClampMode(PadReader::StickClampMode stickMode);
};

}
