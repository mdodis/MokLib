#pragma once
#include <utility>
#include <functional>
#include "Containers/Slice.h"
#include "Containers/Array.h"
#include "Delegates.h"

namespace sort {

    template <typename T>
    using CompareFunc = Delegate<bool, const T&, const T&>;

    template <typename T>
    static _inline i64 quicksort_partition(
        Slice<T>& slice, i64 low, i64 high, CompareFunc<T> predicate)
    {
        T   pivot = slice[high];
        i64 i     = low - 1;

        for (i64 j = low; j < high; ++j) {
            if (predicate.call(slice[j], pivot)) {
                i++;
                std::swap(slice[i], slice[j]);
            }
        }

        std::swap(slice[i + 1], slice[high]);
        return i + 1;
    }

    template <typename T>
    static _inline void quicksort_impl(Slice<T>& slice, i64 low, i64 high, CompareFunc<T> predicate)
    {
        if (low < high) {
            i64 pivot = quicksort_partition(slice, low, high, predicate);
            quicksort_impl(slice, low, pivot - 1, predicate);
            quicksort_impl(slice, pivot + 1, high, predicate);
        }
    }

    template <typename T>
    static _inline bool operator_compare(const T& left, const T& right)
    {
        return left < right;
    }

    template <typename T>
    static _inline void quicksort(
        Slice<T>& slice,
        CompareFunc<T> predicate = CompareFunc<T>::create_static(operator_compare<T>))
    {
        quicksort_impl(slice, 0, i64(slice.count) - 1, predicate);
    }

}  // namespace sort
