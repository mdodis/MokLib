#pragma once
#include "Base.h"
#include "Traits.h"

enum class IOError : u32 {
    None = 0,
    /** The specified file could not be found */
    FileNotFound,
    /** Only Shows in load_dll. The Module's dependencies could not be resolved */
    UnresolvedModuleDependencies,
    /** Access not allowed */
    AccessDenied,
    Unrecognized = NumProps<u32>::max,
};

template<typename T>
struct TEnum
{
    T e;
    TEnum(const T& e) :e(e) {}
    _inline operator bool() const {
        return e != (T)0;
    }
};

template <typename T>
struct TErr : TEnum<T> {
    Str extra;

    TErr(const T &e, Str extra = Str::NullStr)
        : TEnum<T>(e)
        , extra(extra)
        {}
};

template <typename T>
static _inline TErr<T> ok() {

}

static _inline Str format_error(const IOError &err) {
    switch (err) {
        case IOError::None:                         return LIT("None");
        case IOError::FileNotFound:                 return LIT("File not found");
        case IOError::UnresolvedModuleDependencies: return LIT("Module dependencies could not be resolved");
        default:                                    return LIT("Unrecognized");
    }
}

template <typename T>
static _inline Str format_error(const TEnum<T>& err) {
    return format_error(err.e);
}