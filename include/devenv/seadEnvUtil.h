#pragma once

#include "prim/seadEnum.h"
#include "prim/seadSafeString.h"

namespace sead
{
    enum RegionLanguageID
    {
        JPja,
        USen,
        USes,
        USfr,
        USpt,
        EUen,
        EUes,
        EUfr,
        EUde,
        EUit,
        EUpt,
        EUnl,
        EUru,
        KRko,
        CNzh,
        TWzh
    };

    enum RegionID
    {
        JP,
        US,
        EU,
        KR,
        CN,
        TW
    };

class EnvUtil
{
public:
    static const SafeString& getRomType();
    static RegionLanguageID getRegionLanguage();
    static RegionID getRegion();
    static s32 getEnvironmentVariable(BufferedSafeString* out, const SafeString& variable);
    static s32 resolveEnvronmentVariable(BufferedSafeString* out, const SafeString& str);
};
}  // namespace sead
