#ifndef SEAD_AUDIO_SYSTEM_CTR_H_
#define SEAD_AUDIO_SYSTEM_CTR_H_

#include "audio/seadAudioSystem.h"

namespace sead 
{
class AudioFx;
class ISoundFrameCallback;

class AudioSystemCtr
{
    SEAD_RTTI_OVERRIDE(AudioSystemCtr, AudioSystem)

public:
    AudioSystemCtr();
    virtual ~AudioSystemCtr();

    virtual void initialize();
    virtual void finalize();
    virtual bool setOutputMode(AudioGlobal::OutputMode mode);
    virtual AudioGlobal::OutputMode getOutputMode() const;
    virtual bool appendFxObject(AudioGlobal::AuxBus bus, AudioFx* fx);
    virtual void clearEffect(AudioGlobal::AuxBus bus, s32 eff);
    virtual bool isFinishedClearEffect(AudioGlobal::AuxBus bus);
    virtual void appendSoundFrameCallback(ISoundFrameCallback& callback);
    virtual void removeSoundFrameCallback(ISoundFrameCallback& callback);
    virtual void clearSoundFrameCallback();

    bool isAuxBusA(AudioGlobal::AuxBus bus){ return bus == AudioGlobal::cAuxBusA; }
    bool isAuxBusB(AudioGlobal::AuxBus bus){ return bus == AudioGlobal::cAuxBusB; }

protected:
    virtual void initializeDsp_();
    virtual void finalizeDsp_();
    virtual void initializeNw_();
};

} // namespace sead

#endif