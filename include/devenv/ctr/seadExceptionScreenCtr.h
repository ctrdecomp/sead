#pragma once

#include "devenv/ctr/seadDirectPrintCtr.h"
#include "prim/seadDelegate.h"
#include <nn/os.h>
#include <nn/os/ARM/os_ExceptionHandler.h>

namespace sead
{
class ExceptionScreenCtr
{
public:
    ExceptionScreenCtr();

    virtual ~ExceptionScreenCtr();

    void disableAssertCallback();
    static void exceptionHandler(ARM::ExceptionInfo * info, ARM::ExceptionContext* context);
    static s32 putBackTraceString(BufferedSafeString* string, size_t size);

    static ExceptionScreenCtr* sExceptionScreenCtr;
    static const char* sMapFilePath;

    void enableAssertCallback(){ AssertConfig::registerFinalCallback(&mEvent); }
    void disableAssertCallback(){ AssertConfig::registerFinalCallback(&mEvent); }
    DirectPrintCtr* getDirectPrint(){ return &mPrinter; }

protected:
    virtual void onHalt_(const char* msgName);
    virtual int putHaltMessage_(Vector2<int> const& pos, char const* msg);
    virtual void clearMessage_(Vector2<int> const& bufSize, const char* msg);

private:
    DirectPrintCtr mPrinter;
    Delegate1<ExceptionScreenCtr, const char*> mEvent;
    PtrUtil mExceptionStack;
};
}