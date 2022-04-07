#pragma once
#include "Base.h"

template <typename T>
struct Slice {
    T *ptr;
    u32 count;

    constexpr Slice(T *ptr, u32 count)
        : ptr(ptr)
        , count(count)
        {}

    constexpr Slice(TArray<T> &array)
        : ptr(array.data)
        , count(array.size)
        {}

    T &operator[](i32 index) {
        ASSERT((index >= 0) && (index < count));
        return ptr[index];
    }

    SliceIterator<T> begin() const { return SliceIterator<T>(ptr, 0); }
    SliceIterator<T> end()   const { return SliceIterator<T>(ptr, count); }

    SliceIterator<T, false> rbegin() const { return SliceIterator<T, false>(ptr, 0); }
    SliceIterator<T, false> rend()   const { return SliceIterator<T, false>(ptr, count); }
};