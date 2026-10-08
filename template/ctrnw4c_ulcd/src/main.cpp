/**
* @file main.cpp
*
* @author "SA" Luigifan27
*
* @brief Primitive Example for GameFrameworkCtr for sead.
*
* @date 10/7/2026
*/

#include "framework/ctr_nw4c/seadUlcdDoubleCmdGameFrameworkCtrNw4c.h"
#include "heap/seadExpHeap.h"
#include "RootTask.h"

const s32 cWidth = 400;
const s32 cHeight = 240;

void nnMain()
{
    {
        sead::Framework::InitializeArg arg;
        arg.heap_size = 24 * 1024 * 1024; // 24 MiB

        UlcdDoubleCmdGameFrameworkCtrNw4c::initialize(arg);
    }

    sead::Heap* heap = sead::HeapMgr::instance()->getRootHeap(0);

    {
        ExpHeap* h = sead::ExpHeap::tryCreate(50 * 1024 * 1024, "NonSeadThreadHeap", heap, Heap::cHeapDirection_Forward, true);
        sead::HeapMgr::instance()->setAllocFromNotSeadThreadHeap(h);
    }

    UlcdDoubleCmdGameFrameworkCtrNw4c* appFw;
    {
        UlcdDoubleCmdGameFrameworkCtrNw4c::CreateArg arg;
        /* Standard 3DS Console */
        arg.widthTop   = 400;
        arg.heightTop  = 240;
        arg.widthBtm   = 320;
        arg.heightBtm  = 240;

        arg.physW_Top  = 76.92f;
        arg.physH_Top  = 46.15f;
        arg.physW_Btm  = 61.44f;
        arg.physH_Btm  = 46.08f;

        arg.clearColor = sead::Color4f(0.0f, 0.0f, 0.3f, 1.0f);

        appFw = new(heap) UlcdDoubleCmdGameFrameworkCtrNw4c(arg);
        appFw->initializeGraphicsSystem(heap, Vector2f(cWidth, cHeight), Vector2f(cWidth, cHeight));
    }

    {
        sead::TaskBase::CreateArg taskArg(&sead::TTaskFactory<RootTask>);
        sead::Framework::RunArg runArg;

        appFw->run(heap, taskArg, runArg);
    }
}