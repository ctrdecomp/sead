#pragma once

#include "container/seadTreeNode.h"
#include "hostio/seadHostIOReflexible.h"

namespace sead
{
namespace hostio
{
class Node : public Reflexible
{
public:
    NodeClassType getNodeClassType() const override { return Reflexible::NodeClassType::cNode; }

#ifdef SEAD_DEBUG
public:
    Node();
    Node(Heap* heap, IDisposer::HeapNullOption heap_null_option);
    virtual ~Node() { disposeHostIOImpl_(); }

    void appendChild(Node* node);
    void insertAfterSelf(Node* node);
    void insertBeforeSelf(Node* node);
    void detachAll();
    void detach();
    void destroy() { detachAll(); }
    Node* getParentNode() const;
    Node* getChildNode() const;
    Node* getNextNode() const;
    Node* getPrevNode() const;

    bool isAppended() const;

    virtual Reflexible* searchNode(const SafeString& name);
    virtual void calcURL(BufferedSafeString* url) const { calcURLImpl_(url, true); }

protected:
    virtual void genChildNode(Context* context);
    virtual bool isHaveChild() const { return mTreeNode.child() != nullptr; }
    virtual void disposeHostIO()
    {
        disposeHostIOImpl_();
        Reflexible::disposeHostIO();
    }

private:
    void disposeHostIOImpl_();
    void calcURLImpl_(BufferedSafeString* url, bool) const;

    TTreeNode<Node*> mTreeNode;
#endif
};
}  // namespace hostio
}  // namespace sead
