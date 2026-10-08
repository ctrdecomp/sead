/**
* @file main.cpp
*
* @author "SA" Luigifan27
*
* @brief Primitive Example for GameFrameworkCtr for sead.
*
* @date 10/8/2026
*/

#include "framework/ctr_nw4c/seadGameFrameworkCtrNw4c.h"
#include "heap/seadExpHeap.h"
#include "RootTask.h"

const s32 cWidthTop = 256;
const s32 cWidthBtm = 416;
const s32 cHeightTop = 256;
const s32 cHeightBtm = 320;

typedef GameFrameworkCtrNw4c AppFramework;

void nnMain()
{
    {
        sead::Framework::InitializeArg arg;
        arg.heap_size = nn::os::GetDeviceMemorySize();

        AppFramework::initialize(arg);
    }

    sead::Heap* heap = sead::HeapMgr::instance()->getRootHeap(0);

    {
        /* Choose your own memory size. */
        ExpHeap* h = sead::ExpHeap::tryCreate(50 * 1024 * 1024, "FrameworkHeap", heap, Heap::cHeapDirection_Forward, true);
        sead::HeapMgr::instance()->setAllocFromNotSeadThreadHeap(h);
    }

    AppFramework* fw;
    {
        AppFramework::CreateArg arg;
        /* Standard 3DS Console */
        arg.widthTop = cWidthTop;
        arg.heightTop = cHeightTop;
        arg.widthBtm = cWidthBtm;
        arg.heightBtm = cHeightBtm;
        arg.physW_Top = 0;
        arg.physH_Top = 0;
        arg.physW_Btm = 0;
        arg.physH_Btm = 0;

        arg.wait_vblank = true;

        arg.clearColor = sead::Color4f(0.3f, 0.3f, 1.0f, 1.0f);

        arg.cmdMemSize = 0x600000;
        arg.vsync_buf = 0x4000;

        arg.memoryMgrCtr = NULL;
        arg.wait_vblank = true;

        arg.format = GL_RGB565;
        arg.screenshot_buffer = NULL;

        arg.cmdBufSize = 0x80000;
        arg.cmdRequestCount = 0x800;

        fw = new(heap) AppFramework(arg);
        fw->initializeGraphicsSystem(heap, Vector2f(cWidthTop, cWidthBtm), Vector2f(cHeightTop, cHeightBtm));
    }

    {
        sead::TaskBase::CreateArg taskArg(&sead::TTaskFactory<RootTask>);
        sead::Framework::RunArg runArg;

        fw->run(heap, taskArg, runArg);
    }

    delete fw;
}