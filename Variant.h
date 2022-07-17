#pragma once
#include "Base.h"
#include <typeinfo>
#include <utility>

template <typename Type, typename... Types>
struct VariantSize {
    static const u64 size = sizeof(Type) > VariantSize<Types...>::size
        ? sizeof(Type)
        : VariantSize<Types...>::size;
};

template <typename T>
struct VariantSize<T> {
    static const u64 size = sizeof(T);
};

template <typename... Types>
struct TVariant {
    size_t type_id;
    u8 data[VariantSize<Types...>::size];

    TVariant() :type_id(invalid()) {}

    template <typename T>
    TVariant(T &&other) {
        set(other);
    }

    template <typename T>
    T &operator=(T&& other) {
        set(other);
        return *(T*)data;
    }

    template <typename T, typename... Args>
    void set(Args&&... args) {
        new (data) T(std::forward(args)...);
        type_id = typeid(T).hash_code();
    }

    template <typename T>
    void set(T &other) {
        T *self = (T*)data;
        *self = other;
        type_id = typeid(T).hash_code();
    }

    template <typename T, typename... Args>
    T *get() const {
        if (type_id == typeid(T).hash_code()) {
            return (T*)data;
        } else {
            return 0;
        }
    }

    template <typename T, typename... Args>
    bool is() const {
        return type_id == typeid(T).hash_code();
    }

    static inline size_t invalid() {
        return typeid(void).hash_code();
    }
};