#pragma once
#include "Variant.h"
#include "Debugging/Assertions.h"

template <typename T>
struct Ok {
    explicit constexpr Ok(T value) : value(std::move(value)) {}

    constexpr T&& take() {
        return std::move(value);
    }

    T value;
};

template <typename T>
struct Err {
    explicit constexpr Err(T value) : value(std::move(value)) {}

    constexpr T&& take() {
        return std::move(value);
    }

    T value;
};

template <typename RetType, typename ErrKind>
struct Result {

    using OkType  = Ok<RetType>;
    using ErrType = Err<ErrKind>;
    using VarType = TVariant<OkType, ErrType>;

    constexpr Result(OkType value)
        : var(std::move(value))
        {}

    constexpr Result(ErrType value)
        : var(std::move(value))
        {}

    constexpr bool ok() {
        return var.template is<OkType>();
    }

    constexpr RetType &&unwrap() {
        ASSERT(ok());
        return var.template get<OkType>()->take();
    }

    constexpr RetType &&value() {
        ASSERT(ok());
        return var.template get<OkType>()->take();
    }

    constexpr ErrKind &&err() {
        ASSERT(!ok());
        return var.template get<ErrType>()->take();
    }

    VarType var;
};