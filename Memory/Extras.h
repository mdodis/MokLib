#pragma once
#include <new>
#include <utility>

#include "Base.h"
#include "RawBuffer.h"

#define PUSH_STRUCT(alloc, type) (type*)alloc->reserve(sizeof(type))

template <typename T, typename... Args>
T* alloc(Allocator& allocator, Args&&... args)
{
    umm ptr = (umm)allocator.reserve(sizeof(T));
    if (!ptr) return 0;

    T* result = new (ptr) T(std::forward<Args>(args)...);
    return result;
}

template <typename T>
T* alloc_array(Allocator& allocator, size_t count)
{
    umm ptr = allocator.reserve(sizeof(T) * count);
    return (T*)ptr;
}

static _inline Raw alloc_raw(Allocator& allocator, size_t size)
{
    void* ptr = allocator.reserve(size);
    if (!ptr) return Raw(0, 0);

    return Raw(ptr, size);
}

template <typename T>
void dealloc(Allocator& allocator, T* object)
{
    object->~object();
    allocator.release(object);
}

template <typename T>
struct TSharedRef {
    T*   ptr;
    u64  references = 0;
    bool is_owner   = true;

    TSharedRef(TSharedRef& other)
    {
        ptr              = other.ptr;
        references       = other.references;
        is_owner         = true;
        other.is_owner   = false;
        other.references = 0;
    }

    template <typename SubType>
    TSharedRef<T>& operator=(TSharedRef<SubType>& sub_type)
    {
        TSharedRef<T> new_ref(dynamic_cast<T*>(sub_type.ptr));
        *this = TSharedRef(new_ref);
        return *this;
    }

    TSharedRef() : ptr(0), is_owner(false) {}
    TSharedRef(T* ptr) : ptr(ptr) {}
    TSharedRef(T& ref) : ptr(&ref) {}
    TSharedRef(T&& ref) : ptr(&ref) {}

    T* operator->() { return ptr; }

    T& operator*() { return *ptr; }
};

template <typename T>
static _inline TSharedRef<T> to_shared_ref(T* ptr)
{
    return TSharedRef<T>(ptr);
}