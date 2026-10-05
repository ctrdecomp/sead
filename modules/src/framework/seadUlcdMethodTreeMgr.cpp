#include <framework/seadUlcdMethodTreeMgr.h>

namespace sead 
{
UlcdMethodTreeMgr::UlcdMethodTreeMgr(): 
    DualScreenMethodTreeMgr(),
    mRightRootDrawNode(getTreeCriticalSection()), 
    mRightSysDrawNode(getTreeCriticalSection()), 
    mRightAppDrawNode(getTreeCriticalSection()), 
    mRightAppDrawFinalNode(getTreeCriticalSection())
{
    mRightRootDrawNode.setName("sead::RightRootDraw");
    mRightSysDrawNode.setName("sead::RightSysDraw");
    mRightAppDrawNode.setName("sead::RightAppDraw");
    mRightAppDrawFinalNode.setName("sead::RightAppDrawFinal");

    mRightRootDrawNode.pushBackChild(&mRightAppDrawNode);
    mRightRootDrawNode.pushBackChild(&mRightAppDrawFinalNode);
    mRightRootDrawNode.pushBackChild(&mRightSysDrawNode);

    mRightSysDrawNode.setPauseFlag(MethodTreeNode::cPause_None);
    mRightAppDrawNode.setPauseFlag(MethodTreeNode::cPause_None);
    mRightAppDrawFinalNode.setPauseFlag(MethodTreeNode::cPause_None);
    mRightRootDrawNode.setPauseFlag(MethodTreeNode::cPause_Both);
}

UlcdMethodTreeMgr::~UlcdMethodTreeMgr()
{
}

void UlcdMethodTreeMgr::attachMethod(s32 methodType, MethodTreeNode* node)
{
    switch (methodType)
    {
        case 11:
            DualScreenMethodTreeMgr::attachMethod(5, node);
            break;

        case 12:
            DualScreenMethodTreeMgr::attachMethod(6, node);
            break;

        case 13:
            DualScreenMethodTreeMgr::attachMethod(7, node);
            break;

        case 14:
            mRightSysDrawNode.pushFrontChild(node);
            break;

        case 15:
            mRightAppDrawNode.pushFrontChild(node);
            break;

        case 16:
            mRightAppDrawFinalNode.pushFrontChild(node);
            break;

        default:
            DualScreenMethodTreeMgr::attachMethod(methodType, node);            
            break;
    }
}

MethodTreeNode* UlcdMethodTreeMgr::getRootMethodTreeNode(s32 methodType)
{
    switch (methodType)
    {
        case 11:
            return DualScreenMethodTreeMgr::getRootMethodTreeNode(5);

        case 12:
            return DualScreenMethodTreeMgr::getRootMethodTreeNode(6);

        case 13:
            return DualScreenMethodTreeMgr::getRootMethodTreeNode(7);

        case 14:
            return &mRightSysDrawNode;

        case 15:
            return &mRightAppDrawNode;
        
        case 16:
            return &mRightAppDrawNode;

        default:
            return DualScreenMethodTreeMgr::getRootMethodTreeNode(methodType);
    }
}

void UlcdMethodTreeMgr::pauseAll(bool p)
{
    DualScreenMethodTreeMgr::pauseAll(p);
    if (p)
    {
        mRightRootDrawNode.setPauseFlag(MethodTreeNode::cPause_Both);
    }
    else
    {
        mRightRootDrawNode.setPauseFlag(MethodTreeNode::cPause_None);
    }
}

void UlcdMethodTreeMgr::drawRight()
{
    mRightRootDrawNode.call();
}

void UlcdMethodTreeMgr::drawLeft()
{
    DualScreenMethodTreeMgr::drawTop();
}

} // namespace sead