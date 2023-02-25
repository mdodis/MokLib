#pragma once
#include <cstddef>
#include <initializer_list>
#include <iterator>

#include "../Base.h"
#include "../Config.h"
#include "../Debugging/Assertions.h"
#include "../Memory/Base.h"
#include "ArrayIterator.h"
#include "Memory/Arena.h"
#include "Traits.h"

template <typename T>
struct TArray {
    using FwdIter = SliceIterator<T>;
    using RevIter = SliceIterator<T, false>;

    T          *data;
    IAllocator *alloc;
    u64         capacity, size;

    static constexpr u64 Init_Capacity = 4;

    _inline TArray(void)
    {
        this->data     = 0;
        this->capacity = 0;
        this->size     = 0;
    }

    _inline TArray(IAllocator *alloc)
    {
        this->alloc    = alloc;
        this->data     = 0;
        this->capacity = 0;
        this->size     = 0;
    }

    _inline TArray(std::initializer_list<T> init_list)
        : TArray(get_system_allocator(), init_list)
    {}

    _inline TArray(IAllocator *alloc, std::initializer_list<T> init_list)
        : TArray(alloc)
    {
        for (const T &elem : init_list) {
            add(elem);
        }
    }

    u64 add(const T &item)
    {
        if (!data) {
            if (!init(Init_Capacity)) return -1;
        }

        if (capacity == (size)) {
            if (!stretch()) return -1;
        }

        data[size++] = item;
        return size - 1;
    }

    void add_range(T *elems, u64 num_elems)
    {
        for (u64 i = 0; i < num_elems; ++i) {
            add(elems[i]);
        }
    }

    T *add()
    {
        if (!data) {
            if (!init(Init_Capacity)) return 0;
        }

        if (capacity == (size)) {
            if (!stretch()) return 0;
        }

        size++;

        return &data[size - 1];
    }

    void del(u64 index, bool keep_order = false)
    {
        if (!is_index_valid(index)) return;

        if (keep_order) {
            UNIMPLEMENTED();
        } else {
            data[index] = data[size - 1];
            size--;
        }
    }

    void empty() { size = 0; }

    void release()
    {
        empty();
        alloc->release((umm)data);
    }

    bool init(u64 amount)
    {
        data = (T *)alloc->reserve(amount * sizeof(T));
        if (!data) {
            return false;
        }

        capacity = amount;
        size     = 0;
        return true;
    }

    void init_range(u64 amount)
    {
        init(amount);
        size = amount;
    }

    bool stretch()
    {
        T *new_data = (T *)alloc->resize(
            (umm)data,
            capacity * sizeof(T),
            capacity * 2 * sizeof(T));
        if (!new_data) return false;
        data = new_data;
        capacity *= 2;
        return true;
    }

    T *last() const
    {
        if (size > 0)
            return ((T *)data) + (size - 1);
        else
            return 0;
    }

    _inline bool is_index_valid(u64 i) const { return (i >= 0) && (i < size); }

    _inline T &operator[](u64 idx)
    {
        ASSERT(is_index_valid(idx));
        return data[idx];
    }

    _inline const T &operator[](u64 idx) const
    {
        ASSERT(is_index_valid(idx));
        return data[idx];
    }

    u64 index_of(const T &query) const
    {
        for (u64 i = 0; i < size; ++i) {
            if (query == data[i]) {
                return i;
            }
        }

        return NumProps<u64>::max;
    }

    FwdIter begin() const { return FwdIter(data, 0); }
    FwdIter end() const { return FwdIter(data, size); }
    RevIter rbegin() const { return RevIter(data, size - 1); }
    RevIter rend() const { return RevIter(data, -1); }
};

template <typename T, uint32 Count>
struct TInlineArray : TArray<T> {
    TInlineArray()
        : alloc_arena(memory, (sizeof(T) * Count)), TArray<T>(&alloc_arena)
    {
        this->init(Count);
    }

    TInlineArray(std::initializer_list<T> init_list) : TInlineArray()
    {
        for (const T &elem : init_list) {
            add(elem);
        }
    }

    u8    memory[(sizeof(T) * Count)];
    Arena alloc_arena;
};

template <typename T, u32 Count>
struct TArr {
    using FwdIter = SliceIterator<T>;

    T elements[Count];

    constexpr TArr(const std::initializer_list<T> init_list)
    {
        u32 c = 0;
        for (const T &elem : init_list) {
            elements[c++] = elem;
        }
    }

    T &operator[](i32 index)
    {
        ASSERT((index >= 0) && (index < Count));
        return elements[index];
    }

    operator T *() { return elements; }

    constexpr u32 count() const { return Count; }

    FwdIter begin() const { return FwdIter((T *)elements, 0); }
    FwdIter end() const { return FwdIter((T *)elements, Count); }
};

// constexpr TArr<Type, sizeof...(Rest)> arr(Rest&&... t) {
template <typename Type, typename... Rest>
constexpr auto arr(Rest &&...t) -> TArr<Type, sizeof...(Rest)>
{
    constexpr auto c      = sizeof...(Rest);
    TArr<Type, c>  result = {t...};
    return result;
}