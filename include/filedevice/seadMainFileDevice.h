#ifndef SEAD_MAIN_FILEDEVICE_H_
#define SEAD_MAIN_FILEDEVICE_H_

#include <basis/seadTypes.h>
#include <filedevice/seadFileDevice.h>
#include <prim/seadRuntimeTypeInfo.h>
#include <prim/seadSafeString.h>

namespace sead
{
class MainFileDevice : public FileDevice
{
    SEAD_RTTI_OVERRIDE(MainFileDevice, FileDevice)

public:
    explicit MainFileDevice(Heap* heap);
    virtual ~MainFileDevice();

    virtual void traceFilePath(const SafeString& path) const;
    virtual void traceDirectoryPath(const SafeString& path) const;
    virtual void resolveFilePath(BufferedSafeString* out, const SafeString& path) const;
    virtual void resolveDirectoryPath(BufferedSafeString* out, const SafeString& path) const;

protected:
    virtual bool doIsAvailable_() const { return mFileDevice->isAvailable(); }

    virtual FileDevice* doOpen_(FileHandle* handle, const SafeString& path, FileOpenFlag flag)
    {
        return mFileDevice->tryOpen(handle, path, flag, handle->getDivSize());
    }

    virtual bool doClose_(FileHandle* handle) { return mFileDevice->tryClose(handle); }

    virtual bool doRead_(u32* bytesRead, FileHandle* handle, u8* outBuffer, u32 bytesToRead)
    {
        return mFileDevice->tryRead(bytesRead, handle, outBuffer, bytesToRead);
    }

    virtual bool doWrite_(u32* bytesWritten, FileHandle* handle, const u8* inBuffer,
        u32 bytesToWrite)
    {
        return mFileDevice->tryWrite(bytesWritten, handle, inBuffer, bytesToWrite);
    }

    virtual bool doSeek_(FileHandle* handle, s32 offset, SeekOrigin origin)
    {
        return mFileDevice->trySeek(handle, offset, origin);
    }

    virtual bool doGetCurrentSeekPos_(u32* seekPos, FileHandle* handle)
    {
        return mFileDevice->tryGetCurrentSeekPos(seekPos, handle);
    }

    virtual bool doGetFileSize_(u32* fileSize, const SafeString& path)
    {
        return mFileDevice->tryGetFileSize(fileSize, path);
    }

    virtual bool doGetFileSize_(u32* fileSize, FileHandle* handle)
    {
        return mFileDevice->tryGetFileSize(fileSize, handle);
    }

    virtual bool doIsExistFile_(bool* exists, const SafeString& path)
    {
        return mFileDevice->tryIsExistFile(exists, path);
    }

    virtual bool doIsExistDirectory_(bool* exists, const SafeString& path)
    {
        return mFileDevice->tryIsExistDirectory(exists, path);
    }

    virtual FileDevice* doOpenDirectory_(DirectoryHandle* handle, const SafeString& path)
    {
        return mFileDevice->tryOpenDirectory(handle, path);
    }

    virtual bool doCloseDirectory_(DirectoryHandle* handle)
    {
        return mFileDevice->tryCloseDirectory(handle);
    }

    virtual bool doReadDirectory_(u32* entriesRead, DirectoryHandle* handle, DirectoryEntry* entries,
                          u32 entriesToRead)
    {
        return mFileDevice->tryReadDirectory(entriesRead, handle, entries, entriesToRead);
    }

    virtual bool doMakeDirectory_(const SafeString& path, u32 x)
    {
        return mFileDevice->tryMakeDirectory(path, x);
    }

    virtual s32 doGetLastRawError_() const { return mFileDevice->getLastRawError(); }

    virtual bool isMatchDevice_(const HandleBase* handle) const
    {
        return mFileDevice->isMatchDevice_(handle);
    }

    FileDevice* mFileDevice;
};

}  // namespace sead

#endif  // SEAD_MAIN_FILEDEVICE_H_
