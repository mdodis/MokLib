#pragma once
#include <utility>

#include "Containers/Slice.h"

namespace sort {

    template <typename T>
    static _inline i64 quicksort_partition(Slice<T>& slice, i64 low, i64 high)
    {
        T   pivot = slice[high];
        i64 i     = low - 1;

        for (i64 j = low; j < high; ++j) {
            if (slice[j] < pivot) {
                i++;
                std::swap(slice[i], slice[j]);
            }
        }

        std::swap(slice[i + 1], slice[high]);
        return i + 1;
    }

    template <typename T>
    static _inline void quicksort_impl(Slice<T>& slice, i64 low, i64 high)
    {
        if (low < high) {
            i64 pivot = quicksort_partition(slice, low, high);
            quicksort_impl(slice, low, pivot - 1);
            quicksort_impl(slice, pivot + 1, high);
        }
    }

    template <typename T>
    static _inline void quicksort(Slice<T>& slice)
    {
        quicksort_impl(slice, 0, i64(slice.count) - 1);
    }

}  // namespace sort
