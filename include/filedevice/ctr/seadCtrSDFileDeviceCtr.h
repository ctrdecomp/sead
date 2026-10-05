#pragma once

#include "filedevice/ctr/seadCtrFileStreamFileDeviceCtr.h"

namespace sead
{
class CtrSDFileDevice : public CtrFileStreamFileDevice
{
    SEAD_RTTI_OVERRIDE(CtrSDFileDevice, CtrFileStreamFileDevice)
public:
    CtrSDFileDevice():
        CtrFileStreamFileDevice("sd")
    {
    }
    virtual ~CtrSDFileDevice(){ }
protected:
    virtual bool doIsExistFile_(bool* exists, const SafeString& path);
    virtual const char* getArchiveName_() const;
};
}