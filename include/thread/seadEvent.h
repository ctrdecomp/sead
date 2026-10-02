#pragma once

#include <nn/os.h>

#include "heap/seadDisposer.h"
#include "time/seadTickSpan.h"

namespace sead
{
class Heap;

class Event : public IDisposer
{
public:
    Event();
    explicit Event(bool manual_reset);
    explicit Event(Heap* disposer_heap);
    Event(Heap* disposer_heap, bool manual_reset);
    Event(Heap* disposer_heap, IDisposer::HeapNullOption heap_null_option);
    Event(Heap* disposer_heap, IDisposer::HeapNullOption heap_null_option, bool manual_reset);
    virtual ~Event();

    void initialize(bool manual_reset);
    void wait();
    bool wait(TickSpan duration);
    void setSignal();
    void resetSignal();

private:
    Event(const Event&);
    Event& operator=(const Event&);
    
    void setInitialized(bool initialized)
    {
#ifdef SEAD_DEBUG
        mInitialized = initialized;
#endif
    }

    nn::os::Event mEventInner;

#ifdef SEAD_DEBUG
    bool mInitialized;
#endif
};
}  // namespace sead
