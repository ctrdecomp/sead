#include <algorithm>
#include <basis/seadNew.h>
#include <container/seadPtrArray.h>

namespace sead
{
void PtrArrayImpl::setBuffer(s32 ptrNumMax, void* buf)
{
    if (ptrNumMax < 1)
    {
        SEAD_ASSERT_MSG(false, "ptrNumMax[%d] must be larger than zero", ptrNumMax);
        return;
    }

    if (buf == NULL)
    {
        SEAD_ASSERT_MSG(false, "buf is null");
        return;
    }

    mPtrs = static_cast<void**>(buf);
    mPtrNum = 0;
    mPtrNumMax = ptrNumMax;
}

void PtrArrayImpl::allocBuffer(s32 ptrNumMax, Heap* heap, s32 alignment)
{
    SEAD_ASSERT(mPtrs == nullptr);

    if (ptrNumMax < 1)
    {
        SEAD_ASSERT_MSG(false, "ptrNumMax[%d] must be larger than zero", ptrNumMax);
        return;
    }

    setBuffer(ptrNumMax, new (heap, alignment, std::nothrow) u8[s32(sizeof(void*)) * ptrNumMax]);
}

bool PtrArrayImpl::tryAllocBuffer(s32 ptrNumMax, Heap* heap, s32 alignment)
{
    SEAD_ASSERT(mPtrs == nullptr);

    if (ptrNumMax < 1)
    {
        SEAD_ASSERT_MSG(false, "ptrNumMax[%d] must be larger than zero", ptrNumMax);
        return false;
    }

    u8* buf = new (heap, alignment, std::nothrow) u8[s32(sizeof(void*)) * ptrNumMax];
    if (!buf)
        return false;

    setBuffer(ptrNumMax, buf);
    return true;
}

void PtrArrayImpl::freeBuffer()
{
    if (isBufferReady())
    {
        delete[] mPtrs;
        mPtrs = nullptr;
        mPtrNum = 0;
        mPtrNumMax = 0;
    }
}

void PtrArrayImpl::erase(s32 pos, s32 count)
{
    if (pos < 0)
    {
        SEAD_ASSERT_MSG(false, "illegal position[%d]", pos);
        return;
    }

    if (count < 0)
    {
        SEAD_ASSERT_MSG(false, "illegal number[%d]", count);
        return;
    }

    if (pos + count > mPtrNum)
    {
        SEAD_ASSERT_MSG(false, "pos[%d] + num[%d] exceed size[%d]", pos, count, mPtrNum);
        return;
    }

    const s32 endPos = pos + count;
    if (mPtrNum > endPos)
        MemUtil::copyOverlap(mPtrs + pos, mPtrs + endPos, sizeof(void*) * (mPtrNum - endPos));

    mPtrNum -= count;
}

// NON_MATCHING: semantically equivalent
void PtrArrayImpl::reverse()
{
    for (s32 i = 0; i < mPtrNum / 2; ++i)
        swap(mPtrNum - i - 1, i);
}

// Fisher–Yates shuffle.
void PtrArrayImpl::shuffle(Random* random)
{
    SEAD_ASSERT(random);
    for (s32 i = mPtrNum - 1; i > 0; --i)
        swap(i, random->getS32Range(0, i + 1));
}

void PtrArrayImpl::insert(s32 pos, void* ptr)
{
    if (!checkInsert(pos, 1))
        return;

    createVacancy(pos, 1);
    mPtrs[pos] = ptr;
    ++mPtrNum;
}

bool PtrArrayImpl::checkInsert(s32 pos, s32 num)
{
    if (pos < 0)
    {
        SEAD_ASSERT_MSG(false, "illegal position[%d]", pos);
        return false;
    }

    if (mPtrNum + num > mPtrNumMax)
    {
        SEAD_ASSERT_MSG(false, "list is full.");
        return false;
    }

    if (mPtrNum < pos)
    {
        SEAD_ASSERT_MSG(false, "pos[%d] exceed size[%d]", pos, mPtrNum);
        return false;
    }

    return true;
}

void PtrArrayImpl::sort(CompareCallbackImpl cmp)
{
    s32 top;
    s32 bottom;
    s32 lastSwap;
    void* x;
    void** ptrs = mPtrs;

    if (mPtrNum < 2)
    {
        return;
    }

    top = 0;
    bottom = mPtrNum - 1;

    while (true)
    {
        lastSwap = top;
        for (s32 i = top; i < bottom; i++)
        {
            if (cmp(ptrs[i], ptrs[i + 1]) > 0)
            {
                x = ptrs[i + 1];
                ptrs[i + 1] = ptrs[i];
                ptrs[i] = x;
                lastSwap = i;
            }
        }

        bottom = lastSwap;
        if (top == bottom)
        {
            break;
        }

        lastSwap = bottom;
        for (s32 i = bottom; i > top; i--)
        {
            if (cmp(ptrs[i], ptrs[i - 1]) < 0)
            {
                x = ptrs[i - 1];
                ptrs[i - 1] = ptrs[i];
                ptrs[i] = x;
                lastSwap = i;
            }
        }

        top = lastSwap;
        if (top == bottom)
        {
            break;
        }
    }
}

void PtrArrayImpl::heapSort(CompareCallbackImpl cmp)
{
    s32 i;
    s32 k;
    s32 t;
    s32 n;
    void* x;
    void** ptrs;

    n = mPtrNum;
    ptrs = mPtrs;
    for (i = n / 2; i > 0; i--)
    {
        x = ptrs[i - 1];

        k = i;
        while (t = k * 2, t <= n)
        {
            if (t < n && cmp(ptrs[t - 1], ptrs[t]) < 0)
            {
                t++;
            }

            if (cmp(x, ptrs[t - 1]) >= 0)
            {
                break;
            }

            ptrs[k - 1] = ptrs[t - 1];
            k = t;
        }

        ptrs[k - 1] = x;
    }

    while (n > 1)
    {
        x = ptrs[n - 1];
        ptrs[n - 1] = ptrs[0];

        n--;
        k = 1;
        while (t = k * 2, t <= n)
        {
            if (t < n && cmp(ptrs[t - 1], ptrs[t]) < 0)
            {
                t++;
            }

            if (cmp(x, ptrs[t - 1]) >= 0)
            {
                break;
            }

            ptrs[k - 1] = ptrs[t - 1];
            k = t;
        }

        ptrs[k - 1] = x;
    }
}

s32 PtrArrayImpl::compare(const PtrArrayImpl& o, CompareCallbackImpl cmp) const
{
    s32 result;
    for (s32 i = 0; i < mPtrNum; i++)
    {
        if (i >= o.mPtrNum)
        {
            return 1;
        }

        result = cmp(mPtrs[i], o.mPtrs[i]);
        if (result != 0)
        {
            return result;
        }
    }

    return mPtrNum == o.mPtrNum ? 0 : -1;
}

void PtrArrayImpl::uniq(CompareCallbackImpl cmp)
{
    for (s32 i = 0; i < mPtrNum - 1; i++)
    {
        s32 k = i + 1;
        while (k < mPtrNum)
        {
            if (cmp(mPtrs[i], mPtrs[k]) == 0)
            {
                erase(k);
            }
            else
            {
                k++;
            }
        }
    }
}

s32 PtrArrayImpl::binarySearch(const void* ptr, CompareCallbackImpl cmp) const
{
    if (mPtrNum == 0)
    {
        return -1;
    }

    s32 left;
    s32 right;
    s32 mid;
    s32 cmpRet;

    left = 0;
    right = mPtrNum - 1;
    while (left < right)
    {
        mid = (left + right) / 2;

        cmpRet = cmp(mPtrs[mid], ptr);
        if (cmpRet == 0)
        {
            return mid;
        }

        if (cmpRet < 0)
        {
            left = mid + 1;
        }
        else
        {
            right = mid;
        }
    }

    if (cmp(mPtrs[left], ptr) == 0)
    {
        return left;
    }

    return -1;
}

}  // namespace sead

