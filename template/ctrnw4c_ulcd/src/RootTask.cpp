/**
* @file RootTask.cpp
*
* @author "SA" Luigifan27
*
* @brief Primitive Example RootTask for games using sead.
*
* @date 10/7/2026
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