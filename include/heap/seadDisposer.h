#ifndef SEAD_DISPOSER_H_
#define SEAD_DISPOSER_H_

#include <basis/seadNew.h>
#include <basis/seadAssert.h>
#include <basis/seadTypes.h>
#include <container/seadListImpl.h>

namespace sead
{
class Heap;

class IDisposer
{
public:
    enum HeapNullOption
    {
        // disposer_heap must not be nullptr for this option.
        AlwaysUseSpecifiedHeap = 0,
        UseSpecifiedOrContainHeap = 1,
        DoNotAppendDisposerIfNoHeapSpecified = 2,
        UseSpecifiedOrCurrentHeap = 3,
    };

    IDisposer();
    explicit IDisposer(Heap* disposer_heap, HeapNullOption option = UseSpecifiedOrCurrentHeap);
    virtual ~IDisposer();

    static u32 getListNodeOffset() { return offsetof(IDisposer, mListNode); }

protected:
    Heap* getDisposerHeap_() const { return mDisposerHeap; }

private:
    friend class Heap;

    Heap* mDisposerHeap;
    ListNode mListNode;
};

}  // namespace sead

#define SEAD_INSTANCE(CLASS) (CLASS::instance())

#define SEAD_SINGLETON_DISPOSER(CLASS) \
public: \
    class SingletonDisposer_ : public sead::IDisposer \
    { \
    public: \
        SingletonDisposer_(sead::Heap* heap) \
            : sead::IDisposer(heap) \
        { \
        } \
        virtual ~SingletonDisposer_(); \
        static SingletonDisposer_* sStaticDisposer; \
    }; \
    static CLASS* instance() { return sInstance; } \
    static CLASS* createInstance(sead::Heap* heap); \
    static void deleteInstance(); \
private: \
    CLASS(const CLASS&); \
    CLASS& operator=(const CLASS&); \
protected: \
    static CLASS* sInstance; \
    friend class SingletonDisposer_; \
    u32 mSingletonDisposerBuf_[sizeof(SingletonDisposer_) / sizeof(u32)];

#define SEAD_CREATE_SINGLETON_INSTANCE(CLASS) \
    CLASS* CLASS::createInstance(sead::Heap* heap) \
    { \
        if (!sInstance) \
        { \
            u8* buffer = new (heap, __alignof__(CLASS)) u8[sizeof(CLASS)]; \
            SEAD_ASSERT_MSG(!SingletonDisposer_::sStaticDisposer, "Create Singleton Twice (%s).", \
                            #CLASS); \
            u8* disposer_buffer = buffer + offsetof(CLASS, mSingletonDisposerBuf_); \
            SingletonDisposer_::sStaticDisposer = new (disposer_buffer) SingletonDisposer_(heap); \
            sInstance = new (buffer) CLASS; \
        } \
        else \
        { \
            SEAD_ASSERT_MSG(false, "Create Singleton Twice (%s) : addr %p", #CLASS, sInstance); \
        } \
        return CLASS::sInstance; \
    }

#define SEAD_DELETE_SINGLETON_INSTANCE(CLASS) \
    void CLASS::deleteInstance() \
    { \
        SingletonDisposer_* staticDisposer = SingletonDisposer_::sStaticDisposer; \
        if (SingletonDisposer_::sStaticDisposer != NULL) \
        { \
            SingletonDisposer_::sStaticDisposer = NULL; \
            staticDisposer->~SingletonDisposer_(); \
            delete sInstance; \
            sInstance = NULL; \
        } \
    }

#define SEAD_SINGLETON_DISPOSER_IMPL(CLASS) \
    CLASS::SingletonDisposer_::~SingletonDisposer_() \
    { \
        if (this == sStaticDisposer) \
        { \
            sStaticDisposer = NULL; \
            CLASS::sInstance->~CLASS(); \
            CLASS::sInstance = NULL; \
        } \
    } \
    SEAD_CREATE_SINGLETON_INSTANCE(CLASS) \
    SEAD_DELETE_SINGLETON_INSTANCE(CLASS) \
    CLASS* CLASS::sInstance; \
    CLASS::SingletonDisposer_* CLASS::SingletonDisposer_::sStaticDisposer;

#endif  // SEAD_DISPOSER_H_