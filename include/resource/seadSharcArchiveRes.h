#ifndef SEAD_SHARC_ARCHIVE_RES_H_
#define SEAD_SHARC_ARCHIVE_RES_H_

#include <basis/seadTypes.h>
#include <container/seadBuffer.h>
#include <filedevice/seadFileDevice.h>
#include <prim/seadEndian.h>
#include <prim/seadSafeString.h>
#include <resource/seadArchiveRes.h>

namespace sead
{
class SharcArchiveRes : public ArchiveRes
{
    SEAD_RTTI_OVERRIDE(SharcArchiveRes, ArchiveRes)

public:
    struct ArchiveBlockHeader
    {
        char signature[4];
        u16 header_size;
        u16 byte_order;
        u32 file_size;
        u32 data_block_offset;
        u16 version;
        u16 reserved;
    };

    struct FATBlockHeader
    {
        char signature[4];
        u16 header_size;
        u16 file_num;
        u32 hash_key;
    };

    struct FATEntry
    {
        u32 hash;
        u32 name_offset;
        u32 data_start_offset;
        u32 data_end_offset;
    };

    struct FNTBlockHeader
    {
        char signature[4];
        u16 header_size;
        u16 reserved;
    };

    struct HandleInner;

public:
    SharcArchiveRes();
    virtual ~SharcArchiveRes();

    virtual const void* getFileImpl_(const SafeString& file_path, FileInfo* file_info = NULL);
    virtual const void* getFileFastImpl_(s32 entry_id, FileInfo* file_info);
    virtual s32 convertPathToEntryIDImpl_(const SafeString& file_path);
    virtual bool setCurrentDirectoryImpl_(const SafeString&);
    virtual bool openDirectoryImpl_(HandleBuffer* handle, const SafeString& path);
    virtual bool closeDirectoryImpl_(HandleBuffer* handle) { return true; }
    virtual u32 readDirectoryImpl_(HandleBuffer* handle, DirectoryEntry* entry, u32 num);
    virtual bool prepareArchive_(const void* archive);

protected:
    static const u32 cArchiveVersion = 0x100;
    static const u32 cArchiveEntryMax = 0x3fff;
    static const u32 cFileNameTableAlign = 4;

    const ArchiveBlockHeader* mArchiveBlockHeader;
    const FATBlockHeader* mFATBlockHeader;
    const char* mFNTBlock;
    Buffer<const FATEntry> mFATEntrys;
    const u8* mDataBlock;
    Endian::Types mEndianType;
};

}  // namespace sead

#endif  // SEAD_SHARC_ARCHIVE_RES_H_

