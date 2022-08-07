#pragma once
#include "Base.h"
#include "Str.h"
#include "Memory/Base.h"
#include "Types.h"
#include "Tape.h"

static _inline void format(Tape *tape,
    Str fmt_str) {
    tape->write_str(fmt_str);
}

template <typename First, typename... Rest>
static _inline void format(Tape *tape, Str fmt_str, const First &first, const Rest&... rest) {
    u64 idx = 0;

    while (idx < fmt_str.len) {
        if (fmt_str[idx] == '$') {
            fmt(tape, first);
            break;
        }

        tape->write((void*)(fmt_str.data + idx), 1);
        idx++;
    }

    format(tape, fmt_str.chop_left(idx), rest...);
}

template <typename First, typename... Rest>
static _inline Str format(
    IAllocator &alloc,
    Str fmt_str,
    const First &first,
    const Rest&... rest)
{
    MeasureTape measurement;
    format(&measurement, fmt_str, first, rest...);

    const u64 size = measurement.num_written;
    umm ptr = alloc.reserve(size);
    if (!ptr)
        return Str::NullStr;

    RawTape output(Raw{ptr, size});
    format(&output, fmt_str, first, rest...);

    return Str((CStr)ptr, size, fmt_str[fmt_str.len - 1] == '\0');
}

template <typename T>
struct TFmtHex {
    const T &value;
    TFmtHex(const T &value)
        : value(value)
        {}

    void _format(Tape *tape) const {
        umm v = (umm)&value;
        u32 num_bytes = sizeof(T);

        const char *prefix = "0x";
        tape->write(prefix, 2);


        for (int i = num_bytes - 1; i >= 0; --i) {
            char s[2];
            hex_of_byte(v[i], s[0], s[1]);
            tape->write(s, 2);
        }
    }
};

namespace FmtPolicy {
    enum : u32 {
        WithoutQuotes   = 0x00000000,
        WithQuotes      = 0x00000001,
    };

    enum : u32 {
        Quotes = 0x00000001,
    };

    typedef u32 Type;
}

template <FmtPolicy::Type Policy>
struct TFmtStr {
    Str *s;
    TFmtStr(Str &str)
        : s(&str)
        {}
    void _format(Tape *tape) const {

        if constexpr (Policy & FmtPolicy::Quotes) {
            tape->write_char('\"');
        }

        Str &str = *s;
        for (u32 i = 0; i < str.len; ++i) {

            switch (str[i]) {

                case '\\': tape->write_str(LIT(R"(\\)")); break;
                case '\"': tape->write_str(LIT(R"(\")")); break;
                case '\t': tape->write_str(LIT(R"(\t)")); break;
                case '\r': tape->write_str(LIT(R"(\r)")); break;
                case '\f': tape->write_str(LIT(R"(\f)")); break;
                case '\n': tape->write_str(LIT(R"(\n)")); break;
                case '\b': tape->write_str(LIT(R"(\b)")); break;

                default: {
                    tape->write_char(str[i]);
                } break;
            }
        }

        if constexpr (Policy & FmtPolicy::Quotes) {
            tape->write_char('\"');
        }
    }
};

template <typename T>
static _inline void fmt(Tape *tape, const TFmtHex<T> &type) {
    type._format(tape);
}

template <FmtPolicy::Type Policy>
static _inline void fmt(Tape *tape, const TFmtStr<Policy> &type) {
    type._format(tape);
}

static _inline Str null_terminate(const Str &str, IAllocator &alloc = System_Allocator) {
    if (!str.has_null_term) {
        return format(alloc, LIT("$\0"), str);
    }
    return str;
}

static _inline Str make_folder_path(const Str &str, IAllocator &alloc = System_Allocator) {
    bool gen_slash = str[str.len - 1] != '/';

    return format(
        alloc,
        LIT("$$\0"),
        str,
        gen_slash ? LIT("/") : Str::NullStr);
}

/**
 * Format string with escaped characters
 */
struct StrFormatter {
    Str *str;
    IAllocator *alloc;
    StrFormatter(Str &str, IAllocator *alloc = 0)
        : str(&str)
        , alloc(alloc)
        {}
};

PROC_FMT_INL(StrFormatter) {
    Str &str = *type.str;
    tape->write_char('\"');
    for (u32 i = 0; i < str.len; ++i) {

        switch (str[i]) {

            case '\\': tape->write_str(LIT(R"(\\)")); break;
            case '\"': tape->write_str(LIT(R"(\")")); break;
            case '\t': tape->write_str(LIT(R"(\t)")); break;
            case '\r': tape->write_str(LIT(R"(\r)")); break;
            case '\f': tape->write_str(LIT(R"(\f)")); break;
            case '\n': tape->write_str(LIT(R"(\n)")); break;
            case '\b': tape->write_str(LIT(R"(\b)")); break;

            default: {
                tape->write_char(str[i]);
            } break;
        }
    }
    tape->write_char('\"');
}

PROC_PARSE_INL(StrFormatter) {
    return parse_escaped_string(tape, *result.str, allocator);
}
