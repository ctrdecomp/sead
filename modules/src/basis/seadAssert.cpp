#include "devenv/seadAssertConfig.h"

namespace sead
{
namespace system
{
void HaltWithDetailNoFormat(const char* file, int lineNo, const char* fmt)
{
    HaltWithDetail(file, lineNo, "%s", fmt);
}
}
}
