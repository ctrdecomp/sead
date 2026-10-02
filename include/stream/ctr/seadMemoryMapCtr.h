#pragma once

#include "stream/seadStream.h"
#include "filedevice/ctr/seadCtrFileDevice.h"

namespace sead
{
class MemoryMapCtr
{
public:
    class Stream : public FileHandle, public CtrFileDevice
    {
    public:
        Stream():
            FileHandle(), CtrFileDevice(),
            mMapBuffer(0), mMapSize(0),
            mFileSize(0), mFilePosition(0),
            mLetter(0)
        {
        }

        ~Stream()
        {
        }

        s32 isValid() const { return mMapBuffer != 0; }
        size_t getMapSize() const { return mMapSize; }
        size_t getFileSize() const { return mFileSize; }
        s32 getPos() const { return mMapPosition + mFilePosition; }
        char getLetter() const { return mLetter; }

        void bufferAll(Heap* heap);
        void close();
        void freeBuffer();
        s32 jump(u32 pos);
        void moveToCurrentLineStart();
        void moveToNextLineStart();
        void moveToNextSectionStart();
        s32 prev();
        bool open(const SafeString& path);
        s32 next();
        u32 readNumber();

    private:
        char* mMapBuffer;
        size_t mMapSize;
        s32 mMapPosition;
        size_t mFileSize;
        s32 mFilePosition;
        char mLetter;
    };

    MemoryMapCtr():
        mMemoryStream(),
        mSectionCount(0), 
        mSectionSize(0), 
        mSectionStartPos(0)
    {
    }

    ~MemoryMapCtr()
    {
    }

    bool open(SafeString const& path);
    bool load(SafeString const& path, Heap* heap);
    void close();
    void unload();
    bool searchFunction(BufferedSafeString* out, s32* result, u32 address);

protected:
    static bool isControlChar_(char c){ return c == ' '; }
private:
    Stream mMemoryStream;
    s32 mSectionCount;
    s32 mSectionSize;
    s32 mSectionStartPos;
};
}

#define SEAD_MEMORY_MAP_CTR_H_
#include <stream/ctr/seadMemoryMapCtr.hpp>
#undef SEAD_MEMORY_MAP_CTR_H_