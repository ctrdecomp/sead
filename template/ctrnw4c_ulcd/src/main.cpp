/**
* @file main.cpp
*
* @author "SA" Luigifan27
*
* @brief Primitive Example for Ulcd GameFrameworks.
*
* @date 10/7/2026
*/

#include "framework/ctr_nw4c/seadUlcdDoubleCmdGameFrameworkCtrNw4c.h"
#include "framework/ctr_nw4c/seadUlcdGameFrameworkCtrNw4c.h"
#include "heap/seadExpHeap.h"
#include "RootTask.h"

const s32 cWidthTop = 256;
const s32 cWidthBtm = 416;
const s32 cHeightTop = 256;
const s32 cHeightBtm = 320;

#ifdef SEAD_TEST_USE_DUAL_SCREEN
typedef UlcdDoubleCmdGameFrameworkCtrNw4c AppFramework;
#else
typedef UlcdGameFrameworkCtrNw4c AppFramework;
#endif

void nnMain()
{
    //* Initialize SEAD application framework.
    {
        sead::Framework::InitializeArg arg;
        arg.heap_size = nn::os::GetDeviceMemorySize();

        AppFramework::initialize(arg);
    }

    sead::Heap* heap = sead::HeapMgr::instance()->getRootHeap(0);

    //* Creating app heap.
    {
        ExpHeap* h = sead::ExpHeap::tryCreate(nn::os::GetDeviceMemorySize(), "RootHeap", heap, Heap::cHeapDirection_Forward, true);
        ScopedCurrentHeapSetter heap(h);
    }

    //* Creating the graphics system used for the device, by setting up its args for initializeGraphicsSystem()
    AppFramework* fw;
    {
        AppFramework::CreateArg arg;

        arg.widthTop  = cWidthTop;
        arg.heightTop = cHeightTop;
        arg.widthBtm  = cWidthBtm;
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

    //* Creating our task. In this case, RootTask.
    {
        sead::TaskBase::CreateArg taskArg(&sead::TTaskFactory<RootTask>);
        sead::Framework::RunArg runArg;

        fw->run(heap, taskArg, runArg);
    }

    delete fw;
}