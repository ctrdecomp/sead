// Filename: seadMainFileDevice.cpp
//
// Project: StandardEAD C++ Library for CTR

#include <filedevice/seadMainFileDevice.h>
#include <prim/seadSafeString.h>

#include <filedevice/ctr/seadCtrFileDevice.h>

namespace sead
{
MainFileDevice::MainFileDevice(Heap* heap) :
    FileDevice("main"), 
    mFileDevice(NULL)
{
    mFileDevice = new (heap) CtrFileDevice();
    SEAD_ASSERT(mFileDevice);
}

MainFileDevice::~MainFileDevice()
{
    if (mFileDevice == NULL)
        return;

    delete mFileDevice;
    mFileDevice = NULL;
}

void MainFileDevice::traceFilePath(const SafeString& path) const
{
    mFileDevice->FileDevice::traceFilePath(path);
}

void MainFileDevice::traceDirectoryPath(const SafeString& path) const
{
    mFileDevice->FileDevice::traceDirectoryPath(path);
}

void MainFileDevice::resolveFilePath(BufferedSafeString* out, const SafeString& path) const
{
    mFileDevice->FileDevice::resolveFilePath(out, path);
}

void MainFileDevice::resolveDirectoryPath(BufferedSafeString* out, const SafeString& path) const
{
    mFileDevice->FileDevice::resolveDirectoryPath(out, path);
}
}  // namespace sead
