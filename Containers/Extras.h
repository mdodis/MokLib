#pragma once
#include "Base.h"
#include "Memory/Extras.h"
#include "Slice.h"

template <typename T, u64 Count>
Slice<T> alloc_slice(IAllocator& allocator)
{
    T* ptr = alloc_array<T, Count>(allocator);
    return slice(ptr, Count);
}