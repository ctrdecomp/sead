// Filename: seadCtrBackupFileDevice.cpp
//
// Project: StandardEAD C++ Library for CTR

#include "filedevice/ctr/seadCtrBackupFileDevice.h"

namespace sead
{
const char* CtrBackupFileDevice::getArchiveName_() const
{
    return "data";
}
}
