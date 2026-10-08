#pragma once

#include <framework/seadMethodTree.h>
#include <framework/seadMethodTreeMgr.h>

namespace sead 
{
class SingleScreenMethodTreeMgr : public MethodTreeMgr
{
    SEAD_RTTI_OVERRIDE(SingleScreenMethodTreeMgr, MethodTreeMgr);

public:
    SingleScreenMethodTreeMgr();
    virtual ~SingleScreenMethodTreeMgr();
    virtual void attachMethod(s32 methodType, MethodTreeNode* node);
    virtual MethodTreeNode* getRootMethodTreeNode(s32 methodType);
    virtual void pauseAll(bool p);
    virtual void pauseAppCalc(bool p);

    void calc();
    void draw();

protected:
    MethodTreeNode mRootCalcNode;
    MethodTreeNode mSysCalcNode;
    MethodTreeNode mAppCalcNode;
    MethodTreeNode mRootDrawNode;
    MethodTreeNode mSysDrawNode;
    MethodTreeNode mAppDrawNode;
    MethodTreeNode mAppDrawFinalNode;
};

} // namespace sead
