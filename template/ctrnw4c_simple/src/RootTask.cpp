/**
* @file RootTask.cpp
*
* @author "SA" Luigifan27
*
* @brief Primitive example for RootTask.
*
* @date 10/8/2026
*/

#include "RootTask.h"

RootTask::RootTask(const TaskConstructArg& arg):
    TaskBase(TaskConstructArg(), "RootTask")
{
}

void RootTask::calc()
{
    SEAD_PRINT("RootTask::calc() non implement\n");
}
