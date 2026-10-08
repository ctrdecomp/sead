#include "stream/seadTextStreamFormat.h"
#include "codec/seadBase64.h"
#include "thread/seadMutex.h"
#include "math/seadMathCalcCommon.h"
#include "stream/seadStreamSrc.h"
#include "prim/seadSafeString.h"
#include "prim/seadStringUtil.h"
#include "prim/seadScopedLock.h"

namespace sead
{
namespace
{
    FixedSafeString<1024> sBuffer;
    Mutex sMutex;

    bool isSJISCode_(int c)
    {
        if (c >= 0x81 && c <= 0x9F)
            return true;

        if (c >= 0xE0 && c <= 0xEF)
            return true;

        return false;
    }
}

TextStreamFormat::TextStreamFormat():
    mSeparator("")
{

}

u8 TextStreamFormat::readU8(StreamSrc* src, Endian::Types endian)
{
    ScopedLock<Mutex> locker(&sMutex);
    u8 rawValue = 0;
    getNextData_(src);
    SEAD_ASSERT_MSG(!StringUtil::tryParseU8(&rawValue, SafeString(sBuffer.cstr()), StringUtil::BaseAuto), "Text field is not number. ( text = \"%s\" )\n", sBuffer.cstr());
    return rawValue;
}

u16 TextStreamFormat::readU16(StreamSrc* src, Endian::Types endian)
{
    ScopedLock<Mutex> locker(&sMutex);
    u16 rawValue = 0;
    getNextData_(src);
    SEAD_ASSERT_MSG(!StringUtil::tryParseU16(&rawValue, SafeString(sBuffer.cstr()), StringUtil::BaseAuto), "Text field is not number. ( text = \"%s\" )\n", sBuffer.cstr());
    return rawValue;
}

u32 TextStreamFormat::readU32(StreamSrc* src, Endian::Types endian)
{
    ScopedLock<Mutex> locker(&sMutex);
    u32 rawValue = 0;
    getNextData_(src);
    SEAD_ASSERT_MSG(!StringUtil::tryParseU32(&rawValue, SafeString(sBuffer.cstr()), StringUtil::BaseAuto), "Text field is not number. ( text = \"%s\" )\n", sBuffer.cstr());
    return rawValue;
}

u64 TextStreamFormat::readU64(StreamSrc* src, Endian::Types endian)
{
    ScopedLock<Mutex> locker(&sMutex);
    u64 rawValue = 0;
    getNextData_(src);
    SEAD_ASSERT_MSG(!StringUtil::tryParseU64(&rawValue, SafeString(sBuffer.cstr()), StringUtil::BaseAuto), "Text field is not number. ( text = \"%s\" )\n", sBuffer.cstr());
    return rawValue;
}

s8 TextStreamFormat::readS8(StreamSrc* src, Endian::Types endian)
{
    ScopedLock<Mutex> locker(&sMutex);
    s8 rawValue = 0;
    getNextData_(src);
    SEAD_ASSERT_MSG(!StringUtil::tryParseS8(&rawValue, SafeString(sBuffer.cstr()), StringUtil::BaseAuto), "Text field is not number. ( text = \"%s\" )\n", sBuffer.cstr());
    return rawValue;
}

s16 TextStreamFormat::readS16(StreamSrc* src, Endian::Types endian)
{
    ScopedLock<Mutex> locker(&sMutex);
    s16 rawValue = 0;
    getNextData_(src);
    SEAD_ASSERT_MSG(!StringUtil::tryParseS16(&rawValue, SafeString(sBuffer.cstr()), StringUtil::BaseAuto), "Text field is not number. ( text = \"%s\" )\n", sBuffer.cstr());
    return rawValue;
}

s32 TextStreamFormat::readS32(StreamSrc* src, Endian::Types endian)
{
    ScopedLock<Mutex> locker(&sMutex);
    s32 rawValue = 0;
    getNextData_(src);
    SEAD_ASSERT_MSG(!StringUtil::tryParseS32(&rawValue, SafeString(sBuffer.cstr()), StringUtil::BaseAuto), "Text field is not number. ( text = \"%s\" )\n", sBuffer.cstr());
    return rawValue;
}

s64 TextStreamFormat::readS64(StreamSrc* src, Endian::Types endian)
{
    ScopedLock<Mutex> locker(&sMutex);
    s64 rawValue = 0;
    getNextData_(src);
    SEAD_ASSERT_MSG(!StringUtil::tryParseS64(&rawValue, SafeString(sBuffer.cstr()), StringUtil::BaseAuto), "Text field is not number. ( text = \"%s\" )\n", sBuffer.cstr());
    return rawValue;
}

f32 TextStreamFormat::readF32(StreamSrc* src, Endian::Types endian)
{
    ScopedLock<Mutex> locker(&sMutex);
    f32 rawValue = 0.0f;
    getNextData_(src);

    if (sBuffer.calcLength() != 0)
    {
        const char* str = sBuffer.cstr();

        SEAD_ASSERT_MSG(sscanf(str, "%f", &rawValue), "Text field is not number. ( text = \"%s\" )\n", str);
    }

    SEAD_ASSERT_MSG(false, "Text field is not number. ( text = \"%s\" )\n", sBuffer.cstr());
    return rawValue;
}

void TextStreamFormat::readBit(StreamSrc* src, void* dst, u32 size)
{
    ScopedLock<Mutex> lock(&sMutex);

    getNextData_(src);

    SafeString str(sBuffer);

    if (str.comparen("0", 2) != 0)
        str = str.getPart(2);

    u8* data = static_cast<u8*>(dst);

    u32 bitCount = 0;
    u32 value = 0;

    for (SafeString::iterator itr = str.begin(); itr != str.end() && bitCount < size; ++itr)
    {
        value = (value << 1);

        if (*itr == '1')
            value |= 1;

        ++bitCount;

        if ((bitCount & 7) == 0)
        {
            data[(bitCount >> 3) - 1] = value;
            value = 0;
        }
    }

    if (bitCount & 7)
    {
        u8& byte = data[bitCount >> 3];
        byte &= 0xff << (bitCount & 7);
        byte |= value;
    }
}

// NOTE: If size > str->getBufferSize(), it wraps around and starts reading to the start again.
// if size > str->getBufferSize()*2, the second iteration continues writing out-of-bounds.
void TextStreamFormat::readString(StreamSrc* src, BufferedSafeString* str, u32 size)
{
    ScopedLock<Mutex> lock(&sMutex);
    getNextData_(src);
    str->copy(sBuffer, size);
}

u32 TextStreamFormat::readMemBlock(StreamSrc* src, void* buffer, u32 size)
{
    ScopedLock<Mutex> lock(&sMutex);

    getNextData_(src);

    size_t dSize = 0;

    bool ret = Base64::decode(buffer, size, sBuffer.cstr(), sBuffer.calcLength(), &dSize);
    SEAD_ASSERT(ret);
    return dSize;
}

void TextStreamFormat::writeU8(StreamSrc* src, Endian::Types endian, u8 value)
{
    FixedSafeString<32> buffer;

    buffer.format("%u", value);

    u32 wb = src->write(buffer.cstr(), buffer.calcLength());
    u32 write_size = src->write(buffer.cstr(), 1);

    SEAD_ASSERT(wb == write_size + 1);
}

void TextStreamFormat::writeU16(StreamSrc* src, Endian::Types endian, u16 value)
{
    FixedSafeString<32> buffer;

    buffer.format("%u", value);

    u32 wb = src->write(buffer.cstr(), buffer.calcLength());
    u32 write_size = src->write(buffer.cstr(), 1);

    SEAD_ASSERT(wb == write_size + 1);
}

void TextStreamFormat::writeU32(StreamSrc* src, Endian::Types endian, u32 value)
{
    FixedSafeString<32> buffer;

    buffer.format("%u", value);

    u32 wb = src->write(buffer.cstr(), buffer.calcLength());
    u32 write_size = src->write(buffer.cstr(), 1);

    SEAD_ASSERT(wb == write_size + 1);
}

void TextStreamFormat::writeU64(StreamSrc* src, Endian::Types endian, u64 value)
{
    FixedSafeString<32> buffer;

    buffer.format("%llu", value);

    u32 wb = src->write(buffer.cstr(), buffer.calcLength());
    u32 write_size = src->write(buffer.cstr(), 1);

    SEAD_ASSERT(wb == write_size + 1);
}

void TextStreamFormat::writeS8(StreamSrc* src, Endian::Types endian, s8 value)
{
    FixedSafeString<32> buffer;

    buffer.format("%d", value);

    u32 wb = src->write(buffer.cstr(), buffer.calcLength());
    u32 write_size = src->write(buffer.cstr(), 1);

    SEAD_ASSERT(wb == write_size + 1);
}

void TextStreamFormat::writeS16(StreamSrc* src, Endian::Types endian, s16 value)
{
    FixedSafeString<32> buffer;

    buffer.format("%d", value);

    u32 wb = src->write(buffer.cstr(), buffer.calcLength());
    u32 write_size = src->write(buffer.cstr(), 1);

    SEAD_ASSERT(wb == write_size + 1);
}

void TextStreamFormat::writeS32(StreamSrc* src, Endian::Types endian, s32 value)
{
    FixedSafeString<32> buffer;

    buffer.format("%d", value);

    u32 wb = src->write(buffer.cstr(), buffer.calcLength());
    u32 write_size = src->write(buffer.cstr(), 1);

    SEAD_ASSERT(wb == write_size + 1);
}

void TextStreamFormat::writeS64(StreamSrc* src, Endian::Types endian, s64 value)
{
    FixedSafeString<32> buffer;

    buffer.format("%lld", value);

    u32 wb = src->write(buffer.cstr(), buffer.calcLength());
    u32 write_size = src->write(buffer.cstr(), 1);

    SEAD_ASSERT(wb == write_size + 1);
}

void TextStreamFormat::writeF32(StreamSrc* src, Endian::Types endian, f32 value)
{
    FixedSafeString<32> buffer;

    buffer.format("%f", value);

    u32 wb = src->write(buffer.cstr(), buffer.calcLength());
    u32 write_size = src->write(buffer.cstr(), 1);

    SEAD_ASSERT(wb == write_size + 1);
}

// NOTE: Writes extra bits in last byte into stream normally
void TextStreamFormat::writeBit(StreamSrc* src, const void* data, u32 bitnum)
{
    ScopedLock<Mutex> lock(&sMutex);

    sBuffer.copy("0b");

    u8* bits = static_cast<u8*>(const_cast<void*>(data));

    u32 wb = (bitnum + 7) >> 3;

    for (u32 i = 0; i < wb; ++i)
    {
        u32 bitCount = 8;

        if (i == wb - 1)
            bitCount = bitnum - (i << 3);

        for (int bit = bitCount - 1; bit >= 0; --bit)
        {
            if (bits[i] & (1 << bit))
                sBuffer.append('1');
            else
                sBuffer.append('0');
        }
    }

    int writeSize = 0;

    writeSize += src->write(sBuffer.cstr(), bitnum + 2);
    writeSize += src->write(mSeparator.cstr(), 1);

    SEAD_ASSERT(wb == 2 + bitnum + 1);
}

void TextStreamFormat::writeString(StreamSrc* src, const SafeString& str, u32 max_len)
{
    u32 length = str.calcLength();
    if (max_len > length)
        max_len = length;

    char quote = '"';
    char slash = '\\';

    int wb = 0;
    int write_count = 0;

    wb += src->write(&quote, 1);
    ++write_count;

    for (u32 i = 0; i < max_len; ++i)
    {
        char c = str[i];

        if (c == '"')
        {
            wb += src->write(&slash, 1);
            ++write_count;
        }
        else if (i != 0)
        {
            char prev = str[i - 1];

            if (!isSJISCode_(prev) && prev == '"')
            {
                wb += src->write(&slash, 1);
                ++write_count;
            }
        }

        wb += src->write(str.cstr() + i, 1);
        ++write_count;
    }

    wb += src->write(&quote, 1);
    ++write_count;

    SEAD_ASSERT(wb == write_count);
}

void TextStreamFormat::writeMemBlock(StreamSrc* src, const void* data, u32 size)
{
    ScopedLock<Mutex> lock(&sMutex);

    sBuffer.clear();

    u32 base64Size = (size / 3) * 4;
    if (size % 3 != 0)
        base64Size += 4;

    SEAD_ASSERT_MSG(sBuffer.getBufferSize() >= base64Size + 1, "Can't encode data to Base64");

    Base64::encodeWithNullTerminate((char*)sBuffer.cstr(), data, size, 0);

    s32 write_size = sBuffer.calcLength();

    s32 wb = src->write("\"", 1);
    wb = src->write(sBuffer.cstr(), write_size);
    wb = src->write("\"", 1);
    wb = src->write(mSeparator.cstr(), 1);

    SEAD_ASSERT(wb == write_size + 3);
}

void TextStreamFormat::writeDecorationText(StreamSrc* src, const SafeString& str)
{
    s32 write_size = str.calcLength(); 
    s32 wb = src->write(str.cstr(), write_size); 
    SEAD_ASSERT(wb == write_size);
}

void TextStreamFormat::writeNullChar(StreamSrc* src)
{
    SafeString* str = NULL;
    s32 wb = src->write(str, 1); 
    SEAD_ASSERT(wb == 1);
}

void TextStreamFormat::skip(StreamSrc* src, u32 offset)
{
    SEAD_UNUSED(offset);

    ScopedLock<Mutex> lock(&sMutex);
    getNextData_(src);
}

void TextStreamFormat::rewind(StreamSrc* src)
{
    src->rewind();
}

void TextStreamFormat::getNextData_(StreamSrc* src)
{
    sBuffer.clear();

    int index = 0;
    int comment = 0;
    bool quoted = false;
    bool sjis = false;

    for(;;)
    {
        char c;

        if (src->read(&c, 1) == 0)
            return;

        if (comment != 0)
        {
            if (isSJISCode_(static_cast<unsigned char>(c)))
            {
                if (src->read(&c, 1) == 0)
                    return;

                sBuffer.append(c);
                ++index;
                continue;
            }

            if (c == comment)
            {
                if (comment == '/')
                {
                    comment = 0;
                }
                else
                {
                    comment = '/';
                    continue;
                }
            }

            continue;
        }

        if (!quoted)
        {
            if (c == '"')
            {
                quoted = true;
                continue;
            }

            if (sBuffer.isEmpty())
            {
                if (c == '"')
                {
                    quoted = true;
                    continue;
                }
            }

            if (mSeparator.include(c) || c == '\0')
            {
                if (!sBuffer.isEmpty())
                    return;

                continue;
            }

            sBuffer.append(c);
            ++index;

            if (isSJISCode_(static_cast<unsigned char>(c)))
            {
                if (src->read(&c, 1) == 0)
                    return;

                sBuffer.append(c);
                ++index;
                sjis = true;
            }
            else
            {
                sjis = false;
            }

            continue;
        }

        if (c == '"')
        {
            if (sjis)
            {
                quoted = false;
                return;
            }

            if (index != 0 && sBuffer[index - 1] == '\\')
            {
                SafeString quote("\"");
                sBuffer.copyAt(index - 1, quote, 1);
            }

            continue;
        }

        sBuffer.append(c);
        ++index;

        if (isSJISCode_(static_cast<unsigned char>(c)))
        {
            if (src->read(&c, 1) == 0)
                return;

            sBuffer.append(c);
            ++index;
            sjis = true;
        }
        else
        {
            sjis = false;
        }
    }
}

}  // namespace sead
