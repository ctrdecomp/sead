#include <filedevice/seadFileDeviceMgr.h>
#include <heap/seadHeap.h>
#include <heap/seadHeapMgr.h>
#include <math/seadMathCalcCommon.h>
#include <prim/seadBitUtil.h>
#include <prim/seadEndian.h>
#include <prim/seadPtrUtil.h>
#include <prim/seadSafeString.h>
#include <resource/seadSZSDecompressor.h>

namespace
{
asm s32 decodeSZSCtrAsm_(void* dst, const void* src)
{
    push {r4-r8,lr}
    ldr r4, [r1,#4]
    eor r5, r4, r4,ror#16
    bic r5, r5, #0xff0000
    mov r4, r4,ror#8
    eor r4, r4, r5,lsr#8
    mov r2, r4
    add r1, r1, #0x10
    mov r5, #0
    mov lr, #0x1000
    sub lr, lr, #1

_decloop0
    movs r5, r5,lsr#1
    bne _decloop1
    ldrb r6, [r1],#1
    mov r5, #0x80

_decloop1
    tst r6, r5
    bne _decloop5
    ldrb r3, [r1],#1
    ldrb r7, [r1],#1
    add r3, r7, r3,lsl#8
    movs r7, r3,lsr#12
    and r3, r3, lr
    add r3, r3, #1
    add r7, r7, #2
    bne _decloop2
    ldrb r7, [r1],#1
    add r7, r7, #0x12

_decloop2
    sub r4, r4, r7

_decloop3
    ldrb r12, [r0,-r3]
    subs r7, r7, #1
    strb r12, [r0],#1
    bne _decloop3
    cmp r4, #0
    bne _decloop0
    b _decloop8

_decloop5
    ldrb r12, [r1],#1
    subs r4, r4, #1
    strb r12, [r0],#1
    bne _decloop0

_decloop8
    pop {r4-r8,lr}
    mov r0, r2
    bx lr
}
}  // namespace

namespace sead
{
SZSDecompressor::DecompContext::DecompContext()
{
    initialize(NULL);
}

SZSDecompressor::DecompContext::DecompContext(void* dst)
{
    initialize(dst);
}

void SZSDecompressor::DecompContext::initialize(void* dst)
{
    destp = static_cast<u8*>(dst);
    destCount = 0;
    forceDestCount = 0;
    flagMask = 0;
    flags = 0;
    packHigh = 0;
    step = SZSDecompressor::cStepNormal;
    lzOffset = 0;
    headerSize = 0x10;
}

SZSDecompressor::SZSDecompressor(u32 workSize, u8* workBuffer) : Decompressor("szs")
{
    if (workBuffer == NULL)
    {
        mWorkSize = Mathu::roundUpPow2(workSize, FileDevice::cBufferMinAlignment);
        mWorkBuffer = NULL;
    }

    else
    {
        mWorkSize = workSize;
        mWorkBuffer = workBuffer;
    }
}

u8* SZSDecompressor::tryDecompFromDevice(const ResourceMgr::LoadArg& loadArg, Resource* resource,
                                         u32* outSize, u32* outAllocSize, bool* outAllocated)
{
    Heap* heap = loadArg.load_data_heap;
    if (heap == NULL)
        heap = HeapMgr::sInstancePtr->getCurrentHeap();

    FileHandle handle;
    FileDevice* device;
    u8* src;

    if (loadArg.device != NULL)
        device = loadArg.device->tryOpen(&handle, loadArg.path, FileDevice::cFileOpenFlag_ReadOnly,
                                         loadArg.div_size);
    else
        device = FileDeviceMgr::instance()->tryOpen(
            &handle, loadArg.path, FileDevice::cFileOpenFlag_ReadOnly, loadArg.div_size);

    if (device != NULL &&
        ((src = mWorkBuffer, src != NULL) ||
         (src = new (heap, -FileDevice::cBufferMinAlignment) u8[mWorkSize], src != NULL)))
    {
        u32 bytesRead = handle.read(src, mWorkSize);
        if (bytesRead >= 0x10)
        {
            u32 decompSize = getDecompSize(src);
            s32 decompAlignment = getDecompAlignment(src);

            u32 allocSize = loadArg.load_data_buffer_size;
            u8* dst = loadArg.load_data_buffer;

            if (decompSize > allocSize && allocSize != 0)
                decompSize = allocSize;

            bool allocated = false;
            allocSize = Mathu::roundUpPow2(decompSize, 0x20);

            if (dst == NULL)
            {
                DirectResource* directResource = DynamicCast<DirectResource, Resource>(resource);
                if (directResource != NULL)
                {
                    s32 alignment = loadArg.load_data_alignment;
                    if (alignment != 0)
                        decompAlignment = (alignment < 0x20) ? 0x20 : alignment;

                    else
                    {
                        if (decompAlignment == 0)
                            decompAlignment = directResource->getLoadDataAlignment();

                        decompAlignment = ((loadArg.instance_alignment < 0) ? -1 : 1) *
                                          ((decompAlignment < 0x20) ? 0x20 : decompAlignment);
                    }
                }

                else
                    decompAlignment = -(((loadArg.instance_alignment < 0) ? -1 : 1) << 5);

                dst = new (heap, decompAlignment) u8[allocSize];

                if (dst != NULL)
                    allocated = true;
            }

            if (dst != NULL)
            {
                s32 error;
                if (bytesRead < mWorkSize)
                    error = decomp(dst, allocSize, src, mWorkSize);

                else
                {
                    DecompContext context(dst);
                    context.forceDestCount = decompSize;

                    do
                    {
                        error = streamDecomp(&context, src, bytesRead);
                        if (error <= 0)
                            break;
                    } while ((bytesRead = handle.read(src, mWorkSize), bytesRead != 0));
                }

                if (!(error < 0))
                {
                    if (mWorkBuffer == NULL)
                        delete[] src;

                    if (outSize != NULL)
                        *outSize = decompSize;

                    if (outAllocSize != NULL)
                        *outAllocSize = allocSize;

                    if (outAllocated != NULL)
                        *outAllocated = allocated;

                    return dst;
                }

                if (allocated)
                    delete[] dst;
            }
        }

        if (mWorkBuffer == NULL)
            delete[] src;
    }

    return NULL;
}

u32 SZSDecompressor::getDecompAlignment(const void* src)
{
    return Endian::toHostU32(Endian::cBig, BitUtil::bitCastPtr<u32>(src, 8));
}

u32 SZSDecompressor::getDecompSize(const void* src)
{
    return Endian::toHostU32(Endian::cBig, BitUtil::bitCastPtr<u32>(src, 4));
}

s32 SZSDecompressor::readHeader_(DecompContext* context, const u8* src, u32 src_size)
{
    s32 len = 0;

    while (context->headerSize != 0)
    {
        context->headerSize -= 1;

        if (context->headerSize == 0xF)
        {
            if (*src != 0x59)
                return -1;
        }

        else if (context->headerSize == 0xE)
        {
            if (*src != 0x61)
                return -1;
        }

        else if (context->headerSize == 0xD)
        {
            if (*src != 0x7A)
                return -1;
        }

        else if (context->headerSize == 0xC)
        {
            if (*src != 0x30)
                return -1;
        }

        else if (7 < context->headerSize)
            context->destCount |= static_cast<u32>(*src) << (context->headerSize - 8) * 8;

        src++;
        len += 1;
        if (--src_size == 0 && context->headerSize != 0)
            return len;
    }

    if (context->forceDestCount < 1)
        return len;

    if (context->destCount <= context->forceDestCount)
        return len;

    context->destCount = context->forceDestCount;
    return len;
}

s32 SZSDecompressor::streamDecomp(DecompContext* context, const void* src, u32 src_size)
{
    const u8* _src = static_cast<const u8*>(src);
    u32 n;

    if (context->headerSize != 0)
    {
        s32 len = readHeader_(context, _src, src_size);
        if (len < 0)
            return len;

        src_size -= len;
        _src += len;

        if (src_size == 0)
        {
            if (context->headerSize == 0)
                return context->destCount;

            return -1;
        }
    }

    while (context->destCount > 0)
    {
        if (context->step == cStepLong)
        {
            n = *_src + 0x12;
            if (!context->doCopy(n))
                return -2;
        }

        else if (context->step == cStepShort)
        {
            context->lzOffset = (((context->packHigh << 8) & 0xf00) | *_src) + 1;

            n = context->packHigh >> 4;
            if (n != 0)
            {
                n += 2;
                if (!context->doCopy(n))
                    return -2;
            }

            else
                context->step = cStepLong;
        }

        else
        {
            if (context->flagMask == 0)
            {
                context->flags = *_src++;
                context->flagMask = 0x80;
                if (--src_size == 0)
                    break;
            }

            if ((context->flags & context->flagMask) == 0)
            {
                context->packHigh = *_src;
                context->step = cStepShort;
            }

            else
            {
                *context->destp++ = *_src;
                context->destCount -= 1;
            }

            context->flagMask >>= 1;
        }

        if (--src_size == 0)
            break;

        _src++;
    }

    if (context->destCount == 0 && context->forceDestCount == 0 && 0x20 < src_size)
        return -1;

    else
        return context->destCount;
}

s32 SZSDecompressor::decomp(void* dst, u32 dstSize, const void* src, u32 src_size)
{
    SEAD_ASSERT(dst);
    SEAD_ASSERT(src);

    u32 magic = Endian::toHostU32(Endian::cBig, BitUtil::bitCastPtr<u32>(src));
    if (magic != 0x59617A30)
        return -1;

    u32 decompSize = getDecompSize(src);
    s32 error = -2;
    if (dstSize >= decompSize)
    {
        error = decodeSZSCtrAsm_(dst, src);
    }

    return error;
}

}  // namespace sead
