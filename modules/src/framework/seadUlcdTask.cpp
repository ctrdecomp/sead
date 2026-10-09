// Filename: seadUlcdTask.cpp
//
// Project: StandardEAD C++ Library for CTR

#include "framework/seadUlcdTask.h"
#include "framework/seadUlcdMethodTreeMgr.h"

namespace sead
{
UlcdTask::UlcdTask(const TaskConstructArg& arg):
    DualScreenTask(arg),
    mRightNode(NULL)
{
    mRightNode.setPauseFlag(MethodTreeNode::cPause_None);

    mRightNode.bind(this, &UlcdTask::drawRight, "Task");
}

UlcdTask::UlcdTask(const TaskConstructArg& arg, const char* name):
    DualScreenTask(arg, name),
    mRightNode(NULL)
{
    mRightNode.setPauseFlag(MethodTreeNode::cPause_None);

    mRightNode.bind(this, &UlcdTask::drawRight, name);
}

UlcdTask::~UlcdTask()
{
}

void UlcdTask::pauseDraw(bool b)
{
    DualScreenTask::pauseDraw(b);
    if(b)
    {
        mRightNode.setPauseFlag(MethodTreeNode::cPause_Self);
    }
    else
    {
        mRightNode.setPauseFlag(MethodTreeNode::cPause_None);
    }
}

void UlcdTask::pauseDrawRec(bool b)
{
    DualScreenTask::pauseDrawRec(b);
    if(b)
    {
        mRightNode.setPauseFlag(MethodTreeNode::cPause_Both);
    }
    else
    {
        mRightNode.setPauseFlag(MethodTreeNode::cPause_None);
    }
}

void UlcdTask::attachDrawImpl()
{
    DualScreenTask::attachDrawImpl();

    ScopedLock<CriticalSection> lock(getMethodTreeMgr()->getTreeCriticalSection());

    TaskBase* p = parent() ? parent()->val() : nullptr;

    switch(getTag())
    {
    case cSystem:
        attachMethodWithCheck(14, &mRightNode);
        break;
    default:
        SEAD_ASSERT_MSG(false, "Undefined Tag(%d).", getTag());
        //! Fallthrough
    case cApp:
        if (!p)
        {
            attachMethodWithCheck(15, &mRightNode);
        }
        else
        {
            SEAD_ASSERT_MSG(p->isConnectable(this), "illigal parent ( MethodTreeMgr matching failed ).");

            MethodTreeNode* rightDraw = p->getMethodTreeNode(9);
            SEAD_ASSERT(rightDraw);
            rightDraw->pushBackChild(&mRightNode);
        }
    }
}

void UlcdTask::detachDrawImpl()
{
    DualScreenTask::detachDrawImpl();
    mRightNode.detachAll();
}

const RuntimeTypeInfo::Interface* UlcdTask::getCorrespondingMethodTreeMgrTypeInfo() const
{
    return UlcdMethodTreeMgr::getRuntimeTypeInfoStatic();
}

MethodTreeNode* UlcdTask::getMethodTreeNode(s32 methodType)
{
    switch (methodType)
    {
        case 11: 
            break;
        case 12: 
            break;
        case 13: 
            return DualScreenTask::getMethodTreeNode(6);
        case 14: 
            break;
        case 15: 
            break;
        case 16:
            return &mRightNode;
        default:
            return DualScreenTask::getMethodTreeNode(methodType);
    }
}
}
