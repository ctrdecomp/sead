#ifndef SEAD_FILEDEVICEMGR_H_
#define SEAD_FILEDEVICEMGR_H_

#include <nn/fs.h>

#include <basis/seadTypes.h>
#include <container/seadTList.h>
#include <filedevice/seadFileDevice.h>
#include <filedevice/seadMainFileDevice.h>
#include <heap/seadDisposer.h>
#include <prim/seadSafeString.h>

namespace sead
{
class FileDeviceMgr
{
    SEAD_SINGLETON_DISPOSER(FileDeviceMgr)
    FileDeviceMgr();
    ~FileDeviceMgr();

public:
    void traceFilePath(const SafeString& path) const;
    void traceDirectoryPath(const SafeString& path) const;
    void resolveFilePath(BufferedSafeString* out, const SafeString& path) const;
    void resolveDirectoryPath(BufferedSafeString* out, const SafeString& path) const;

    FileDevice* open(FileHandle* handle, const SafeString& filename, FileDevice::FileOpenFlag flag, u32 divNum);
    FileDevice* openDirectory(DirectoryHandle* handle, const SafeString& dirname);
    u8* load(FileDevice::LoadArg& arg);
    bool save(FileDevice::SaveArg& arg);

    void mount(FileDevice* device, const SafeString& name = SafeString::cEmptyString);
    void unmount(FileDevice* device);
    void unmount(const SafeString& name);
    FileDevice* findDeviceFromPath(const SafeString& path, BufferedSafeString* pathNoDrive) const;
    FileDevice* findDevice(const SafeString& name) const;

    FileDevice* tryOpen(FileHandle* handle, const SafeString& path, FileDevice::FileOpenFlag flag,
                        u32 divSize);
    FileDevice* tryOpenDirectory(DirectoryHandle* handle, const SafeString& path);

    u8* tryLoad(FileDevice::LoadArg& arg);
    void unload(u8* data);
    bool trySave(FileDevice::SaveArg& arg);

    void mountSaveDataForDebug(Heap* heap);
    void unmountSaveDataForDebug();

    FileDevice* getMainFileDevice() const { return mMainFileDevice; }
    FileDevice* getDefaultFileDevice() const { return mDefaultFileDevice; }
    void setDefaultFileDevice(FileDevice* device) { mDefaultFileDevice = device; }

private:
    typedef TList<FileDevice*> DeviceList;

    DeviceList mDeviceList;
    FileDevice* mDefaultFileDevice;
    MainFileDevice* mMainFileDevice;
    u8* mRomMemory;
};

inline FileDevice* FileDeviceMgr::open(FileHandle* handle, const SafeString& filename, FileDevice::FileOpenFlag flag, u32 divNum)
{
    FileDevice* ret = tryOpen(handle, filename, flag, divNum);
    SEAD_ASSERT_MSG(ret, "open failed. [%s]", filename.cstr());
    return ret;
}

inline FileDevice* FileDeviceMgr::openDirectory(DirectoryHandle* handle, const SafeString& dirname)
{
    FileDevice* ret = tryOpenDirectory(handle, dirname);
    SEAD_ASSERT_MSG(ret, "open directory failed. [%s]", dirname.cstr());
    return ret;
}

inline u8* FileDeviceMgr::load(FileDevice::LoadArg& arg)
{
    u8* ret = tryLoad(arg);
    SEAD_ASSERT_MSG(ret, "load failed. [%s]", arg.path.cstr());
    return ret;
}

inline bool FileDeviceMgr::save(FileDevice::SaveArg& arg)
{
    bool success = trySave(arg);
    SEAD_ASSERT_MSG(success, "save failed. [%s]", arg.path.cstr());
    return success;
}

}  // namespace sead

#endif  // SEAD_FILEDEVICEMGR_H_

