#include <heap/seadHeap.h>
#include <heap/seadHeapMgr.h>
#include <prim/seadScopedLock.h>

namespace sead
{

Heap::Heap(const SafeString& name, Heap* parent, void* start, size_t size, HeapDirection direction, bool enableLock): 
    IDisposer(parent, UseSpecifiedOrContainHeap), 
    INamable(name), 
    mStart(start), 
    mSize(size), 
    mParent(parent), 
    mChildren(), 
    mListNode(), 
    mDisposerList(), 
    mDirection(direction), 
    mCS(),
#if defined(SEAD_DEBUG)
    mFlag((1 << cEnableWarning) | (1 << cEnableDebugFillSystem) | (1 << cEnableDebugFillUser)),
#else 
    mFlag(1 << cEnableWarning),
#endif // SEAD_DEBUG
    mHeapCheckTag(static_cast<u16>(HeapMgr::getHeapCheckTag()))
#if defined(SEAD_DEBUG)
    ,
    mAccessThread(NULL)
#endif // SEAD_DEBUG
{
    mFlag.changeBit(cEnableLock, enableLock);

    ConditionalScopedLock<CriticalSection> lock(&mCS, isEnableLock());

    mChildren.initOffset(offsetof(Heap, mListNode));
    mDisposerList.initOffset(IDisposer::getListNodeOffset());
}

Heap::~Heap()
{
}

#if defined(SEAD_DEBUG)
void Heap::listenPropertyEvent(const hostio::PropertyEvent* ev)
{
    switch (ev->id)
    {
        case 'dmpc':
        {
            dump();
            break;
        }
    }
}

void Heap::genMessage(hostio::Context* context)
{
    context->genButton("Dump to console", 'dmpc', "$SEAD_META_HEAP_DMPC", nullptr);

    genInformation_(context);

    for (HeapList::iterator it = mChildren.begin(); it != mChildren.end(); ++it)
    {
        Heap* heap = &(*it);

        FixedSafeString<64> meta;
        heap->makeMetaString_(&meta);

        context->genNode(heap->getName(), heap, meta);
    }
}

void Heap::genInformation_(hostio::Context* context)
{
    hostio::Context::ContextBufferAccessor* ctxBuf = context->beginHTMLLabel("");
    if (ctxBuf)
    {
        BufferedSafeString buf(static_cast<char*>(ctxBuf->getBuffer()), ctxBuf->getMaxSize());
        buf.format(
            "<font face=\"‚l‚r ƒSƒVƒbƒN\"><table><tr><th>Name</th><td>%s</td></tr><tr><th>Range</th><td>" SEAD_FMT_UINTPTR " - " SEAD_FMT_UINTPTR "</td></tr><tr><th>Parent</th><td>%s (0x%08X)</td></tr><tr><th>Direction</th><td>%s</td></tr><tr><th>Size</th><td>%d</td></tr><tr><th>FreeSize</th><td>%d</td></tr><tr><th>MaxAllocatableSize</th><td>%d</td></tr></table></font>",

            getName().cstr(), getStartAddress(), getEndAddress(), getParent() ? getParent()->getName().cstr() : "--", getParent(),
            getDirection() == HeapDirection::eForward ? "Forward" : "Reverse", getSize(), getFreeSize(), getMaxAllocatableSize()
        );

        context->endHTMLLabel(buf.calcLength());
    }
}

void Heap::makeMetaString_(BufferedSafeString* dst)
{
    dst->format("$SEAD_META_HEAP_%03d", static_cast<s32>((1.0f - static_cast<f32>(getFreeSize()) / static_cast<f32>(getSize())) * 10.0f) * 10);
}

#endif // SEAD_DEBUG

Heap* Heap::findContainHeap_(const void* ptr)
{
    if (!isInclude(ptr))
        return nullptr;

    for (HeapList::iterator it = mChildren.begin(); it != mChildren.end(); ++it)
    {
        if (it->isInclude(ptr))
            return it->findContainHeap_(ptr);
    }

    return this;
}

#if defined(SEAD_DEBUG)
bool Heap::isEnableDebugFillAlloc_() const
{
    return HeapMgr::instance()->isEnableDebugFillAlloc() && mFlag.isOnAll((1 << cEnableDebugFillSystem) | (1 << eEnableDebugFillUser));
}

bool Heap::isEnableDebugFillFree_() const
{
    return HeapMgr::instance()->isEnableDebugFillFree() && mFlag.isOnAll((1 << cEnableDebugFillSystem) | (1 << cEnableDebugFillUser));
}

bool Heap::isEnableDebugFillHeapDestroy_() const
{
    return HeapMgr::instance()->isEnableDebugFillHeapDestroy() && mFlag.isOn(1 << cEnableDebugFillUser);
}
#endif // SEAD_DEBUG

void Heap::destruct_()
{
    ConditionalScopedLock<CriticalSection> lock(&mCS, isEnableLock());

    dispose_(nullptr, nullptr);

    if (mParent)
        mParent->eraseChild_(this);
}

void Heap::pushBackChild_(Heap* child)
{
    ConditionalScopedLock<CriticalSection> heapLock(&mCS, isEnableLock());

    mChildren.pushBack(child);
}

void Heap::eraseChild_(Heap* child)
{
    ConditionalScopedLock<CriticalSection> heapLock(&mCS, isEnableLock());

    mChildren.erase(child);
}

void Heap::dispose_(const void* begin, const void* end)
{
    mFlag.setBit(cDisposing);

    for (DisposerList::iterator it = mDisposerList.begin(); it != mDisposerList.end(); )
    {
        if (!it->mDisposerHeap || (begin || end) && !PtrUtil::isInclude(&(*it), begin, end))
        {
            ++it;
            continue;
        }

        it->~IDisposer();

        it = mDisposerList.begin();
    }

    mFlag.resetBit(cDisposing);
}

void Heap::appendDisposer_(IDisposer* disposer)
{
    ConditionalScopedLock<CriticalSection> lock(&mCS, isLockEnabled());
    mDisposerList.pushBack(disposer);
}

void Heap::removeDisposer_(IDisposer* disposer)
{
    ConditionalScopedLock<CriticalSection> lock(&mCS, isLockEnabled());
    mDisposerList.erase(disposer);
}


}  // namespace sead

