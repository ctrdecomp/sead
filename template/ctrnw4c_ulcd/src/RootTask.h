/**
* @file RootTask.h
*
* @author "SA" Luigifan27
*
* @date 10/7/2026
*/

#ifndef ROOT_TASK_H_
#define ROOT_TASK_H_

#include "framework/seadTaskBase.h"

using namespace sead;

class RootTask : public TaskBase
{
    SEAD_RTTI_OVERRIDE(RootTask, TaskBase)
public:
    explicit RootTask(const TaskConstructArg& arg);

    void calc();
};

#endif // ROOT_TASK_H_