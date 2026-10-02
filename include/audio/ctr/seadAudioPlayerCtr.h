#ifndef SEAD_AUDIO_PLAYER_CTR_H_
#define SEAD_AUDIO_PLAYER_CTR_H_

#include <audio/seadAudioPlayer.h>
#include <thread/seadCriticalSection.h>

#include <nw/snd/snd_SoundArchivePlayer.h>

namespace sead 
{

class AudioRmtSpeakerMgrCtr;
class AudioSoundDataMgrCtr;
class AudioSoundHeapCtr;
class SoundHandle;

class AudioPlayerCtr : public AudioPlayer, public nw::snd::SoundArchivePlayer
{
    SEAD_RTTI_OVERRIDE(AudioPlayerCtr, AudioPlayer)

public:
    AudioPlayerCtr();
    virtual ~AudioPlayerCtr();

    virtual void initialize();
    virtual void finalize();
    virtual void calc();
    virtual bool startSound(SoundHandle* handle, u32);
    virtual bool startSound(SoundHandle* handle, const char* name);
    virtual bool holdSound(SoundHandle* handle, u32);
    virtual bool holdSound(SoundHandle* handle, const char* name);
    virtual u32 getSoundCount() const;
    virtual const char* getSoundName(u32) const;
    virtual u32 getSoundId(const char*) const;

protected:
    virtual nw::snd::SoundStartable::StartResult detail_SetupSound(nw::snd::SoundHandle* handle, u32 soundId, bool holdFlag, const nw::snd::SoundStartable::StartInfo* startInfo);

public:
    AudioSoundDataMgrCtr* getSoundDataMgr() const
    {
        return mSoundDataMgr;
    }

    AudioSoundHeapCtr* getSoundHeap() const
    {
        return mSoundHeap;
    }

    AudioRmtSpeakerMgrCtr* getRmtSpeakerMgr() const
    {
        return mRmtSpeakerMgr;
    }

protected:
    u8* mPlayerBuffer;
    u32 mPlayerBufferSize;
    u8* mStreamBuffer;
    u32 mStreamBufferSize;
    u32 _d8;
    u8* mStreamCacheBuffer;
    u32 mStreamCacheBufferSize;
    AudioSoundDataMgrCtr* mSoundDataMgr;
    AudioSoundHeapCtr* mSoundHeap;
    AudioRmtSpeakerMgrCtr* mRmtSpeakerMgr;
    u8 _f0;
    u8 _f1;
    CriticalSection mUpdateLock;
    bool mIsValidUpdateLock;
};

} // namespace sead

#endif // SEAD_AUDIO_PLAYER_CTR_H_