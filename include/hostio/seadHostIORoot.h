#pragma once

#include <hostio/seadHostIONode.h>

namespace sead {

class HostIORoot : public hostio::Node
{
public:
    HostIORoot(): 
        hostio::Node()
    {
    }

#if defined(SEAD_DEBUG)
    HostIORoot(Heap* disposerHeap, IDisposer::HeapNullOption option): 
        hostio::Node(disposerHeap, option)
    {
    }

    virtual void listenPropertyEvent(const hostio::PropertyEvent* ev);
    virtual void genMessage(hostio::Context* ctx);
#endif // SEAD_DEBUG
};

} // namespace sead