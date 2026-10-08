// Filename: seadGameFrameworkCtrNw4c.cpp
//
// Project: StandardEAD C++ Library for CTR
// 
// Purpose: CTR Implementation of ConsoleFramework based off the Windows version of S.E.A.D.

#include <framework/ctr/seadConsoleFrameworkCtr.h>
#include <framework/seadSingleScreenMethodTreeMgr.h>

namespace sead
{
ConsoleFrameworkCtr::ConsoleFrameworkCtr():
    Framework()
{
}

ConsoleFrameworkCtr::~ConsoleFrameworkCtr()
{
}

void ConsoleFrameworkCtr::initialize(const InitializeArg& arg)
{
    Framework::initialize(arg);
}

void ConsoleFrameworkCtr::runImpl()
{
    SingleScreenMethodTreeMgr* methodTreeMgr = DynamicCast<SingleScreenMethodTreeMgr>(getMethodTreeMgr());

    while (!mTaskMgr->getRootTask())
    {
        mTaskMgr->beforeCalc();
        mTaskMgr->afterCalc();

        Thread::sleep(TickSpan::makeFromMilliSeconds(10));
    }

    methodTreeMgr->pauseAll(false);

    while (true)
    {
        methodTreeMgr->draw();

        mTaskMgr->beforeCalc();
        methodTreeMgr->calc();
        mTaskMgr->afterCalc();

        procReset_();

        Thread::sleep(TickSpan::makeFromMilliSeconds(16));
    }
}

MethodTreeMgr* ConsoleFrameworkCtr::createMethodTreeMgr_(Heap* heap)
{
    return new(heap) SingleScreenMethodTreeMgr();
}
} // namespace sead
