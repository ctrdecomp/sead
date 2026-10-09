#pragma once

#include <prim/seadStringUtil.h>

#ifndef SEAD_MEMORY_MAP_CTR_H_
#include <stream/ctr/seadMemoryMapCtr.h>
#endif // SEAD_MEMORY_MAP_CTR_H_

namespace sead
{

/* MemoryMapCtr::Stream */

void MemoryMapCtr::Stream::bufferAll(Heap* heap)
{
    if (!HandleBase::isOpened())
        return;

    if (mMapBuffer != nullptr && *mMapBuffer != mHandleBuffer[0])
        return;

    mMapBuffer = new char[mFileSize];

    FileHandle::seek(0, cSeekOrigin_Begin);

    mMapSize = FileHandle::read(reinterpret_cast<u8*>(mMapBuffer), mFileSize);

    if (mMapSize < mFileSize)
        mFileSize = mMapSize;

    mMapPosition = 0;
    mFilePosition = 0;
    mLetter = 0;
}

void MemoryMapCtr::Stream::close()
{
    if(HandleBase::isOpened())
    {
        FileHandle::close();
    }

    if(*mMapBuffer == mHandleBuffer[0])
    {
        mMapBuffer = 0;
        mMapSize = 0;
        mMapPosition = 0;
        mFileSize = 0;
        mFilePosition = 0;
        mLetter = 0;
    }
}

void MemoryMapCtr::Stream::freeBuffer()
{
    if (mMapBuffer == nullptr)
        return;

    if (mMapBuffer != reinterpret_cast<char*>(&mHandleBuffer[0]))
        delete(mMapBuffer);

    if (HandleBase::isOpened())
        mMapBuffer = reinterpret_cast<char*>(&mHandleBuffer[0]);
    else
        mMapBuffer = nullptr;

    mFileSize = 0;
    mMapSize = 0;
    mMapPosition = 0;
    mFilePosition = 0;
    mLetter = 0;
}

int MemoryMapCtr::Stream::jump(u32 pos)
{
    if (mFileSize <= pos)
    {
        mLetter = 0;
        return mLetter;
    }

    if (mMapPosition <= pos && pos <= mMapPosition + mMapSize)
    {

    }
    else
    {
        if (pos >= 128)
            mMapPosition = pos - 128;
        else
            mMapPosition = 0;

        FileHandle::seek(mMapPosition, cSeekOrigin_Begin);

        mMapSize = FileHandle::read(reinterpret_cast<u8*>(&mHandleBuffer), 256);
    }

    mFilePosition = pos - mMapPosition;

    if (mFilePosition >= 0 && mFilePosition < mMapSize)
        mLetter = mMapBuffer[mFilePosition];
    else
        mLetter = 0;

    return mLetter;
}

void MemoryMapCtr::Stream::moveToCurrentLineStart()
{
    prev();
    while (getLetter() && getLetter() != '\n')
        prev();
    next();
}

void MemoryMapCtr::Stream::moveToNextLineStart()
{
    while (getLetter())
    {
        if (getLetter() == '\n')
        {
            next();
            return;
        }
        next();
    }
}

void MemoryMapCtr::Stream::moveToNextSectionStart()
{
    while (getLetter())
    {
        if (isControlChar_(getLetter()))
        {
            next();
            return;
        }

        next();
    }
}

s32 MemoryMapCtr::Stream::prev()
{
    --mFilePosition;

    if (mFilePosition < 0 && mMapPosition != 0)
    {
        if (mMapPosition >= 256)
            mMapPosition -= 256;
        else
            mMapPosition = 0;

        FileHandle::seek(mMapPosition, cSeekOrigin_Begin);

        mMapSize = FileHandle::read(reinterpret_cast<u8*>(&mHandleBuffer), 256);

        mFilePosition = mMapSize - 1;
    }

    if (mFilePosition >= 0 && mFilePosition < mMapSize)
    {
        mLetter = mHandleBuffer[mFilePosition];
    }
    else
    {
        mLetter = 0;
    }

    return mLetter;
}

bool MemoryMapCtr::Stream::open(const SafeString& path)
{
    if (mMapBuffer != nullptr)
        return false;

    if (!tryOpen(this, path, FileDevice::cFileOpenFlag_ReadOnly))
        return false;

    mMapBuffer = reinterpret_cast<char*>(&mHandleBuffer[0]);

    mMapSize = 0;
    mMapPosition = 0;
    mFileSize = FileHandle::getFileSize();
    mFilePosition = 0;
    mLetter = 0;

    return true;
}

s32 MemoryMapCtr::Stream::next()
{
    ++mFilePosition;

    if (mFilePosition >= mMapSize)
    {
        if (mMapPosition + mMapSize < mFileSize)
        {
            mMapPosition += mMapSize;

            FileHandle::seek(mMapPosition, cSeekOrigin_Begin);

            mMapSize = FileHandle::read(reinterpret_cast<u8*>(&mHandleBuffer), 0x100);

            mFilePosition = 0;
        }
    }

    if (mFilePosition >= 0 && mFilePosition < mMapSize)
        mLetter = mMapBuffer[mFilePosition];
    else
        mLetter = 0;

    return mLetter;
}

u32 MemoryMapCtr::Stream::readNumber()
{
    char buffer[8];
    u32 result = 0;

    for (u32 i = 0; i < 8; ++i)
    {
        char c = getLetter();

        if (MemoryMapCtr::isControlChar_(c))
        {
            buffer[i] = '\0';
            break;
        }

        buffer[i] = c;
        next();
    }

    SafeString str(buffer);
    StringUtil::tryParseU32(&result, str, StringUtil::Base16);

    return result;
}

/* MemoryMapCtr */

bool MemoryMapCtr::open(const SafeString& path)
{
    if (mMemoryStream.isValid())
    {
        mMemoryStream.freeBuffer();
        mMemoryStream.close();
    }

    mSectionCount = 0;
    mSectionSize = 0;
    mSectionStartPos = 0;

    if (!mMemoryStream.open(path))
        return false;

    mMemoryStream.jump(0);
    mSectionCount = mMemoryStream.readNumber();

    mMemoryStream.jump(mMemoryStream.getFileSize() - 1);

    while (mMemoryStream.getLetter() != 0 && isControlChar_(mMemoryStream.getLetter()))
    {
        mMemoryStream.prev();
    }

    mMemoryStream.moveToCurrentLineStart();

    mSectionStartPos = mMemoryStream.getPos();
    mSectionSize = mMemoryStream.readNumber();

    if (mSectionStartPos != 0 && mSectionCount != 0 && mSectionSize > mSectionCount)
    {
        return true;
    }

    mMemoryStream.close();
    return false;
}

bool MemoryMapCtr::load(SafeString const& path, Heap* heap)
{
    bool doOpen = open(path);
    if(doOpen)
    {
        mMemoryStream.bufferAll(heap);
    }
    mMemoryStream.close();
    return doOpen;
}

void MemoryMapCtr::close()
{
    mMemoryStream.close();
    if(!mMemoryStream.isValid())
    {
        mSectionCount = 0;
        mSectionSize = 0;
        mSectionStartPos = 0;
    }
}

void MemoryMapCtr::unload()
{
    mMemoryStream.freeBuffer();
    if(!mMemoryStream.isValid())
    {
        mSectionCount = 0;
        mSectionSize = 0;
        mSectionStartPos = 0;
    }
}

bool MemoryMapCtr::searchFunction(BufferedSafeString* out, s32* result, u32 address)
{
    s32 searchPos = 0;
    s32 sectionStart = mSectionStartPos;

    while (searchPos < sectionStart)
    {
        s32 midpoint = (searchPos + sectionStart) >> 1;

        mMemoryStream.jump(midpoint);
        mMemoryStream.moveToCurrentLineStart();

        s32 linePos = mMemoryStream.getPos();
        u32 lineAddress = mMemoryStream.readNumber();

        if (address < lineAddress)
        {
            sectionStart = linePos;
            continue;
        }

        s32 currentPos = mMemoryStream.getPos();

        if (currentPos < midpoint)
            mMemoryStream.jump(midpoint);

        mMemoryStream.moveToNextLineStart();

        s32 nextLinePos = mMemoryStream.getPos();
        u32 nextAddress = mMemoryStream.readNumber();

        if (address >= nextAddress)
        {
            searchPos = nextLinePos;
            continue;
        }

        mMemoryStream.jump(currentPos);
        mMemoryStream.moveToNextSectionStart();

        out->clear();

        s32 length = 0;
        while (length < out->getBufferSize() - 1)
        {
            char c = mMemoryStream.getLetter();

            if (isControlChar_(c))
                break;

            out->append(c);
            mMemoryStream.next();
            ++length;
        }

        if (result)
            *result = address - lineAddress;

        return true;
    }

    return false;
}
}
