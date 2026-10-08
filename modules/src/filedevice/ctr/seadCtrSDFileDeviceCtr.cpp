// Filename: seadCtrSDFileDeviceCtr.cpp
//
// Project: StandardEAD C++ Library for CTR

#include "filedevice/ctr/seadCtrSDFileDeviceCtr.h"

namespace sead
{
bool CtrSDFileDevice::doIsExistFile_(bool* exists, const SafeString& path)
{
    nn::fs::Directory dir;
    nn_result = openDirectryImpl_(&dir, getArchiveName_(), path);

    if(nn_result.IsSuccess())
    {
        exists = NULL;
        return true;
    }
    
    if(nn::fs::ResultNotFound().Includes(nn_result))
    {
        FileStream fstream;
        nn_result = openFileStreamImpl_(&fstream, getArchiveName_(), path, nn::fs::OPEN_MODE_READ);

        if(nn_result.IsSuccess())
        {
            *exists = true;
            return true;
        }
        if(nn::fs::ResultNotFound().Includes(nn_result))
        {
            *exists = false;
            return true;
        }
    }
    
    *exists = false;
    return false;
}

const char* CtrSDFileDevice::getArchiveName_() const
{
    return "sdmc";
}
}