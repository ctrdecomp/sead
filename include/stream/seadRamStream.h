#pragma once

#include "stream/seadStream.h"
#include "stream/seadStreamSrc.h"

namespace sead
{
class RamStreamSrc : public StreamSrc
{
public:
    RamStreamSrc(void* buffer, u32 bufferSize);
    virtual ~RamStreamSrc();

    virtual u32 read(void* data, u32 size);
    virtual u32 write(const void* data, u32 size);
    virtual u32 skip(s32 offset);
    virtual void rewind() { mCurrentPos = 0; }
    virtual bool isEOF() { return mCurrentPos >= mBufferSize; }

private:
    u8* mBuffer;
    u32 mBufferSize;
    u32 mCurrentPos;
};

class RamReadStream : public ReadStream
{
public:
    RamReadStream(const void* buffer, u32 buffer_size, Stream::Modes mode);
    RamReadStream(const void* buffer, u32 buffer_size, StreamFormat* format);
    virtual ~RamReadStream();

private:
    RamStreamSrc mSrc;
};

class RamWriteStream : public WriteStream
{
public:
    RamWriteStream(void* buffer, u32 buffer_size, Stream::Modes mode);
    RamWriteStream(void* buffer, u32 buffer_size, StreamFormat* format);
    virtual ~RamWriteStream();

private:
    RamStreamSrc mSrc;
};

}  // namespace sead