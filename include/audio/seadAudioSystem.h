#ifndef SEAD_AUDIOSYSTEM_H_
#define SEAD_AUDIOSYSTEM_H_

#include <audio/seadAudioGlobal.h>
#include <prim/seadRuntimeTypeInfo.h>

namespace sead 
{
class AudioFx;
class ISoundFrameCallback;

class AudioSystem
{
    SEAD_RTTI_BASE(AudioSystem)

public:
    AudioSystem(){ }
    virtual ~AudioSystem(){ }

    virtual void initialize() = 0;
    virtual void finalize() = 0;
    virtual bool setOutputMode(AudioGlobal::OutputMode mode) = 0;
    virtual AudioGlobal::OutputMode getOutputMode() const = 0;
    virtual bool appendFxObject(AudioGlobal::AuxBus bus, AudioFx* fx) = 0;
    virtual void clearEffect(AudioGlobal::AuxBus bus, s32 eff) = 0;
    virtual bool isFinishedClearEffect(AudioGlobal::AuxBus bus) = 0;
    virtual void appendSoundFrameCallback(ISoundFrameCallback& callback) = 0;
    virtual void removeSoundFrameCallback(ISoundFrameCallback& callback) = 0;
    virtual void clearSoundFrameCallback() = 0;

protected:
    virtual void initializeDsp_() = 0;
    virtual void finalizeDsp_() = 0;
    virtual void initializeNw_() = 0;
};

} // namespace sead

#endif // SEAD_AUDIOSYSTEM_H_