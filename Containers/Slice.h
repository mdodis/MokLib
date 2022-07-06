#pragma once
#include "Base.h"
#include "Array.h"
#include "Debugging/Assertions.h"

template <typename T>
struct Slice {
    T *ptr;
    u32 count;

    constexpr Slice()
        : ptr(0)
        , count(0)
        {}

    constexpr Slice(T *ptr, u32 count)
        : ptr(ptr)
        , count(count)
        {}

    constexpr Slice(TArray<T> &array, u32 start = 0)
        : ptr(array.data + start)
        , count(array.size - start)
        {}

    T &operator[](i32 index) const {
        ASSERT((index >= 0) && (index < count));
        return ptr[index];
    }

    T *last() const {
        if (count > 0)
            return ((T*)ptr) + (count - 1);
        else
            return 0;
    }

    SliceIterator<T> begin() const { return SliceIterator<T>(ptr, 0); }
    SliceIterator<T> end()   const { return SliceIterator<T>(ptr, count); }

    SliceIterator<T, false> rbegin() const { return SliceIterator<T, false>(ptr, 0); }
    SliceIterator<T, false> rend()   const { return SliceIterator<T, false>(ptr, count); }
};

template <typename T>
Slice<T> slice(TArray<T> &arr, u32 start) {
    return Slice<T>(arr, start);
}

template <typename T, u32 Count>
Slice<T> slice(TArr<T, Count> &arr, u32 start = 0) {
    umm start_ptr = ((umm)arr.elements);
    start_ptr += sizeof(T) * start;
    u32 count = Count - start;
    return Slice<T>((T*)start_ptr, count);
}