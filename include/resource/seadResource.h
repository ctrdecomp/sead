#ifndef SEAD_RESOURCE_H_
#define SEAD_RESOURCE_H_

#include <basis/seadNew.h>
#include <basis/seadTypes.h>
#include <container/seadTList.h>
#include <heap/seadDisposer.h>
#include <heap/seadHeap.h>
#include <prim/seadBitFlag.h>
#include <resource/seadDecompressor.h>
#include <resource/seadResourceMgr.h>

namespace sead
{
class ReadStream;

class Resource
{
public:
    SEAD_RTTI_BASE(Resource)

    Resource();
    virtual ~Resource();
};

class DirectResource : public Resource
{
    SEAD_RTTI_OVERRIDE(DirectResource, Resource)

public:
    DirectResource();
    virtual ~DirectResource();
    virtual s32 getLoadDataAlignment() const { return 4; }

    void create(u8* buffer, u32 bufferSize, u32 allocSize, bool allocated, Heap* heap);

    u8* getRawData() const { return mRawData; }
    u32 getRawSize() const { return mRawSize; }
    u32 getBufferSize() const { return mBufferSize; }

    static const size_t cLoadDataAlignment = 4;

protected:
    virtual void doCreate_(u8* buffer, u32 bufferSize, Heap* heap)
    {
        SEAD_UNUSED(buffer);
        SEAD_UNUSED(bufferSize);
        SEAD_UNUSED(heap);
    }
    u8* mRawData;
    u32 mRawSize;
    u32 mBufferSize;
    BitFlag32 mSettingFlag;
};

class IndirectResource : public Resource
{
    SEAD_RTTI_OVERRIDE(IndirectResource, Resource)

public:
    IndirectResource();

    void create(sead::ReadStream* stream, u32 size, sead::Heap* heap);

protected:
    virtual void doCreate_(ReadStream* stream, u32 bufferSize, Heap* heap)
    {
        SEAD_UNUSED(stream);
        SEAD_UNUSED(bufferSize);
        SEAD_UNUSED(heap);
    }
};

class ResourceFactory : public TListNode<ResourceFactory*>, public IDisposer
{
    SEAD_RTTI_BASE(ResourceFactory)
public:
    ResourceFactory() : TListNode<ResourceFactory*>(this), IDisposer(), mExt() {}

    virtual ~ResourceFactory();

    virtual Resource* tryCreate(const ResourceMgr::LoadArg& loadArg) = 0;
    virtual Resource* tryCreateWithDecomp(const ResourceMgr::LoadArg& loadArg,
                                          Decompressor* decompressor) = 0;
    virtual Resource* create(const ResourceMgr::CreateArg& createArg) = 0;

    const SafeString& getExt() const { return mExt; }
    void setExt(const SafeString& ext) { mExt = ext; }

protected:
    FixedSafeString<32> mExt;
};

class DirectResourceFactoryBase : public ResourceFactory
{
    SEAD_RTTI_OVERRIDE(DirectResourceFactoryBase, ResourceFactory)
public:
    DirectResourceFactoryBase() : ResourceFactory() {}

    virtual ~DirectResourceFactoryBase(){}

    virtual Resource* create(const ResourceMgr::CreateArg& createArg);
    virtual Resource* tryCreate(const ResourceMgr::LoadArg& loadArg);
    virtual Resource* tryCreateWithDecomp(const ResourceMgr::LoadArg& loadArg, Decompressor* decompressor);
    virtual DirectResource* newResource_(Heap* heap, s32 alignment) = 0;
};

template <typename T>
class DirectResourceFactory : public DirectResourceFactoryBase
{
    SEAD_RTTI_OVERRIDE(DirectResourceFactory<T>, DirectResourceFactoryBase)
public:
    DirectResourceFactory() : DirectResourceFactoryBase() {}

    virtual ~DirectResourceFactory() {}

    DirectResource* newResource_(Heap* heap, s32 alignment)
    {
        return new (heap, alignment) T;
    }
};

class IndirectResourceFactoryBase : public ResourceFactory
{
    SEAD_RTTI_OVERRIDE(IndirectResourceFactoryBase, ResourceFactory)
public:
    IndirectResourceFactoryBase() : ResourceFactory() {}

    virtual ~IndirectResourceFactoryBase(){}
    virtual Resource* create(const ResourceMgr::CreateArg& createArg);
    virtual Resource* tryCreate(const ResourceMgr::LoadArg& loadArg);
    virtual Resource* tryCreateWithDecomp(const ResourceMgr::LoadArg& loadArg, Decompressor* decompressor);
    virtual IndirectResource* newResource_(Heap* heap, s32 alignment) = 0;
};

}  // namespace sead

#endif  // SEAD_RESOURCE_H_
