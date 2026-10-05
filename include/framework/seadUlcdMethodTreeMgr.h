#pragma once

#include "framework/seadDualScreenMethodTreeMgr.h"

namespace sead
{
class UlcdMethodTreeMgr : public DualScreenMethodTreeMgr
{
    SEAD_RTTI_OVERRIDE(UlcdMethodTreeMgr, DualScreenMethodTreeMgr)
public:
    UlcdMethodTreeMgr();
    virtual ~UlcdMethodTreeMgr();
    virtual void attachMethod(s32 methodType, MethodTreeNode* node);
    virtual MethodTreeNode* getRootMethodTreeNode(s32 methodType);
    virtual void pauseAll(bool p);

    void calc();
    void drawRight();
    void drawLeft();
protected:
    MethodTreeNode mRightRootDrawNode;
    MethodTreeNode mRightSysDrawNode;
    MethodTreeNode mRightAppDrawNode;
    MethodTreeNode mRightAppDrawFinalNode;
};
}