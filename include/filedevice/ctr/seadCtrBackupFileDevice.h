#pragma once

#include "filedevice/ctr/seadCtrFileStreamFileDeviceCtr.h"

namespace sead
{
class CtrBackupFileDevice : public CtrFileStreamFileDevice
{
    SEAD_RTTI_OVERRIDE(CtrBackupFileDevice, CtrFileStreamFileDevice)
public:
    CtrBackupFileDevice():
        CtrFileStreamFileDevice("backup")
    {
    }

    virtual ~CtrBackupFileDevice(){ }
protected:
    virtual const char* getArchiveName_() const;
};
}