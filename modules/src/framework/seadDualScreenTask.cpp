#include "framework/seadDualScreenTask.h"
#include "framework/seadDualScreenMethodTreeMgr.h"
#include "framework/seadFramework.h"

namespace sead
{
DualScreenTask::DualScreenTask(const TaskConstructArg& arg):
    TaskBase(arg), mCalcNode(NULL), mTopNode(NULL),mBtmNode(NULL)
{
    mCalcNode.setPauseFlag(MethodTreeNode::cPause_None);
    mTopNode.setPauseFlag(MethodTreeNode::cPause_None);
    mBtmNode.setPauseFlag(MethodTreeNode::cPause_None);

    mCalcNode.bind(this, &DualScreenTask::calc, "Task");
    mTopNode.bind(this, &DualScreenTask::drawTop, "Task");
    mBtmNode.bind(this, &DualScreenTask::drawBtm,"Task");
}

DualScreenTask::DualScreenTask(const TaskConstructArg& arg, const char* name):
    TaskBase(arg, name), mCalcNode(NULL), mTopNode(NULL),mBtmNode(NULL)
{
    mCalcNode.setPauseFlag(MethodTreeNode::cPause_None);
    mTopNode.setPauseFlag(MethodTreeNode::cPause_None);
    mBtmNode.setPauseFlag(MethodTreeNode::cPause_None);

    mCalcNode.bind(this, &DualScreenTask::calc, name);
    mTopNode.bind(this, &DualScreenTask::drawTop, name);
    mBtmNode.bind(this, &DualScreenTask::drawBtm, name);
}

DualScreenTask::~DualScreenTask()
{
}

void DualScreenTask::pauseCalc(bool b)
{
    if(b)
    {
        mCalcNode.setPauseFlag(MethodTreeNode::cPause_Self);
    }
    else
    {
        mCalcNode.setPauseFlag(MethodTreeNode::cPause_None);
    }
}

void DualScreenTask::pauseDraw(bool b)
{
    if(b)
    {
        mTopNode.setPauseFlag(MethodTreeNode::cPause_Self);
        mBtmNode.setPauseFlag(MethodTreeNode::cPause_Self);
    }
    else
    {
        mTopNode.setPauseFlag(MethodTreeNode::cPause_None);
        mBtmNode.setPauseFlag(MethodTreeNode::cPause_None);
    }
}

void DualScreenTask::pauseDrawRec(bool b)
{
    if(b)
    {
        mTopNode.setPauseFlag(MethodTreeNode::cPause_Both);
        mBtmNode.setPauseFlag(MethodTreeNode::cPause_Both);
    }
    else
    {
        mTopNode.setPauseFlag(MethodTreeNode::cPause_None);
        mBtmNode.setPauseFlag(MethodTreeNode::cPause_None);
    }
}

void DualScreenTask::pauseCalcRec(bool b)
{
    if(b)
    {
        mCalcNode.setPauseFlag(MethodTreeNode::cPause_Both);
    }
    else
    {
        mCalcNode.setPauseFlag(MethodTreeNode::cPause_None);
    }
}

void DualScreenTask::detachCalcImpl()
{
    mCalcNode.detachAll();
}

void DualScreenTask::detachDrawImpl()
{
    mTopNode.detachAll();
    mBtmNode.detachAll();
}

void DualScreenTask::attachCalcImpl()
{
    ScopedLock<CriticalSection> lock(getMethodTreeMgr()->getTreeCriticalSection());

    TaskBase* p = parent() ? parent()->val() : nullptr;

    switch(getTag())
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

void DualScreenTask::attachDrawImpl()
{
    ScopedLock<CriticalSection> lock(getMethodTreeMgr()->getTreeCriticalSection());

    TaskBase* p = parent() ? parent()->val() : nullptr;

    switch(getTag())
    {
    case cSystem:
        attachMethodWithCheck(5, &mTopNode);
        attachMethodWithCheck(8, &mBtmNode);
        break;
    default:
        SEAD_ASSERT_MSG(false, "Undefined Tag(%d).", getTag());
        //! Fallthrough
    case cApp:
        if (!p)
        {
            attachMethodWithCheck(6, &mTopNode);
            attachMethodWithCheck(9, &mBtmNode);
        }
        else
        {
            SEAD_ASSERT_MSG(p->isConnectable(this), "illigal parent ( MethodTreeMgr matching failed ).");

            MethodTreeNode* topDraw = p->getMethodTreeNode(6);
            SEAD_ASSERT(topDraw);
            topDraw->pushBackChild(&mTopNode);

            MethodTreeNode* btmDraw = p->getMethodTreeNode(9);
            SEAD_ASSERT(btmDraw);
            btmDraw->pushBackChild(&mBtmNode);
        }
    }
}

const RuntimeTypeInfo::Interface* DualScreenTask::getCorrespondingMethodTreeMgrTypeInfo() const
{
    return DualScreenMethodTreeMgr::getRuntimeTypeInfoStatic();
}

MethodTreeNode* DualScreenTask::getMethodTreeNode(s32 methodType)
{
    switch (methodType)
    {
        case 0:
            return nullptr;

        case 1:
            return &mCalcNode;

        case 2:
        {
            DualScreenMethodTreeMgr* tree = DynamicCast<DualScreenMethodTreeMgr>(getMethodTreeMgr());

            SEAD_ASSERT(tree);

            if (tree->getSysDrawScreen() == 0)
                return &mCalcNode;
            else
                return &mTopNode;
        }

        case 3:
            return nullptr;

        case 4:
        {
            DualScreenMethodTreeMgr* tree = DynamicCast<DualScreenMethodTreeMgr>(getMethodTreeMgr());

            SEAD_ASSERT(tree);

            if (tree->getAppDrawScreen() == 0)
                return &mCalcNode;
            else
                return &mTopNode;
        }

        case 5:
            return nullptr;

        case 6:
            return nullptr;

        case 7:
            return &mCalcNode;

        case 8:
            return nullptr;

        case 9:
            return nullptr;

        case 10:
            return &mTopNode;

        default:
            SEAD_ASSERT_MSG(false, "Undefined method_type(%d).", methodType);
            return nullptr;
    }
}

}
