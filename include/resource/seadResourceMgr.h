#ifndef SEAD_RESOURCEMGR_H_
#define SEAD_RESOURCEMGR_H_

#include <basis/seadTypes.h>
#include <container/seadTList.h>
#include <filedevice/seadFileDevice.h>
#include <heap/seadDisposer.h>
#include <heap/seadHeap.h>
#include <prim/seadSafeString.h>

namespace sead
{
class Resource;
class DirectResource;
class ResourceFactory;
class Decompressor;

template <typename T>
class DirectResourceFactory;

class ResourceMgr
{
    SEAD_SINGLETON_DISPOSER(ResourceMgr)

public:
    struct CreateArg
    {
        CreateArg():
            buffer(nullptr),
            file_size(0),
            buffer_size(0),
            need_unload(false),
            factory(nullptr),
            ext(),
            heap(nullptr),
            alignment(32)
        {
        }

        u8* buffer;
        u32 file_size;
        u32 buffer_size;
        bool need_unload;
        ResourceFactory* factory;
        SafeString ext;
        Heap* heap;
        s32 alignment;
    };

    struct LoadArg
    {
        LoadArg(): 
            path(), 
            instance_heap(nullptr), 
            load_data_heap(nullptr), 
            instance_alignment(32), 
            load_data_alignment(0), 
            load_data_buffer(nullptr), 
            load_data_buffer_size(0), 
            factory(nullptr), 
            device(nullptr), 
            div_size(0),
            assert_on_alloc_fail(true),
            has_tried_create_with_decomp(NULL)
        {
        }

        SafeString path;
        Heap* instance_heap;
        Heap* load_data_heap;
        s32 instance_alignment;
        s32 load_data_alignment;
        u8* load_data_buffer;
        u32 load_data_buffer_size;
        s32 load_data_buffer_alignment;
        ResourceFactory* factory;
        FileDevice* device;
        u32 div_size;
        bool assert_on_alloc_fail;
        bool* has_tried_create_with_decomp;
    };

public:
    ResourceMgr();
    ~ResourceMgr();

    Resource* create(const CreateArg& arg);

    void registerFactory(ResourceFactory* factory, const SafeString& name);
    void unregisterFactory(ResourceFactory* factory);
    ResourceFactory* getDefaultFactory() const { return mDefaultResourceFactory; }
    /// Set the specified factory as the default factory. Its name is set to "".
    /// @param factory  If null, a dummy resource factory is set as the default factory.
    /// @returns the previous default factory
    ResourceFactory* setDefaultFactory(ResourceFactory* factory);
    ResourceFactory* findFactory(const SafeString& name);

    void registerDecompressor(Decompressor* decompressor, const SafeString& name);
    void unregisterDecompressor(Decompressor* decompressor);
    Decompressor* findDecompressor(const SafeString& name);

    Resource* tryLoad(const LoadArg& arg, const SafeString& factory_name, Decompressor* decompressor);
    Resource* tryLoadWithoutDecomp(const LoadArg& arg);
    void unload(Resource* res);

private:
    typedef TList<ResourceFactory*> FactoryList;
    typedef TList<Decompressor*> DecompressorList;

    FactoryList mFactoryList;
    DecompressorList mDecompList;
    ResourceFactory* mNullResourceFactory;
    ResourceFactory* mDefaultResourceFactory;
};

}  // namespace sead

#endif  // SEAD_RESOURCEMGR_H_
