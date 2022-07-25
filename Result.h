#pragma once
#include "Variant.h"
#include "Debugging/Backtrace.h"
#include "Debugging/DebugOutput.h"
#include "Debugging/Assertions.h"
#include "FileSystem/Extras.h"
#include "Traits.h"

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
        if (!ok()) {

            if constexpr (HasFmt<ErrKind>::value) {
                debug_print(LIT("Result unwrap() failed: $\n"), err());
            } else {
                debug_print(LIT("Result unwrap() failed\n"));
            }
            print_backtrace(get_debug_tape());
            DEBUG_BREAK();
        }
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