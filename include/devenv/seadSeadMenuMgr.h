#pragma once

#include <framework/seadTask.h>
#include <framework/seadTaskMgr.h>

namespace sead 
{
class SeadMenuMgr : public Task
{
    SEAD_TASK_SINGLETON_DISPOSER(SeadMenuMgr);

public:
    explicit SeadMenuMgr(const TaskConstructArg& arg);
};

} // namespace sead
