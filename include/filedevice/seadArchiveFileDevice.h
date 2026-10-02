#pragma once

#include "filedevice/seadFileDevice.h"

namespace sead{
class ArchiveRes;

class ArchiveFileDevice : public FileDevice{
    SEAD_RTTI_OVERRIDE(ArchiveFileDevice, FileDevice)
public:
    explicit ArchiveFileDevice(ArchiveRes* archive_res): 
        FileDevice("arc"), 
        mArchive(archive_res)
    {
    }
    virtual ~ArchiveFileDevice()
    {
    }

    u8* tryLoadWithEntryID(s32 id, LoadArg& arg);
    FileDevice* tryOpenWithEntryID(FileHandle* handle, s32 id, FileOpenFlag flag, u32 div_size);
    s32 tryConvertPathToEntryID(const SafeString& path);
    bool setCurrentDirectory(const SafeString& dir);

protected:
    struct ArchiveFileHandle;

    virtual bool doIsAvailable_() const { return true; }
    virtual u8* doLoad_(LoadArg& arg);
    virtual FileDevice* doOpen_(FileHandle* handle, const SafeString& path, FileOpenFlag flag);
    virtual bool doClose_(FileHandle* handle);
    virtual bool doFlush_(FileHandle* handle);
    virtual bool doRemove_(const SafeString& str);
    virtual bool doRead_(u32* bytesRead, FileHandle* handle, u8* outBuffer, u32 bytesToRead);
    virtual bool doWrite_(u32*, FileHandle*, const u8*, u32) { return false; }
    virtual bool doSeek_(FileHandle* handle, s32 offset, SeekOrigin origin);
    virtual bool doGetCurrentSeekPos_(u32* seekPos, FileHandle* handle);
    virtual bool doGetFileSize_(u32* fileSize, const SafeString& path);
    virtual bool doGetFileSize_(u32* fileSize, FileHandle* handle);
    virtual bool doIsExistFile_(bool* exists, const SafeString& path);
    virtual bool doIsExistDirectory_(bool* exists, const SafeString& path);
    virtual FileDevice* doOpenDirectory_(DirectoryHandle* handle, const SafeString& path);
    virtual bool doCloseDirectory_(DirectoryHandle* handle);
    virtual bool doReadDirectory_(u32* entriesRead, DirectoryHandle* handle, DirectoryEntry* entry,
                          u32 entriesToRead);
    virtual bool doMakeDirectory_(const SafeString& path, u32 u_32);
    virtual s32 doGetLastRawError_() const;

    virtual u8* doLoadWithEntryID_(s32 id, LoadArg& arg);
    virtual FileDevice* doOpenWithEntryID_(FileHandle* handle, s32 id, FileOpenFlag flag);
    virtual s32 doConvertPathToEntryID_(const SafeString& path);
    virtual bool doSetCurrentDirectory_(const SafeString& path);

    ArchiveFileHandle* getArchiveFileHandle_(FileHandle* handle) const;
    ArchiveFileHandle* constructArchiveFileHandle_(FileHandle* handle) const;

    ArchiveRes* mArchive;
};
}  // namespace sead
