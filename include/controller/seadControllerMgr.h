#pragma once

#include "container/seadOffsetList.h"
#include "container/seadPtrArray.h"
#include "controller/seadController.h"
#include "framework/seadCalculateTask.h"
#include "framework/seadTaskMgr.h"
#include "framework/seadTaskParameter.h"
#include "heap/seadDisposer.h"
#include "prim/seadRuntimeTypeInfo.h"

namespace sead
{
class ControlDevice;
class Controller;
class ControllerAddon;

class ControllerMgr : public CalculateTask
{
    SEAD_TASK_SINGLETON_DISPOSER(ControllerMgr);

private:
    class ConstructArg : public TaskConstructArg
    {
    public:
        ConstructArg() : TaskConstructArg(), mHeapArray() { heap_array = &mHeapArray; }

    private:
        HeapArray mHeapArray;
    };

public:
    struct Parameter : public TaskParameter
    {
        SEAD_RTTI_OVERRIDE(Parameter, TaskParameter)

    public:
        s32 controllerMax;
        IDelegate1<ControllerMgr*>* proc;
    };

public:
    ControllerMgr();
    explicit ControllerMgr(const TaskConstructArg& arg);
    virtual ~ControllerMgr(){ }

    virtual void prepare();
    virtual void calc();

    void initialize(s32 controller_max, Heap* heap);
    void finalize();

    void initializeDefault(Heap* heap);
    void finalizeDefault();

    // TODO: Add/remove devices & controllers

    Controller* getControllerByOrder(ControllerDefine::ControllerId id, s32 index) const;
    ControlDevice* getControlDevice(ControllerDefine::DeviceId id) const;
    ControllerAddon* getControllerAddon(s32 index, ControllerDefine::AddonId id) const;
    ControllerAddon* getControllerAddonByOrder(s32 controller_index,
                                               ControllerDefine::AddonId addon_id,
                                               int addon_index) const;

    template <typename T>
    T getControllerByOrderAs(s32 index) const;
    template <typename T>
    T getControlDeviceAs() const;
    template <typename T>
    T getControllerAddonAs(s32 index) const;

    Framework* getFramework() const;

    Controller* getController(int port) { return mControllers[port]; }

private:
    OffsetList<ControlDevice> mDevices;
    PtrArray<Controller> mControllers;
};

template <typename T>
T ControllerMgr::getControllerByOrderAs(s32 index) const
{
    for (PtrArray<Controller>::iterator it = mControllers.begin(); it != mControllers.end(); ++it)
    {
        T controller = DynamicCast<typename RemovePointer<T>::Type>(&(*it));
        if (controller)
        {
            if (index == 0)
                return controller;

            index--;
        }
    }

    return NULL;
}

template <typename T>
T ControllerMgr::getControlDeviceAs() const
{
    for (OffsetList<ControlDevice>::iterator it = mDevices.begin(); it != mDevices.end(); ++it)
    {
        T device = DynamicCast<typename RemovePointer<T>::Type>(&(*it));
        if (device)
            return device;
    }

    return NULL;
}

template <typename T>
T ControllerMgr::getControllerAddonAs(s32 index) const
{
    Controller* controller = mControllers.at(index);
    if (controller)
        return controller->getAddonAs<T>();

    return NULL;
}

}  // namespace sead
