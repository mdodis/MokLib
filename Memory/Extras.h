#pragma once
#include "Base.h"
#include <utility>
#include <new>

#define PUSH_STRUCT(alloc, type) (type*)alloc->reserve(sizeof(type))

template <typename T, typename... Args>
T *alloc(IAllocator &allocator, Args&&... args) {
    umm ptr = allocator.reserve(sizeof(T));
    if (!ptr)
        return 0;

    T *result = new (ptr) T(std::forward<Args>(args)...);
    return result;
}

template <typename T>
void dealloc(IAllocator &allocator, T *object) {
    object->~object();
    allocator.release(object);
}

template <typename T>
struct TSharedRef {
    T *ptr;
    u64 references = 0;
    bool is_owner = true;

    TSharedRef(TSharedRef &other) {
        ptr = other.ptr;
        references = other.references;
        is_owner = true;
        other.is_owner = false;
        other.references = 0;
    }

    template <typename SubType>
    TSharedRef<T> &operator=(TSharedRef<SubType> &sub_type) {
        TSharedRef<T> new_ref(dynamic_cast<T*>(sub_type.ptr));
        *this = TSharedRef(new_ref);
        return *this;
    }

    TSharedRef() : ptr(0), is_owner(false) {}
    TSharedRef(T *ptr) : ptr(ptr) {}
    TSharedRef(T &ref) : ptr(&ref) {}
    TSharedRef(T &&ref) : ptr(&ref) {}

    T *operator->() {
        return ptr;
    }

    T &operator*() {
        return *ptr;
    }
};

template <typename T>
static _inline TSharedRef<T> to_shared_ref(T *ptr) {
    return TSharedRef<T>(ptr);
}