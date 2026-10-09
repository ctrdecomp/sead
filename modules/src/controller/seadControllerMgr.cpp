#include "controller/seadControllerMgr.h"
#include "basis/seadNew.h"
#include "controller/ctr/seadCtrHidDeviceCtr.h"
#include "controller/ctr/seadCtrController.h"
#include "controller/ctr/seadCtrAccelerometerAddon.h"
#include "controller/seadControlDevice.h"
#include "framework/seadTaskID.h"
#include "prim/seadDelegate.h"
#include "thread/seadDelegateThread.h"

namespace sead
{
SEAD_TASK_SINGLETON_DISPOSER_IMPL(ControllerMgr);

// NON_MATCHING: storing too much 00s into stack (for ConstructArg)
ControllerMgr::ControllerMgr():
    CalculateTask(ConstructArg(), "sead::ControllerMgr"),
    mDevices(), 
    mControllers()
{
    mDevices.initOffset(offsetof(ControlDevice, mListNode));
}

ControllerMgr::ControllerMgr(const TaskConstructArg& arg): 
    CalculateTask(arg, "sead::ControllerMgr"),
    mDevices(), 
    mControllers()
{
    mDevices.initOffset(offsetof(ControlDevice, mListNode));
}

void ControllerMgr::prepare()
{
    Parameter* parameter = DynamicCast<Parameter>(mParameter);
    if (parameter)
    {
        initialize(parameter->controllerMax, nullptr);
        if (parameter->proc)
            parameter->proc->invoke(this);
    }
    else
    {
        initializeDefault(nullptr);
    }
}

void ControllerMgr::initialize(s32 controller_max, Heap* heap)
{
    mControllers.allocBuffer(controller_max, heap);
}

void ControllerMgr::finalize()
{
    mControllers.freeBuffer();
}

void ControllerMgr::initializeDefault(Heap* heap)
{
    initialize(1, heap);

    {
        CtrHidDevice* device = new (heap) CtrHidDevice(this);

        mDevices.pushBack(device);
    }

    {
        CtrController* controller = new(heap) CtrController(this);
        
        mControllers.pushBack(controller);
        controller->mAddons.pushBack(new(heap) CtrAccelerometerAddon(controller));
    }
}

void ControllerMgr::finalizeDefault()
{
    finalize();
}

void ControllerMgr::calc()
{
    for (OffsetList<sead::ControlDevice>::iterator it = mDevices.begin(); it != mDevices.end(); ++it)
        it->calc();

    for (PtrArray<sead::Controller>::iterator it = mControllers.begin(); it != mControllers.end(); ++it)
        it->calc();
}

Controller* ControllerMgr::getControllerByOrder(ControllerDefine::ControllerId id, s32 index) const
{
    for (PtrArray<sead::Controller>::iterator it = mControllers.begin(); it != mControllers.end(); ++it)
    {
        Controller& controller = *it;
        if (controller.mId == id)
        {
            if (index == 0)
                return &controller;

            index--;
        }
    }

    return NULL;
}

ControlDevice* ControllerMgr::getControlDevice(ControllerDefine::DeviceId id) const
{
    for (OffsetList<ControlDevice>::iterator it = mDevices.begin(); it != mDevices.end(); ++it)
    {
        ControlDevice* device = &*it;
        if (device->mId == id)
            return device;
    }

    return NULL;
}

ControllerAddon* ControllerMgr::getControllerAddon(s32 index, ControllerDefine::AddonId id) const
{
    Controller* controller = mControllers.at(index);
    if (controller)
        return controller->getAddon(id);

    return nullptr;
}

ControllerAddon* ControllerMgr::getControllerAddonByOrder(s32 controller_index,
                                                          ControllerDefine::AddonId id,
                                                          s32 addon_index) const
{
    Controller* controller = mControllers.at(controller_index);
    if (controller)
        return controller->getAddonByOrder(id, addon_index);

    return nullptr;
}

Framework* ControllerMgr::getFramework() const
{
    if (mTaskMgr)
        return mTaskMgr->getFramework();
    return nullptr;
}

}  // namespace sead

