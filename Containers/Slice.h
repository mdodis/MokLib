#pragma once
#include "Array.h"
#include "Base.h"
#include "Debugging/Assertions.h"
#include "Memory/RawBuffer.h"

template <typename T>
struct Slice {
    T*  ptr;
    u64 count;

    constexpr Slice() : ptr(0), count(0) {}

    constexpr Slice(T* ptr, u64 count) : ptr(ptr), count(count) {}

    constexpr Slice(TArray<T>& array, u64 start = 0)
        : ptr(array.data + start), count(array.size - start)
    {}

    T& operator[](u64 index) const
    {
        ASSERT((index >= 0) && (index < count));
        return ptr[index];
    }

    T* last() const
    {
        if (count > 0)
            return ((T*)ptr) + (count - 1);
        else
            return 0;
    }

    size_t size() const { return sizeof(T) * count; }

    constexpr operator Raw() { return Raw(ptr, count * sizeof(T)); }

    SliceIterator<T> begin() const { return SliceIterator<T>(ptr, 0); }
    SliceIterator<T> end() const { return SliceIterator<T>(ptr, (i32)count); }

    SliceIterator<T, false> rbegin() const
    {
        return SliceIterator<T, false>(ptr, 0);
    }
    SliceIterator<T, false> rend() const
    {
        return SliceIterator<T, false>(ptr, count);
    }
};

template <typename T>
Slice<T> slice(TArray<T>& arr, u32 start = 0)
{
    return Slice<T>(arr, start);
}

template <typename T, u32 Count>
Slice<T> slice(TArr<T, Count>& arr, u32 start = 0)
{
    umm start_ptr = ((umm)arr.elements);
    start_ptr += sizeof(T) * start;
    u32 count = Count - start;
    return Slice<T>((T*)start_ptr, count);
}

template <typename T>
Slice<T> slice(T* ptr, u64 count)
{
    return Slice(ptr, count);
}

static _inline Slice<u8> slice(const Raw& raw)
{
    return Slice<u8>((u8*)raw.buffer, raw.size);
}