#include <prim/seadFormatPrint.h>

#include <prim/seadBitFlag.h>
#include <prim/seadSafeString.h>
#include <prim/seadStringUtil.h>
#include <time/seadTickSpan.h>

#include <cstring>

namespace sead {

void PrintOutput::writeLineBreak()
{
    write(&SafeString::cLineBreakChar, 1);
}

PrintFormatter& PrintOutput::operator<<(PrintFormatter& f)
{
    f.setPrintOutput(this);
    return f;
}

BufferingPrintOutput::~BufferingPrintOutput()
{
    mStreamSrc.write(&SafeString::cNullChar, 1);
    mStreamSrc.flush();
}

void BufferingPrintOutput::write(const char* str, s32 len)
{
    mStreamSrc.write(str, len);
}

PrintFormatter::PrintFormatter(const char* formatStr, PrintOutput* output)
    : mFormatStr(formatStr)
    , mPrintOutput(output)
    , mPos(0)
    , mFormatStrLength(0)
    , mIsFormatRestAll(false)
{
    if (mFormatStr)
        mFormatStrLength = static_cast<s32>(std::strlen(mFormatStr));
}

void PrintFormatter::flush()
{
    if (!mFormatStr)
        return;

    mIsFormatRestAll = false;

    char option[cOptionBufSize];

    while (mPos < mFormatStrLength)
    {
        bool end = proceedToFormatMark_(option);
        SEAD_ASSERT_MSG(!end, "too much format specifier: %s", mFormatStr);
    }
}

void PrintFormatter::flushWithLineBreak()
{
    flush();

    if (mFormatStr)
        mPrintOutput->writeLineBreak();
}

PrintFormatter& PrintFormatter::operator<<(const char* str)
{
    if (!mFormatStr)
    {
        mFormatStr = str;
        mFormatStrLength = static_cast<s32>(std::strlen(str));
        return *this;
    }

    char option[cOptionBufSize];

    bool end = proceedToFormatMark_(option);
    if (end)
        PrintFormatter::outSimpleObject_(NULL, option, mPrintOutput, str);

    return *this;
}

bool PrintFormatter::proceedToFormatMark_(char* option)
{
    option[0] = '\0';

    if (!mFormatStr)
    {
        SEAD_ASSERT_MSG(false, "format string is not set. please check argument order.");
        return false;
    }

    if (mIsFormatRestAll)
        return true;

    if (mPos >= mFormatStrLength)
        return false;

    const char* str = mFormatStr + mPos;
    s32 idx = 0;

    while (str[idx] != '\0')
    {
        if (str[idx] == '%')
        {
            if (str[idx + 1] != '%')
            {
                if (idx > 0)
                {
                    mPrintOutput->write(str, idx);

                    str += idx;
                    mPos += idx;
                    idx = 0;
                }

                s32 idxNext = idx + 1;
                if (str[idxNext] == '@')
                {
                    idx += 2;
                }
                else if (str[idxNext] == '<')
                {
                    idx += 2;
                    mIsFormatRestAll = true;
                }
                else
                {
                    option[0] = '%';

                    while (PrintFormatter::isQualification_(str[idxNext]))
                    {
                        if (idxNext >= cOptionLengthMax)
                        {
                            SEAD_ASSERT_MSG(false, "option string too long: %s", mFormatStr);
                            option[0] = '\0';
                            mPos = mFormatStrLength;
                            return true;
                        }

                        option[idxNext] = str[idxNext];
                        idxNext++;
                    }

                    if (str[idxNext] == '\0')
                    {
                        SEAD_ASSERT_MSG(false, "illegal format: %s", mFormatStr);
                        option[0] = '\0';
                        mPos = mFormatStrLength;
                        return true;
                    }

                    option[idxNext] = str[idxNext];
                    idx = idxNext + 1;
                    option[idx] = '\0';
                }

                mPos += idx;
                return true;
            }

            mPrintOutput->write(str, idx + 1);

            str += idx + 2;
            mPos += idx + 2;
            idx = 0;
        }
        else
        {
            idx++;
        }
    }

    if (idx > 0)
    {
        mPrintOutput->write(str, idx);
        mPos += idx;
    }

    return false;
}

void PrintFormatter::outputPtr_(const char* option, PrintOutput* output, uintptr_t ptr)
{
    FixedSafeString<cOptionBufSize> str;

    if (!option)
        option = "0x%p";

    s32 strLen = str.format(option, ptr);

    output->write(str.cstr(), strLen);
}

BufferingPrintFormatter::BufferingPrintFormatter()
    : PrintFormatter(nullptr, &mOutput)
    , mOutput(mBuffer, cBufferSize)
{
}

BufferingPrintFormatter::BufferingPrintFormatter(const char* formatStr)
    : PrintFormatter(formatStr, &mOutput)
    , mOutput(mBuffer, cBufferSize)
{
}

/*template <typename T>
void PrintFormatter::out(const T& obj, const char* option, PrintOutput* output)
{
    PrintFormatter::outSimpleObject_("%u", option, output, obj);
}*/

template <>
void PrintFormatter::out<bool>(const bool& obj, const char* option, PrintOutput* output)
{
    if (!option)
    {
        if (obj)
            output->write("true", 4);
        else
            output->write("false", 5);
    }
    else
    {
        PrintFormatter::out<s32>(obj, option, output);
    }
}
/*
template <>
void PrintFormatter::out<char>(const char* obj, const char* option, PrintOutput* output)
{
}

template <>
void PrintFormatter::out<char16>(const char16* obj, const char* option, PrintOutput* output)
{
}
*/
template <>
void PrintFormatter::out<TickSpan>(const TickSpan& obj, const char*, PrintOutput* output)
{
    PrintFormatter::out(obj.toMicroSeconds(), "%lld", output);
    output->write("us", 2);
}

template <>
void PrintFormatter::OutImpl<u8, BitFlag>::out(const BitFlag<u8>& obj, const char* option, PrintOutput* output)
{
    PrintFormatter::outSimpleObject_("0x%x", option, output, obj.getDirect());
}

template <>
void PrintFormatter::OutImpl<u16, BitFlag>::out(const BitFlag<u16>& obj, const char* option, PrintOutput* output)
{
    PrintFormatter::outSimpleObject_("0x%x", option, output, obj.getDirect());
}

template <>
void PrintFormatter::OutImpl<u32, BitFlag>::out(const BitFlag<u32>& obj, const char* option, PrintOutput* output)
{
    PrintFormatter::outSimpleObject_("0x%x", option, output, obj.getDirect());
}

template <>
void PrintFormatter::OutImpl<u64, BitFlag>::out(const BitFlag<u64>& obj, const char* option, PrintOutput* output)
{
    PrintFormatter::outSimpleObject_("0x%llx", option, output, obj.getDirect());
}

template <>
void PrintFormatter::OutImpl<char, SafeStringBase>::out(const SafeStringBase<char>& obj, const char* option, PrintOutput* output)
{
    output->write(obj.cstr(), obj.calcLength());
}

} // namespace sead
