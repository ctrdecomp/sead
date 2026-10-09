#ifndef SEAD_ARCHIVE_RES_H_
#define SEAD_ARCHIVE_RES_H_

#include <basis/seadAssert.h>
#include <basis/seadTypes.h>
#include <container/seadSafeArray.h>
#include <filedevice/seadFileDevice.h>
#include <heap/seadHeap.h>
#include <prim/seadSafeString.h>
#include <resource/seadResource.h>

namespace sead
{
class ArchiveRes : public DirectResource
{
    SEAD_RTTI_OVERRIDE(ArchiveRes, DirectResource)

public:
    struct FileInfo
    {
        u32 mStartOffset;
        u32 mLength;
    };

public:
    ArchiveRes(): 
        DirectResource(), 
        mEnable(false) 
    {
    }
    virtual ~ArchiveRes()
    {
    }

    virtual s32 getLoadDataAlignment() const { return 0x80; }
    virtual void doCreate_(u8* buf, u32, Heap*);

    const void* getFile(const SafeString& file_path, FileInfo* info = nullptr)
    {
        SEAD_ASSERT(mEnable);
        return getFileImpl_(file_path, info);
    }

    const void* getFileFast(const s32 entry_id, FileInfo* info)
    {
        SEAD_ASSERT(mEnable);
        return getFileFastImpl_(entry_id, info);
    }

    s32 convertPathToEntryID(const SafeString& path)
    {
        SEAD_ASSERT(mEnable);
        return convertPathToEntryIDImpl_(path);
    }

    bool setCurrentDirectory(const SafeString& dir)
    {
        SEAD_ASSERT(mEnable);
        return setCurrentDirectoryImpl_(dir);
    }

    bool openDirectory(HandleBuffer* handle, const SafeString& dir)
    {
        return openDirectoryImpl_(handle, dir);
    }

    bool closeDirectory(HandleBuffer* handle)
    {
        return closeDirectoryImpl_(handle);
    }

    u32 readDirectory(HandleBuffer* handle, DirectoryEntry* entries, u32 num)
    {
        return readDirectoryImpl_(handle, entries, num);
    }

protected:
    virtual const void* getFileImpl_(const SafeString& file_path, FileInfo* file_info = nullptr) = 0;
    virtual const void* getFileFastImpl_(s32 entry_id, FileInfo* file_info) = 0;
    virtual s32 convertPathToEntryIDImpl_(const SafeString& file_path) = 0;
    virtual bool setCurrentDirectoryImpl_(const SafeString&) = 0;
    virtual bool openDirectoryImpl_(HandleBuffer* handle, const SafeString& path) = 0;
    virtual bool closeDirectoryImpl_(HandleBuffer* handle) = 0;
    virtual u32 readDirectoryImpl_(HandleBuffer* handle, DirectoryEntry* entries, u32 num) = 0;
    virtual bool prepareArchive_(const void* archive) = 0;

    bool mEnable;
};

}  // namespace sead

#endif  // SEAD_ARCHIVE_RES_H_

