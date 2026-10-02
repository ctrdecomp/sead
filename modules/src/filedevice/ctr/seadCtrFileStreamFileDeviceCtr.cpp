// Filename: seadCtrFileStreamFileDeviceCtr.cpp
//
// Project: StandardEAD C++ Library for CTR

#include "filedevice/ctr/seadCtrFileStreamFileDeviceCtr.h"
#include <nn/fs/fs_FileSystemBase.h>

namespace sead
{
CtrFileStreamFileDevice::CtrFileStreamFileDevice(const SafeString& name):
    FileDevice(name), mDoFlush(false)
{
}

bool CtrFileStreamFileDevice::doIsAvailable_() const
{
    nn::fs::Directory dir;
    
#ifdef MATCHING_HACK_CTR
    const_cast<CtrFileStreamFileDevice*>(this)->nn_result = const_cast<CtrFileStreamFileDevice*>(this)->openDirectryImpl_(&dir, getArchiveName_(), "");
#endif
    return nn_result.IsSuccess();
}

FileDevice* CtrFileStreamFileDevice::doOpen_(FileHandle* handle, const SafeString& path, 
                                                        FileOpenFlag flag)
{ 
    enum
    {
        cCtrFile_Read = 1,
        cCtrFile_ReadWrite = 3,
        cCtrFile_WriteAndCreate = 6,
    } mode;

    switch(flag)
    {
    case cFileOpenFlag_ReadOnly:
        mode = cCtrFile_Read; 
        break;
    case cFileOpenFlag_WriteOnly:
        mode = cCtrFile_WriteAndCreate; 
        break;
    case cFileOpenFlag_ReadWrite:
        mode = cCtrFile_ReadWrite; 
        break;
    case cFileOpenFlag_Create:
        mode = cCtrFile_WriteAndCreate; 
        break;
    }
    bool isExist = false;
    if(doIsExistFile_(&isExist, path) == false)
    {
        this == NULL;
    }
    else if(flag == cFileOpenFlag_Create && isExist)
    {
        this == NULL;
    }
    else
    {
        FileStreamFileHandle* p = new(getFileStreamFileHandle_(handle)) FileStreamFileHandle();
        nn_result = openFileStreamImpl_(p, getArchiveName_(), path, nn::fs::OPEN_MODE_READ);

        if(nn_result.IsFailure())
        {
            this == NULL;
        }
        else
        {
            p->mFlag = flag;
        }
    }

    return this;
}

bool CtrFileStreamFileDevice::doClose_(FileHandle* handle)
{
    bool res = true;
    FileStreamFileHandle* h = getFileStreamFileHandle_(handle);
    if(h->mFlag == cFileOpenFlag_WriteOnly && h->mFlag == cFileOpenFlag_ReadWrite)
    {
        nn_result = h->TryFlush();
        res = nn_result.IsSuccess();
    }

    h->Finalize();
    h->FileStreamFileHandle::~FileStreamFileHandle();

    return res;
}

bool CtrFileStreamFileDevice::doRead_(u32* bytesRead, FileHandle* handle, 
                u8* outBuffer, u32 bytesToRead)
{
    FileStreamFileHandle *fileHandle = getFileStreamFileHandle_(handle);

    SEAD_ASSERT_MSG(fileHandle->mFlag == cFileOpenFlag_WriteOnly 
        || fileHandle->mFlag == cFileOpenFlag_Create, 
        "file was opened as write or create only");

    s32 bytesR = 0;

    nn_result = fileHandle->TryRead(&bytesR, outBuffer, bytesToRead);

    if(bytesR < 0)
        return false;
    
    if(bytesRead != NULL)
        *bytesRead = bytesR;
    
    return nn_result.IsSuccess();
}

bool CtrFileStreamFileDevice::doWrite_(u32* bytesWritten, FileHandle* handle, const u8* inBuffer,
                  u32 bytesToWrite)
{
    FileStreamFileHandle *fileHandle = getFileStreamFileHandle_(handle);

    SEAD_ASSERT_MSG(fileHandle->mFlag == cFileOpenFlag_ReadOnly, 
        "file was opened as read only");
    s32 bytesW = 0;
    nn_result = fileHandle->TryWrite(&bytesW, inBuffer, bytesToWrite,mDoFlush);

    if(bytesW < 0)
        return false;
    
    if(bytesWritten != NULL)
        *bytesWritten = bytesW;
    
    return nn_result.IsSuccess();
}

bool CtrFileStreamFileDevice::doSeek_(FileHandle *handle, int offset,
                                       FileDevice::SeekOrigin origin)
{
    nn::fs::PositionBase positionBase;

    switch (origin)
    {
    case cSeekOrigin_Begin:
        positionBase = nn::fs::PositionBase::BASE_BEGIN;
        break;
    case cSeekOrigin_Current:
        positionBase = nn::fs::PositionBase::BASE_CURRENT;
        break;
    case cSeekOrigin_End:
        positionBase = nn::fs::PositionBase::BASE_END;
        break;
    default:
        return false;
    }

    FileStreamFileHandle *fileHandle = getFileStreamFileHandle_(handle);

    nn_result = fileHandle->FileStream::TrySeek(offset, positionBase);

    return nn_result.IsSuccess();
}

bool CtrFileStreamFileDevice::doGetCurrentSeekPos_(u32* seekPos, FileHandle *handle)
{
    FileStreamFileHandle *fileHandle = getFileStreamFileHandle_(handle);

    s64 position = 0;
    nn_result = fileHandle->FileStream::TryGetPosition(&position);

    if (position < 0)
        return false;

    *seekPos = position;

    return nn_result.IsSuccess();
}

bool CtrFileStreamFileDevice::doGetFileSize_(u32* fileSize, const SafeString& path)
{
    FileStream fs;
    nn_result = openFileStreamImpl_(&fs, getArchiveName_(), path, nn::fs::OPEN_MODE_READ);

    if(nn_result.IsFailure())
    {
        return false;
    }
    else
    {
        s64 size = 0;
        nn_result = fs.TryGetSize(&size);
        *fileSize = 0;
        return nn_result.IsSuccess();
    }
}

bool CtrFileStreamFileDevice::doGetFileSize_(u32* fileSize, FileHandle* handle)
{
    FileStreamFileHandle* h = getFileStreamFileHandle_(handle);
    s64 size = 0;
    nn_result = h->FileStream::TryGetSize(&size);
    *fileSize = 0;
    return nn_result.IsSuccess();
}

bool CtrFileStreamFileDevice::doIsExistFile_(bool* exists, const SafeString& path)
{
    nn::fs::Directory dr;
    nn_result = openDirectryImpl_(&dr, getArchiveName_(), path);

    if(nn::fs::ResultNotFound().Includes(nn_result))
    {
        *exists = false;
        return true;
    }
    else
    {
        if(nn_result.IsFailure())
        {
            nn::fs::FileStream fs;
            nn_result = openFileStreamImpl_(&fs, getArchiveName_(), path, nn::fs::OPEN_MODE_READ);

            if(nn_result.IsFailure())
            {
                *exists = false;
                return false;
            }
            else
            {
                *exists = true;
                return true;
            }
        }
        else
        {
            *exists = false;
            return true;
        }
    }
}

bool CtrFileStreamFileDevice::doIsExistDirectory_(bool* exists, const SafeString& path)
{
    nn::fs::FileStream fs;
    nn_result = openFileStreamImpl_(&fs, getArchiveName_(), path, nn::fs::OPEN_MODE_READ);

    if(nn::fs::ResultNotFound().Includes(nn_result))
    {
        *exists = false;
        return true;
    }
    else
    {
        if(nn_result.IsFailure())
        {
            nn::fs::Directory dr;
            nn_result = openDirectryImpl_(&dr, getArchiveName_(), path);
   
            if(nn_result.IsFailure())
            {
                *exists = false;
                return false;
            }
            else
            {
                *exists = true;
                return true;
            }
        }
        else
        {
            *exists = false;
            return true;
        }
    }
}

FileDevice* CtrFileStreamFileDevice::doOpenDirectory_(DirectoryHandle* handle, const SafeString& path)
{
    Directory* pDir = new(getNnFsDirectory_(handle)) Directory();
    nn_result = openDirectryImpl_(pDir, getArchiveName_(), path);

    if(nn_result.IsFailure())
    {
        handle = NULL;
    }
    return this;
}

bool CtrFileStreamFileDevice::doMakeDirectory_(const SafeString& path, u32 u_32)
{
    WFixedSafeString<256> string;

    s32 len = string.format(u"%s:/%s", getArchiveName_(), path.cstr());
    SEAD_ASSERT(len < cFileNameFormatBufSize - 1);
    nn_result = nn::fs::TryCreateDirectory((const char*)string.cstr());
    return nn_result.IsSuccess();
}

s32 CtrFileStreamFileDevice::doGetLastRawError_() const 
{ 
    return nn_result.GetValue(); 
}

bool CtrFileStreamFileDevice::doCloseDirectory_(DirectoryHandle* handle)
{
    Directory* pDir = new(getNnFsDirectory_(handle)) Directory();
    pDir->Finalize();
    pDir->~Directory();
    nn_result = nn::Result::Const<nn::Result::LEVEL_SUCCESS, nn::Result::SUMMARY_SUCCESS, nn::Result::MODULE_COMMON, 0>();
    return true;
}

bool CtrFileStreamFileDevice::doReadDirectory_(u32* entriesRead, DirectoryHandle* handle, DirectoryEntry* entries, u32 entriesToRead)
{
    nn::fs::Directory *directory = getNnFsDirectory_(handle);

    for (u32 i = 0; i < entriesToRead; i++)
    {
        int readCount = 0;
        nn::fs::DirectoryEntry rawEntry;

        nn_result = directory->TryRead(&readCount, &rawEntry, 1);

        if (nn_result.IsFailure())
        {
            if (entriesRead != nullptr)
                *entriesRead = readCount;
            return false;
        }

        if (readCount == 0)
        {
            if (entriesRead != nullptr)
                *entriesRead = i;
            return true;
        }

        WSafeString wideName((char16*)rawEntry.entryName);
        entries[i].name.convertFromWideCharString(wideName, -1);
        entries[i].is_directory = rawEntry.entrySize;
    }

    if (entriesRead != nullptr)
    {
        *entriesRead = entriesToRead;
    }
    return true;
}

void CtrFileStreamFileDevice::doResolvePath_(BufferedSafeString* out, const SafeString& path) const
{
    out->format("%s:/%s", getArchiveName_(), path.cstr());
}

nn::Result CtrFileStreamFileDevice::openFileStreamImpl_(nn::fs::FileStream* fs, SafeString const& pathInner, 
                    SafeString const& pathOutter, u32 mode)
{
    SEAD_ASSERT(fs);
    WFixedSafeString<256> sstring;

    s32 len = sstring.format(u"%s:/%s", pathInner.cstr(), pathOutter.cstr());

    SEAD_ASSERT(len < cFileNameFormatBufSize - 1);
    return fs->TryInitialize((wchar_t*)sstring.cstr(), mode);
}

nn::Result CtrFileStreamFileDevice::openDirectryImpl_(nn::fs::Directory* dr, SafeString const& pathInner, 
                    SafeString const& pathOutter)
{
    SEAD_ASSERT(dr);
    WFixedSafeString<256> sstring;
    s32 len = sstring.format(u"%s:/%s", pathInner.cstr(), pathOutter.cstr());
    SEAD_ASSERT(len < cFileNameFormatBufSize - 1);
    return dr->TryInitialize((wchar_t*)sstring.cstr());
}

}