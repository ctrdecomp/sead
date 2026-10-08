#include <nn/fs.h>
#include <nn/fs/CTR/MPCore/fs_FileSystemBase.h>

#include <basis/seadNew.h>
#include <basis/seadAssert.h>
#include <basis/seadWarning.h>
#include <devenv/seadEnvUtil.h>
#include <filedevice/seadFileDeviceMgr.h>
#include <filedevice/seadPath.h>
#include <heap/seadHeapMgr.h>

namespace sead
{
SEAD_SINGLETON_DISPOSER_IMPL(FileDeviceMgr)

FileDeviceMgr::FileDeviceMgr():
    mDeviceList(),
    mDefaultFileDevice(NULL),
    mMainFileDevice(NULL)
{
    if (HeapMgr::sInstancePtr == NULL)
    {
        SEAD_ASSERT_MSG(false, "FileDeviceMgr need HeapMgr");
        return;
    }

    Heap* const heap = HeapMgr::instance()->findContainHeap(this);

    nn::fs::Initialize();
    int romArchiveSize = nn::fs::GetRomRequiredMemorySize(16, 16, true);
    SEAD_ASSERT_MSG(0 <= romArchiveSize, "Cannot mount rom archive.(%d)", romArchiveSize);
    mRomMemory = new (heap) u8[romArchiveSize];

    MountRom(16, 16, mRomMemory, romArchiveSize, true);

    mMainFileDevice = new (heap) MainFileDevice(heap);
    mount(mMainFileDevice);

    mDefaultFileDevice = mMainFileDevice;
}

FileDeviceMgr::~FileDeviceMgr()
{
    if (mMainFileDevice != NULL)
    {
        delete mMainFileDevice;
        mMainFileDevice = NULL;
    }
    Result ret = nn::fs::Unmount("rom");
    SEAD_ASSERT(ret.IsSuccess());
    delete[] mRomMemory;
}

void FileDeviceMgr::traceFilePath(const SafeString& path) const
{
    SEAD_PRINT("[FileDeviceMgr] %s\n", path.cstr());
    FixedSafeString<256> pathNoDrive;
    FileDevice* device = findDeviceFromPath(path, &pathNoDrive);

    if (device != NULL)
        device->traceFilePath(pathNoDrive);
    else
        SEAD_WARNING("FileDevice not found: %s", path.cstr());
}

void FileDeviceMgr::traceDirectoryPath(const SafeString& path) const
{
    SEAD_PRINT("[FileDeviceMgr] %s\n", path.cstr());
    FixedSafeString<256> pathNoDrive;
    FileDevice* device = findDeviceFromPath(path, &pathNoDrive);

    if (device != NULL)
        device->traceDirectoryPath(pathNoDrive);
    else
        SEAD_WARNING("FileDevice not found: %s", path.cstr());
}

void FileDeviceMgr::resolveFilePath(BufferedSafeString* out, const SafeString& path) const
{
    FixedSafeString<256> pathNoDrive;
    FileDevice* device = findDeviceFromPath(path, &pathNoDrive);

    if (device != NULL)
        device->resolveFilePath(out, pathNoDrive);
    else
        SEAD_WARNING("FileDevice not found: %s", path.cstr());
}

void FileDeviceMgr::resolveDirectoryPath(BufferedSafeString* out, const SafeString& path) const
{
    FixedSafeString<256> pathNoDrive;
    FileDevice* device = findDeviceFromPath(path, &pathNoDrive);

    if (device != NULL)
        device->resolveDirectoryPath(out, pathNoDrive);
    else
        SEAD_WARNING("FileDevice not found: %s", path.cstr());
}

void FileDeviceMgr::mount(FileDevice* device, const SafeString& name)
{
    if (!name.isEqual(SafeString::cEmptyString))
        device->setDriveName(name);

    mDeviceList.pushBack(device);
}

void FileDeviceMgr::unmount(FileDevice* device)
{
    mDeviceList.erase(device);

    if (device == mDefaultFileDevice)
        mDefaultFileDevice = NULL;
}

void FileDeviceMgr::unmount(const SafeString& name)
{
    FileDevice* device = findDevice(name);
    if (!device)
    {
        SEAD_ASSERT_MSG(false, "drive not found: %s\n", name.cstr());
        return;
    }
    unmount(device);
}

FileDevice* FileDeviceMgr::findDeviceFromPath(const SafeString& path,
                                              BufferedSafeString* pathNoDrive) const
{
    FixedSafeString<32> driveName;
    FileDevice* device;

    if (!Path::getDriveName(&driveName, path))
    {
        device = mDefaultFileDevice;
        if (!device)
        {
            SEAD_ASSERT_MSG(false, "drive name not found and default file device is null");
            return nullptr;
        }
    }
    else
        device = findDevice(driveName);

    if (!device)
        return nullptr;

    if (pathNoDrive != NULL)
        Path::getPathExceptDrive(pathNoDrive, path);

    return device;
}

FileDevice* FileDeviceMgr::findDevice(const SafeString& name) const
{
    for (DeviceList::iterator it = mDeviceList.begin(); it != mDeviceList.end(); ++it)
        if ((*it)->getDriveName() == name)
            return *it;

    return nullptr;
}

FileDevice* FileDeviceMgr::tryOpen(FileHandle* handle, const SafeString& path,
                                   FileDevice::FileOpenFlag flag, u32 divSize)
{
    FixedSafeString<256> pathNoDrive;
    FileDevice* device = findDeviceFromPath(path, &pathNoDrive);

    if (device == NULL)
        return NULL;

    return device->tryOpen(handle, pathNoDrive, flag, divSize);
}

FileDevice* FileDeviceMgr::tryOpenDirectory(DirectoryHandle* handle, const SafeString& path)
{
    FixedSafeString<256> pathNoDrive;
    FileDevice* device = findDeviceFromPath(path, &pathNoDrive);
    if (!device)
        return nullptr;

    if (!device->isExistDirectory(pathNoDrive))
        return nullptr;

    return device->tryOpenDirectory(handle, pathNoDrive);
}

u8* FileDeviceMgr::tryLoad(FileDevice::LoadArg& arg)
{
    SEAD_ASSERT_MSG(arg.path != SafeString::cEmptyString, "path is null");

    FixedSafeString<256> pathNoDrive;
    FileDevice* device = findDeviceFromPath(arg.path, &pathNoDrive);

    if (device == NULL)
        return NULL;

    FileDevice::LoadArg arg2(arg);
    arg2.path = pathNoDrive.cstr();

    u8* data = device->tryLoad(arg2);

    arg.read_size = arg2.read_size;
    arg.roundup_size = arg2.roundup_size;
    arg.need_unload = arg2.need_unload;

    return data;
}

void FileDeviceMgr::unload(u8* data)
{
    SEAD_ASSERT(data);
    if (data)
        delete data;
}

bool FileDeviceMgr::trySave(FileDevice::SaveArg& arg)
{
    SEAD_ASSERT_MSG(arg.path != SafeString::cEmptyString, "path is null");

    FixedSafeString<256> pathNoDrive;
    FileDevice* device = findDeviceFromPath(arg.path, &pathNoDrive);
    if (!device)
        return false;

    FileDevice::SaveArg arg2(arg);
    arg2.path = pathNoDrive.cstr();

    const bool ret = device->trySave(arg2);
    arg.write_size = arg2.write_size;
    return ret;
}

}  // namespace sead
