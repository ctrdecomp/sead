#include <framework/seadTask.h>

#include <framework/seadMethodTreeMgr.h>
#include <prim/seadScopedLock.h>

namespace sead {

Task::Task(const TaskConstructArg& arg): 
    TaskBase(arg), 
    mCalcNode(nullptr), 
    mDrawNode(nullptr)
{
    mCalcNode.setPauseFlag(MethodTreeNode::cPause_Both);
    mDrawNode.setPauseFlag(MethodTreeNode::cPause_Both);

    mCalcNode.bind(this, &Task::calc, "Task");
    mDrawNode.bind(this, &Task::draw, "Task");
}

Task::Task(const TaskConstructArg& arg, const char* name): 
    TaskBase(arg, name), 
    mCalcNode(nullptr), 
    mDrawNode(nullptr)
{
    mCalcNode.setPauseFlag(MethodTreeNode::cPause_Both);
    mDrawNode.setPauseFlag(MethodTreeNode::cPause_Both);

    mCalcNode.bind(this, &Task::calc, name);
    mDrawNode.bind(this, &Task::draw, name);
}

Task::~Task()
{
}

void Task::calc()
{
}

void Task::draw()
{
}

void Task::attachCalcImpl()
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

void Task::attachDrawImpl()
{
    ScopedLock<CriticalSection> lock(getMethodTreeMgr()->getTreeCriticalSection());

    TaskBase* p = parent() ? parent()->val() : nullptr;

    switch (getTag())
    {
        case cSystem:
            attachMethodWithCheck(2, &mDrawNode);
            break;

        default:
            SEAD_ASSERT_MSG(false, "Undefined Tag(%d).", getTag());
            //! Fallthrough

        case cApp:
            if (!p)
            {
                attachMethodWithCheck(3, &mDrawNode);
            }
            else
            {
                SEAD_ASSERT_MSG(p->isConnectable(this), "illigal parent ( MethodTreeMgr matching failed ).");

                MethodTreeNode* drawNode = p->getMethodTreeNode(3);
                SEAD_ASSERT(drawNode);

                drawNode->pushFrontChild(&mDrawNode);
            }
    }
}

void Task::detachCalcImpl()
{
    mCalcNode.detachAll();
}

void Task::detachDrawImpl()
{
    mDrawNode.detachAll();
}

void Task::pauseCalc(bool b)
{
    if (b)
        mCalcNode.setPauseFlag(MethodTreeNode::cPause_Self);
    else
        mCalcNode.setPauseFlag(MethodTreeNode::cPause_None);
}

void Task::pauseDraw(bool b)
{
    if (b)
        mDrawNode.setPauseFlag(MethodTreeNode::cPause_Self);
    else
        mDrawNode.setPauseFlag(MethodTreeNode::cPause_None);
}

void Task::pauseCalcRec(bool b)
{
    if (b)
        mCalcNode.setPauseFlag(MethodTreeNode::cPause_Both);
    else
        mCalcNode.setPauseFlag(MethodTreeNode::cPause_None);
}

void Task::pauseDrawRec(bool b)
{
    if (b)
        mDrawNode.setPauseFlag(MethodTreeNode::cPause_Both);
    else
        mDrawNode.setPauseFlag(MethodTreeNode::cPause_None);
}

void Task::pauseCalcChild(bool b)
{
    if (b)
        mCalcNode.setPauseFlag(MethodTreeNode::cPause_Child);
    else
        mCalcNode.setPauseFlag(MethodTreeNode::cPause_None);
}

void Task::pauseDrawChild(bool b)
{
    if (b)
        mDrawNode.setPauseFlag(MethodTreeNode::cPause_Child);
    else
        mDrawNode.setPauseFlag(MethodTreeNode::cPause_None);
}

const RuntimeTypeInfo::Interface* Task::getCorrespondingMethodTreeMgrTypeInfo() const
{
    return MethodTreeMgr::getRuntimeTypeInfoStatic();
}

MethodTreeNode* Task::getMethodTreeNode(s32 methodType)
{
    switch (methodType)
    {
        case 0:
            return &mCalcNode;

        case 1:
            return &mCalcNode;

        case 2:
            return &mDrawNode;

        case 3:
            return &mDrawNode;

        case 4:
            return &mDrawNode;

        default:
            SEAD_ASSERT_MSG(false, "Undefined methodType(%d).", methodType);
            return nullptr;
    }
}

} // namespace sead
