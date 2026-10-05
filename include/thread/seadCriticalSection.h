#ifndef SEAD_CRITICAL_SECTION_H_
#define SEAD_CRITICAL_SECTION_H_

#include <nn/os.h>

#include <basis/seadTypes.h>
#include <heap/seadDisposer.h>

namespace sead
{
class Heap;

class CriticalSection : public IDisposer
{
public:
    CriticalSection();
    explicit CriticalSection(Heap* disposer_heap);
    CriticalSection(const CriticalSection&){ };
    virtual ~CriticalSection();

    CriticalSection& operator=(const CriticalSection&){ };

    void lock();
    bool tryLock();
    void unlock();
    
protected:
    nn::os::CriticalSection mCriticalSectionInner;
};

}  // namespace sead

#endif  // SEAD_CRITICAL_SECTION_H_
