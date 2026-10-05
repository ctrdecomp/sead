#include "framework/seadCalculateTask.h"

namespace sead
{
CalculateTask::CalculateTask(const TaskConstructArg& arg): 
    TaskBase(arg),
    mCalcNode(nullptr)
{
    mCalcNode.setPauseFlag(MethodTreeNode::cPause_Both);

    mCalcNode.bind(this, &CalculateTask::calc, "CalculateTask");
}

CalculateTask::CalculateTask(const TaskConstructArg& arg, const char* name): 
    TaskBase(arg, name),
    mCalcNode(nullptr)
{
    mCalcNode.setPauseFlag(MethodTreeNode::cPause_Both);

    mCalcNode.bind(this, &CalculateTask::calc, name);
}

CalculateTask::~CalculateTask()
{
}

void CalculateTask::calc() 
{
}

void CalculateTask::attachCalcImpl()
{
    ScopedLock<CriticalSection> lock(getMethodTreeMgr()->getTreeCriticalSection());

    TaskBase* p = parent() ? parent()->val() : nullptr;

    switch (getTag())
    {
        case cSystem:
            attachMethodWithCheck(0, &mCalcNode);
            break;

        default:
            SEAD_ASSERT_MSG(false, "Undefined Tag(%d).", getTag());
            //! Fallthrough

        case cApp:
            if (!p)
            {
                attachMethodWithCheck(1, &mCalcNode);
            }
            else
            {
                SEAD_ASSERT_MSG(p->isConnectable(this), "illigal parent ( MethodTreeMgr matching failed ).");

                MethodTreeNode* calcNode = p->getMethodTreeNode(1);
                SEAD_ASSERT(calcNode);

                calcNode->pushBackChild(&mCalcNode);
            }
    }
}

void CalculateTask::attachDrawImpl()
{
}

void CalculateTask::detachCalcImpl()
{
    mCalcNode.detachAll();
}

void CalculateTask::detachDrawImpl()
{
}

void CalculateTask::pauseCalc(bool b)
{
    if (b)
        mCalcNode.setPauseFlag(MethodTreeNode::cPause_Self);
    else
        mCalcNode.setPauseFlag(MethodTreeNode::cPause_None);
}

void CalculateTask::pauseDraw(bool)
{
}

void CalculateTask::pauseCalcRec(bool b)
{
    if (b)
        mCalcNode.setPauseFlag(MethodTreeNode::cPause_Both);
    else
        mCalcNode.setPauseFlag(MethodTreeNode::cPause_None);
}

void CalculateTask::pauseDrawRec(bool)
{
}

void CalculateTask::pauseCalcChild(bool b)
{
    if (b)
        mCalcNode.setPauseFlag(MethodTreeNode::cPause_Child);
    else
        mCalcNode.setPauseFlag(MethodTreeNode::cPause_None);
}

void CalculateTask::pauseDrawChild(bool)
{
}

const RuntimeTypeInfo::Interface* CalculateTask::getCorrespondingMethodTreeMgrTypeInfo() const
{
    return MethodTreeMgr::getRuntimeTypeInfoStatic();
}

MethodTreeNode* CalculateTask::getMethodTreeNode(s32 methodType)
{
    switch (methodType)
    {
        case 0:
            return &mCalcNode;

        case 1:
            return &mCalcNode;

        case 2:
            return nullptr;

        case 3:
            return nullptr;

        case 4:
            return nullptr;

        default:
            SEAD_ASSERT_MSG(false, "Undefined methodType(%d).", methodType);
            return nullptr;
    }
}

}  // namespace sead
