#pragma once

#include <basis/seadTypes.h>
#include <filedevice/seadFileDevice.h>
#include <stream/seadBufferStream.h>
#include <stream/seadStream.h>
#include <stream/seadStreamSrc.h>

namespace sead
{
class FileDeviceStreamSrc : public StreamSrc
{
public:
    FileDeviceStreamSrc();
    FileDeviceStreamSrc(FileHandle* fileHandle);
    virtual ~FileDeviceStreamSrc();

    virtual u32 read(void* buffer, u32 size);
    virtual u32 write(const void* buffer, u32 size);
    virtual u32 skip(s32 offset);
    virtual void rewind();
    virtual bool isEOF();

    void setFileHandle(sead::FileHandle* fileHandle);

    FileHandle* getFileHandle() const { return mFileHandle; }

private:
    FileHandle* mFileHandle;
    u32 mStartingPos;
    FileHandle mHandleTemp;
    bool mIsHandleOpen;
    u32 mFileSize;
};

class FileDeviceWriteStream : public WriteStream
{
public:
    FileDeviceWriteStream(Stream::Modes mode);
    FileDeviceWriteStream(StreamFormat* format);
    FileDeviceWriteStream(FileHandle* fileHandle, Stream::Modes mode);
    FileDeviceWriteStream(FileHandle* fileHandle, StreamFormat* format);
    virtual ~FileDeviceWriteStream();

    void setFileHandle(sead::FileHandle* fileHandle);

    FileDeviceStreamSrc* getSrc() { return &src; }

private:
    FileDeviceStreamSrc src;
};

class FileDeviceReadStream : public ReadStream
{
public:
    FileDeviceReadStream(Stream::Modes mode);
    FileDeviceReadStream(StreamFormat* format);
    FileDeviceReadStream(FileHandle* fileHandle, Stream::Modes mode);
    FileDeviceReadStream(FileHandle* fileHandle, StreamFormat* format);
    virtual ~FileDeviceReadStream();

    void setFileHandle(sead::FileHandle* fileHandle);

    FileDeviceStreamSrc* getSrc() { return &src; }

private:
    FileDeviceStreamSrc src;
};

class BufferFileDeviceWriteStream : public FileDeviceWriteStream
{
public:
    BufferFileDeviceWriteStream(Stream::Modes mode);
    BufferFileDeviceWriteStream(StreamFormat* format);
    BufferFileDeviceWriteStream(FileHandle* fileHandle, Stream::Modes mode);
    BufferFileDeviceWriteStream(FileHandle* fileHandle, StreamFormat* format);

private:
    BufferWriteStreamSrc mBufferSrc;
    u8 mBuffer[0x120];  // NOTE: 0x100 + 0x20 bytes for alignment
};

class BufferFileDeviceReadStream : public FileDeviceReadStream
{
public:
    BufferFileDeviceReadStream(Stream::Modes mode);
    BufferFileDeviceReadStream(StreamFormat* format);
    BufferFileDeviceReadStream(FileHandle* fileHandle, Stream::Modes mode);
    BufferFileDeviceReadStream(FileHandle* fileHandle, StreamFormat* format);

private:
    BufferReadStreamSrc mBufferSrc;
    u8 mBuffer[0x120];  // NOTE: 0x100 + 0x20 bytes for alignment
};

}  // namespace sead
