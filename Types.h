#pragma once
#include <stdio.h>
#include <string.h>

#include "Base.h"
#include "Config.h"
#include "Memory/Base.h"
#include "Parsing.h"
#include "Str.h"
#include "Tape.h"
#include "Traits.h"

template <typename T>
static _inline void fmt(WriteTape* tape, const T& type);

template <typename T>
static _inline bool parse(
    ReadTape* tape, T& result, Allocator& allocator = System_Allocator);

/** Declare an external template specialization for fmt */
#define PROC_FMT(T) \
    template <>     \
    MOKLIB_API void fmt(WriteTape* tape, const T& type)
/** Define an external template specialization for fmt */
#define PROC_FMT_IMPL(T) \
    template <>          \
    MOKLIB_API void fmt(WriteTape* tape, const T& type)
/** Define an inline template specialization for fmt */
#define PROC_FMT_INL(T)                     \
    template <>                             \
    struct HasFmt<T> {                      \
        static constexpr bool value = true; \
    };                                      \
    template <>                             \
    void fmt(WriteTape* tape, const T& type)

/** Declare an external template specialization for parse */
#define PROC_PARSE(T) \
    template <>       \
    MOKLIB_API bool parse(ReadTape* tape, T& result, Allocator& allocator)
/** Define an inline template specialization for parse */
#define PROC_PARSE_IMPL(T) \
    template <>            \
    MOKLIB_API bool parse(ReadTape* tape, T& result, Allocator& allocator)
/** Define an inline template specialization for parse */
#define PROC_PARSE_INL(T) \
    template <>           \
    bool parse(ReadTape* tape, T& result, Allocator& allocator)

template <typename From, typename To>
bool _pass_parse(ReadTape* tape, To& to, Allocator& allocator)
{
    From from;
    if (!parse<From>(tape, from)) {
        return false;
    }

    to = (To)from;
    return true;
}

MOKLIB_API void format_u64(WriteTape* tape, const u64& type);
MOKLIB_API void format_i64(WriteTape* tape, const i64& type);
MOKLIB_API void format_f64(WriteTape* tape, const f64& type);
MOKLIB_API bool parse_u64(ReadTape* tape, u64& result);
MOKLIB_API bool parse_i64(ReadTape* tape, i64& result);
MOKLIB_API bool parse_f64(ReadTape* tape, f64& result);
MOKLIB_API bool parse_str(ReadTape* tape, Str& result, Allocator& allocator);

PROC_FMT_INL(CStr) { tape->write_str(Str(type)); }
PROC_FMT_INL(ConstCStr) { tape->write_str(Str(type)); }

template <size_t N>
static _inline void fmt(WriteTape* tape, const char (&ar)[N])
{
    tape->write_str(Str(ar, N, true));
}

PROC_FMT_INL(Str)
{
    if (type[type.len - 1] == 0)
        tape->write_str(type.chop_right(type.len - 2));
    else
        tape->write_str(type);
}
PROC_FMT_INL(u64) { format_u64(tape, type); }
PROC_FMT_INL(i64) { format_i64(tape, type); }
PROC_FMT_INL(f64) { format_f64(tape, type); }
PROC_FMT_INL(bool) { tape->write_str(type ? LIT("true") : LIT("false")); }
PROC_FMT_INL(char) { tape->write((void*)&type, 1); }

PROC_PARSE_INL(Str) { return parse_str(tape, result, allocator); }
PROC_PARSE_INL(u64) { return parse_u64(tape, result); }
PROC_PARSE_INL(i64) { return parse_i64(tape, result); }
PROC_PARSE_INL(f64) { return parse_f64(tape, result); }

PROC_FMT_INL(u32) { fmt<u64>(tape, (u64)type); }
PROC_PARSE_INL(u32) { return _pass_parse<u64, u32>(tape, result, allocator); }
PROC_FMT_INL(i32) { fmt<i64>(tape, (i64)type); }
PROC_PARSE_INL(i32) { return _pass_parse<i64, i32>(tape, result, allocator); }

PROC_FMT_INL(u16) { fmt<u64>(tape, (u64)type); }
PROC_PARSE_INL(u16) { return _pass_parse<u64, u16>(tape, result, allocator); }
PROC_FMT_INL(i16) { fmt<i64>(tape, (i64)type); }
PROC_PARSE_INL(i16) { return _pass_parse<i64, i16>(tape, result, allocator); }

PROC_FMT_INL(u8) { fmt<u64>(tape, (u64)type); }
PROC_PARSE_INL(u8) { return _pass_parse<u64, u8>(tape, result, allocator); }
PROC_FMT_INL(i8) { fmt<i64>(tape, (i64)type); }
PROC_PARSE_INL(i8) { return _pass_parse<i64, i8>(tape, result, allocator); }

PROC_FMT_INL(f32) { fmt<f64>(tape, (f64)type); }
PROC_PARSE_INL(f32) { return _pass_parse<f64, f32>(tape, result, allocator); }
PROC_PARSE_INL(bool)
{
    Str bstring = parse_string(tape, allocator, is_letter);
    if (bstring == LIT("true")) {
        result = true;
    } else if (bstring == LIT("false")) {
        result = false;
    } else {
        return false;
    }
    return true;
}

#define PROC_FMT_ENUM(enum_type, values)                   \
    template <>                                            \
    struct HasFmt<enum_type::Type> {                       \
        static constexpr bool value = true;                \
    };                                                     \
    template <>                                            \
    void fmt(WriteTape* tape, const enum_type::Type& type) \
    {                                                      \
        switch (type)                                      \
            values                                         \
    }

#define FMT_ENUM_CASE(enum_type, sub) \
    case enum_type::sub:              \
        tape->write_str(LIT(#sub));   \
        break

#define FMT_ENUM_CASE2(enum_type, sub, str) \
    case enum_type::sub:                    \
        tape->write_str(LIT(str));          \
        break

#define FMT_ENUM_DEFAULT_CASE(sub)  \
    default:                        \
        tape->write_str(LIT(#sub)); \
        break

#define FMT_ENUM_DEFAULT_CASE_UNREACHABLE() \
    default:                                \
        break

#define PROC_PARSE_ENUM(enum_type, values)                                  \
    template <>                                                             \
    bool parse(ReadTape* tape, enum_type::Type& type, Allocator& allocator) \
    {                                                                       \
        Str s = parse_string(tape, allocator, is_valid_cid);                \
        if (s == Str::NullStr) return false;                                \
        values return false;                                                \
    }

#define PARSE_ENUM_CASE(enum_type, sub) \
    {                                   \
        Str cmp = LIT(#sub);            \
        if (cmp == s) {                 \
            type = enum_type::sub;      \
            return true;                \
        }                               \
    }

#define PARSE_ENUM_CASE2(enum_type, sub, str) \
    {                                         \
        Str cmp = LIT(str);                   \
        if (cmp == s) {                       \
            type = enum_type::sub;            \
            return true;                      \
        }                                     \
    }