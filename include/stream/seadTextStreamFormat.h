#pragma once

#include <stream/seadStreamFormat.h>

namespace sead 
{
class TextStreamFormat : public StreamFormat
{
public:
    static const u32 BUFFER_SIZE;
    static const u32 SEPARATOR_SIZE;

public:
    TextStreamFormat();

    virtual u8 readU8(StreamSrc* src, Endian::Types endian);
    virtual u16 readU16(StreamSrc* src, Endian::Types endian);
    virtual u32 readU32(StreamSrc* src, Endian::Types endian);
    virtual u64 readU64(StreamSrc* src, Endian::Types endian);
    virtual s8 readS8(StreamSrc* src, Endian::Types endian);
    virtual s16 readS16(StreamSrc* src, Endian::Types endian);
    virtual s32 readS32(StreamSrc* src, Endian::Types endian);
    virtual s64 readS64(StreamSrc* src, Endian::Types endian);
    virtual f32 readF32(StreamSrc* src, Endian::Types endian);
    virtual void readBit(StreamSrc* src, void* data, u32 bitnum);
    virtual void readString(StreamSrc* src, BufferedSafeString* dst, u32 size);
    virtual u32 readMemBlock(StreamSrc* src, void* dst, u32 size);
    virtual void writeU8(StreamSrc* src, Endian::Types endian, u8 value);
    virtual void writeU16(StreamSrc* src, Endian::Types endian, u16 value);
    virtual void writeU32(StreamSrc* src, Endian::Types endian, u32 value);
    virtual void writeU64(StreamSrc* src, Endian::Types endian, u64 value);
    virtual void writeS8(StreamSrc* src, Endian::Types endian, s8 value);
    virtual void writeS16(StreamSrc* src, Endian::Types endian, s16 value);
    virtual void writeS32(StreamSrc* src, Endian::Types endian, s32 value);
    virtual void writeS64(StreamSrc* src, Endian::Types endian, s64 value);
    virtual void writeF32(StreamSrc* src, Endian::Types endian, f32 value);
    virtual void writeBit(StreamSrc* src, const void* data, u32 bitnum);
    virtual void writeString(StreamSrc* src, const SafeString& str, u32 size);
    virtual void writeMemBlock(StreamSrc* src, const void* data, u32 size);
    virtual void writeDecorationText(StreamSrc* src, const SafeString& str);
    virtual void writeNullChar(StreamSrc* src);
    virtual void skip(StreamSrc* src, u32 size);
    virtual void flush(StreamSrc* src){ }
    virtual void rewind(StreamSrc* src);

    FixedSafeString<128>& getSeparator() { return mSeparator; };
    const FixedSafeString<128>& getSeparator() const{ return mSeparator; }

private:
    void getNextData_(StreamSrc* src);

private:
    FixedSafeString<128> mSeparator;
};

} // namespace sead
