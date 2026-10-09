#ifndef SEAD_TASK_ID_H_
#define SEAD_TASK_ID_H_

#include <basis/seadTypes.h>
#include <framework/seadHeapPolicies.h>

namespace sead
{
class HeapArray;
class TaskBase;
class TaskMgr;
class TaskParameter;

struct TaskConstructArg
{
    TaskConstructArg()
        : heap_array(NULL)
        , mgr(NULL)
        , param(NULL)
    {
    }

    TaskConstructArg(HeapArray* heapArray_, TaskMgr* mgr_, TaskParameter* param_)
        : heap_array(heapArray_)
        , mgr(mgr_)
        , param(param_)
    {
    }

    HeapArray* heap_array;
    TaskMgr* mgr;
    TaskParameter* param;
};

class TaskUserID
{
public:
    TaskUserID():
        mID(-1)
    {
    }

    s32 mID;
};

typedef TaskBase* (*TaskFactory)(const TaskConstructArg&);

template <typename T>
TaskBase* TTaskFactory(const TaskConstructArg& arg)
{
    return new (arg.heap_array->getPrimaryHeap()) T(arg);
}

class TaskClassID
{
public:
    enum Type
    {
        cInvalid = 0,
        cInt = 1,
        cFactory = 2,
        cString = 3
    };

    typedef TaskBase* (*IntTaskCreator)(s32, const TaskConstructArg&);
    typedef TaskBase* (*StringTaskCreator)(const char*, const TaskConstructArg&);

    TaskBase* create(const TaskConstructArg& arg) const;

    TaskClassID()
        : mType(cInvalid)
    {
        mID.mInt = 0;
    }

    TaskClassID(s32 i)
        : mType(cInt)
    {
        mID.mInt = i;
    }

    TaskClassID(TaskFactory f)
        : mType(cFactory)
    {
        mID.mFactory = f;
    }

    TaskClassID(const char* s)
        : mType(cString)
    {
        mID.mString = s;
    }

    friend bool operator==(const TaskClassID& a, const TaskClassID& b)
    {
        if (a.mType != b.mType)
            return false;

        switch (a.mType)
        {
            case cInt:
                return a.mID.mInt == b.mID.mInt;

            case cFactory:
                return a.mID.mFactory == b.mID.mFactory;

            case cString:
                return SafeString(a.mID.mString) == SafeString(b.mID.mString);

            default:
                SEAD_ASSERT_MSG(false, "UNKNOWN TYPE %d\n", a.mType);
        }

        return false;
    }

private:
    static IntTaskCreator sIntTaskCreator;
    static StringTaskCreator sStringTaskCreator;

public:
    Type mType;

    union
    {
        s32 mInt;
        TaskFactory mFactory;
        const char* mString;
    } mID;
};

}  // namespace sead

#endif  // SEAD_TASK_ID_H_

