#pragma once
#include "Base.h"
#include "Memory/Extras.h"
#include "Slice.h"

template <typename T>
Slice<T> alloc_slice(Allocator& allocator, size_t count)
{
    T* ptr = alloc_array<T>(allocator, count);
    return slice(ptr, count);
}