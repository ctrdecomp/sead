#ifndef SEAD_AUDIO_PLAYER_H_
#define SEAD_AUDIO_PLAYER_H_

#include <prim/seadRuntimeTypeInfo.h>
#include <basis/seadTypes.h>

namespace sead 
{
class SoundHandle;

class AudioPlayer
{
    SEAD_RTTI_BASE(AudioPlayer)

public:
    AudioPlayer()
    {
    }

    virtual ~AudioPlayer()
    {
    }

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
};

} // namespace sead

#endif // SEAD_AUDIO_PLAYER_H_