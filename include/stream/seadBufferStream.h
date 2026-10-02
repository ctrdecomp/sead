#pragma once

#include "stream/seadStream.h"
#include "stream/seadStreamSrc.h"

namespace sead
{
class BufferReadStreamSrc : public StreamSrc
{
public:
    BufferReadStreamSrc(StreamSrc* src, void* buffer, u32 buffer_size);
    virtual ~BufferReadStreamSrc();

    virtual u32 read(void* data, u32 size);
    virtual u32 write(const void* data, u32 size);
    virtual u32 skip(s32 offset);
    virtual void rewind();
    virtual bool isEOF();

private:
    StreamSrc* mSrc;
    void* mBuffer;
    u32 mBufferSize;
    u32 mCurrentSize = 0;
    u32 mCurrentPos = 0;
};

class BufferReadStream : public ReadStream
{
public:
    BufferReadStream(ReadStream* stream, const void* buffer, u32 buffer_size);
    virtual ~BufferReadStream();

private:
    BufferReadStreamSrc mSrc;
};

class BufferWriteStreamSrc : public StreamSrc
{
public:
    BufferWriteStreamSrc(StreamSrc* src, void* buffer, u32 buffer_size);
    virtual ~BufferWriteStreamSrc();

    virtual u32 read(void* data, u32 size);
    virtual u32 write(const void* data, u32 size);
    virtual u32 skip(s32 offset);
    virtual void rewind();
    virtual bool isEOF() { return mSrc->isEOF(); }
    virtual bool flush();

private:
    StreamSrc* mSrc;
    void* mBuffer;
    u32 mBufferSize = 0;
    u32 mCurrentPos = 0;
};

class BufferWriteStream : public WriteStream
{
public:
    BufferWriteStream(WriteStream* stream, void* buffer, u32 buffer_size);
    virtual ~BufferWriteStream();

private:
    BufferWriteStreamSrc mSrc;
};

class BufferMultiByteTextWriteStreamSrc : public BufferWriteStreamSrc
{
public:
    BufferMultiByteTextWriteStreamSrc(StreamSrc* src, void* buffer, u32 buffer_size);
    virtual ~BufferMultiByteTextWriteStreamSrc(){ }
    virtual u32 write(const void* data, u32 size);
};

class BufferMultiByteTextWriteStream : public WriteStream
{
public:
    BufferMultiByteTextWriteStream(WriteStream* stream, void* buffer, u32 buffer_size);
    virtual ~BufferMultiByteTextWriteStream();

private:
    BufferMultiByteTextWriteStreamSrc mSrc;
};

class BufferMultiByteNullTerminatedTextWriteStreamSrc : public BufferMultiByteTextWriteStreamSrc
{
public:
    virtual ~BufferMultiByteNullTerminatedTextWriteStreamSrc(){}
    virtual bool flush();
};
}  // namespace sead
