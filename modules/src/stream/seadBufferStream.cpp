#include "stream/seadBufferStream.h"

#include "math/seadMathCalcCommon.h"

namespace sead 
{
BufferReadStreamSrc::BufferReadStreamSrc(StreamSrc* src, void* buffer, u32 buffer_size)
    : mSrc(src), mBuffer(buffer), mBufferSize(buffer_size), mCurrentSize(0), mCurrentPos(0)
{
}

BufferReadStreamSrc::~BufferReadStreamSrc()
{
}

// NOTE: cannot take negative `offset`, but expects `mSrc->skip(X)` to work with negatives
u32 BufferReadStreamSrc::read(void* data, u32 size)
{
    u32 totalBytesRead = 0;
    while (true)
    {
        if (mCurrentPos < mCurrentSize)
        {
            u32 readSize = sead::Mathu::clampMax(size - totalBytesRead, mCurrentSize - mCurrentPos);

            memcpy((u8*)data + totalBytesRead, (u8*)mBuffer + mCurrentPos, readSize);
            totalBytesRead += readSize;
            mCurrentPos += readSize;
        }

        if (size <= totalBytesRead)
            break;

        mCurrentSize = mSrc->read(mBuffer, mBufferSize);
        mCurrentPos = 0;

        if (mCurrentSize == 0)
            break;
    }
    return totalBytesRead;
}

u32 BufferReadStreamSrc::write(const void* data, u32 size)
{
    return 0;
}

u32 BufferReadStreamSrc::skip(s32 offset)
{
    s32 remainingBytes = mCurrentSize - mCurrentPos;

    if (remainingBytes >= offset)
    {
        mCurrentPos += offset;
        return offset;
    }

    mCurrentSize = 0;
    mCurrentPos = 0;
    return mSrc->skip(offset - remainingBytes) + remainingBytes;
}

void BufferReadStreamSrc::rewind()
{
    mSrc->rewind();
    mCurrentSize = 0;
    mCurrentPos = 0;
}

bool BufferReadStreamSrc::isEOF()
{
    return mSrc->isEOF() && mCurrentPos >= mCurrentSize;
}

BufferReadStream::BufferReadStream(ReadStream* stream, const void* buffer, u32 buffer_size)
    : mSrc(stream->getSrc(), const_cast<void*>(buffer), buffer_size)
{
    setSrc(&mSrc);
    setUserFormat(stream->getUserFormat());
    setBinaryEndian(stream->getBinaryEndian());
}

BufferReadStream::~BufferReadStream()
{
    setSrc(nullptr);
}

BufferWriteStreamSrc::BufferWriteStreamSrc(StreamSrc* src, void* buffer, u32 buffer_size)
    : mSrc(src), mBuffer(buffer), mBufferSize(buffer_size), mCurrentPos(0)
{
}

BufferWriteStreamSrc::~BufferWriteStreamSrc()
{
};

u32 BufferWriteStreamSrc::read(void* data, u32 size)
{
    return 0;
}

u32 BufferWriteStreamSrc::write(const void* data, u32 size)
{
    u32 totalBytesWritten = 0;
    do
    {
        if (mCurrentPos >= mBufferSize)
            continue;

        u32 writeSize = sead::Mathu::min(mBufferSize - mCurrentPos, size - totalBytesWritten);

        memcpy((u8*)mBuffer + mCurrentPos, (u8*)data + totalBytesWritten, writeSize);
        totalBytesWritten += writeSize;
        mCurrentPos += writeSize;
    } while (totalBytesWritten < size && flush());

    return totalBytesWritten;
}

u32 BufferWriteStreamSrc::skip(s32 offset)
{
    return 0;
}

void BufferWriteStreamSrc::rewind()
{
    flush();
    mSrc->rewind();
}

bool BufferWriteStreamSrc::flush()
{
    if (mCurrentPos == 0)
        return true;

    bool success = mSrc->write(mBuffer, mCurrentPos) >= mCurrentPos;
    mCurrentPos = 0;
    return success;
}

BufferWriteStream::BufferWriteStream(WriteStream* stream, void* buffer, u32 buffer_size)
    : mSrc(stream->getSrc(), buffer, buffer_size)
{
    setSrc(&mSrc);
    setUserFormat(stream->getUserFormat());
    setBinaryEndian(stream->getBinaryEndian());
}

BufferWriteStream::~BufferWriteStream()
{
    flush();
    setSrc(nullptr);
}

BufferMultiByteTextWriteStreamSrc::BufferMultiByteTextWriteStreamSrc(StreamSrc* src, void* start, u32 size)
    : BufferWriteStreamSrc(src, start, size)
{
    SEAD_ASSERT_MSG(size >= 4, "size[%u] must be larger or equal than 4", size);
}

u32 BufferMultiByteTextWriteStreamSrc::write(const void* src, u32 size)
{
    u32 writeSize = 0;
    const u8* data = static_cast<const u8*>(src);

    do
    {
        if (mCurrentPos < mBufferSize)
        {
            u32 remainBufSize = mBufferSize - mCurrentPos;
            u32 writeStep = size - writeSize;

            if (writeStep > remainBufSize)
            {
                writeStep = remainBufSize;

                u32 characterOffset = 0;

                u8 lastByte = data[writeSize + remainBufSize - 1];
                if ((lastByte & 0x80) != 0)
                {
                    if ((lastByte & 0xC0) == 0x80)
                    {
                        for (s32 i = 2; i <= Mathi::min(remainBufSize, 4); i++)
                        {
                            lastByte = data[writeSize + remainBufSize - i];
                            if ((lastByte & 0xC0) != 0x80)
                            {
                                s32 multiByteLen = 0;
                                if ((lastByte & 0xE0) == 0xC0)
                                    multiByteLen = 2;
                                else if ((lastByte & 0xF0) == 0xE0)
                                    multiByteLen = 3;
                                else if ((lastByte & 0xF8) == 0xF0)
                                    multiByteLen = 4;

                                if (multiByteLen > i)
                                    characterOffset = i;

                                break;
                            }
                        }
                    }
                    else
                    {
                        characterOffset = 1;
                    }
                }

                if (characterOffset > 0)
                {
                    writeStep = remainBufSize - characterOffset;
                    reinterpret_cast<u8*>(mBuffer)[mBufferSize - characterOffset] = 0;
                }
            }

            MemUtil::copy((u8*)mBuffer + mCurrentPos, data + writeSize, writeStep);

            writeSize += writeStep;
            mCurrentPos += writeStep;
        }
    } while (writeSize < size && flush());

    return writeSize;
}

bool BufferMultiByteNullTerminatedTextWriteStreamSrc::flush()
{
    SEAD_ASSERT(mCurrentPos <= mBufferSize);

    reinterpret_cast<u8*>(mBuffer)[mCurrentPos] = '\0';
    return BufferWriteStreamSrc::flush();
}

}  // namespace sead
