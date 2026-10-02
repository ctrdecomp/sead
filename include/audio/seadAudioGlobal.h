#ifndef SEAD_AUDIOGLOBAL_H_
#define SEAD_AUDIOGLOBAL_H_

#include <basis/seadTypes.h>
#include <nn/snd.h>

namespace sead 
{
class AudioGlobal // namespace or class, God knows
{
public:
    enum AuxBus
    {
        cAuxBusNull = nn::snd::CTR::AUX_BUS_NULL,
        cAuxBusA    = nn::snd::CTR::AUX_BUS_A,
        cAuxBusB    = nn::snd::CTR::AUX_BUS_B,
        cAuxBusNum  = nn::snd::CTR::AUX_BUS_NUM
    };

    enum OutputMode
    {
        cMono     = nn::snd::CTR::OUTPUT_MODE_MONO,
        cStereo   = nn::snd::CTR::OUTPUT_MODE_STEREO,
        cSurround = nn::snd::CTR::OUTPUT_MODE_3DSURROUND
    };
};

} // namespace sead

#endif // SEAD_AUDIOGLOBAL_H_