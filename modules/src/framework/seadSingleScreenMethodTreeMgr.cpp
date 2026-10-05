#include <framework/seadSingleScreenMethodTreeMgr.h>

namespace sead 
{
SingleScreenMethodTreeMgr::SingleScreenMethodTreeMgr(): 
    MethodTreeMgr(), 
    mRootCalcNode(getTreeCriticalSection()), 
    mSysCalcNode(getTreeCriticalSection()), 
    mAppCalcNode(getTreeCriticalSection()), 
    mRootDrawNode(getTreeCriticalSection()), 
    mSysDrawNode(getTreeCriticalSection()), 
    mAppDrawNode(getTreeCriticalSection()), 
    mAppDrawFinalNode(getTreeCriticalSection())
{
    mRootCalcNode.setName("sead::RootCalc");
    mSysCalcNode.setName("sead::SysCalc");
    mAppCalcNode.setName("sead::AppCalc");

    mRootDrawNode.setName("sead::RootDraw");
    mSysDrawNode.setName("sead::SysDraw");
    mAppDrawNode.setName("sead::AppDraw");
    mAppDrawFinalNode.setName("sead::AppDrawFinal");

    mRootCalcNode.pushBackChild(&mSysCalcNode);
    mRootCalcNode.pushBackChild(&mAppCalcNode);

    mRootDrawNode.pushBackChild(&mAppDrawNode);
    mRootDrawNode.pushBackChild(&mAppDrawFinalNode);
    mRootDrawNode.pushBackChild(&mSysDrawNode);

    mSysCalcNode.setPauseFlag(MethodTreeNode::cPause_None);
    mSysDrawNode.setPauseFlag(MethodTreeNode::cPause_None);

    mAppCalcNode.setPauseFlag(MethodTreeNode::cPause_None);
    mAppDrawNode.setPauseFlag(MethodTreeNode::cPause_None);
    mAppDrawFinalNode.setPauseFlag(MethodTreeNode::cPause_None);

    mRootCalcNode.setPauseFlag(MethodTreeNode::cPause_Both);
    mRootDrawNode.setPauseFlag(MethodTreeNode::cPause_Both);
}

SingleScreenMethodTreeMgr::~SingleScreenMethodTreeMgr()
{
}

void SingleScreenMethodTreeMgr::attachMethod(s32 methodType, MethodTreeNode* node)
{
    switch (methodType)
    {
        case 0:
            mSysCalcNode.pushBackChild(node);
            break;

        case 1:
            mAppCalcNode.pushBackChild(node);
            break;

        case 2:
            mSysDrawNode.pushFrontChild(node);
            break;

        case 3:
            mAppDrawNode.pushFrontChild(node);
            break;

        case 4:
            mAppDrawFinalNode.pushFrontChild(node);
            break;

        default:
            SEAD_ASSERT_MSG(false, "Undefined MethodType(%d).", methodType);
    }
}

MethodTreeNode* SingleScreenMethodTreeMgr::getRootMethodTreeNode(s32 methodType)
{
    switch (methodType)
    {
        case 0:
            return &mSysCalcNode;

        case 1:
            return &mAppCalcNode;

        case 2:
            return &mSysDrawNode;

        case 3:
            return &mAppDrawNode;

        case 4:
            return &mAppDrawFinalNode;

        default:
            SEAD_ASSERT_MSG(false, "Undefined MethodType(%d)", methodType);
            return nullptr;
    }
}

void SingleScreenMethodTreeMgr::pauseAll(bool p)
{
    if (p)
    {
        mRootCalcNode.setPauseFlag(MethodTreeNode::cPause_Both);
        mRootDrawNode.setPauseFlag(MethodTreeNode::cPause_Both);
    }
    else
    {
        mRootCalcNode.setPauseFlag(MethodTreeNode::cPause_None);
        mRootDrawNode.setPauseFlag(MethodTreeNode::cPause_None);
    }
}

void SingleScreenMethodTreeMgr::pauseAppCalc(bool p)
{
    if (p)
        mAppCalcNode.setPauseFlag(MethodTreeNode::cPause_Both);
    else
        mAppCalcNode.setPauseFlag(MethodTreeNode::cPause_None);
}

void SingleScreenMethodTreeMgr::calc()
{
    mRootCalcNode.call();
}

void SingleScreenMethodTreeMgr::draw()
{
    mRootDrawNode.call();
}

} // namespace sead