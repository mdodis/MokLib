#pragma once
#include "Base.h"
#include "Memory/Base.h"
#include "Str.h"
#include "Tape.h"
#include "Types.h"

static _inline void format(WriteTape* tape, Str fmt_str)
{
    tape->write_str(fmt_str);
}

template <typename First, typename... Rest>
static _inline void format(
    WriteTape* tape, Str fmt_str, const First& first, const Rest&... rest)
{
    u64 idx = 0;

    while (idx < fmt_str.len) {
        if (fmt_str[idx] == '{') {
            if (((idx + 1) < fmt_str.len) && (fmt_str[idx + 1] == '}')) {
                fmt(tape, first);
                idx += 1;
                break;
            }
        }

        tape->write((void*)(fmt_str.data + idx), 1);
        idx++;
    }

    format(tape, fmt_str.chop_left(idx), rest...);
}

static _inline Str format(Allocator& allocator, Str fmt_str)
{
    const u64 size = fmt_str.len;
    umm       ptr  = (umm)allocator.reserve(size);
    if (!ptr) return Str::NullStr;

    RawWriteTape output(Raw{ptr, size});
    format(&output, fmt_str);

    return Str((CStr)ptr, size, fmt_str[fmt_str.len - 1] == '\0');
}

template <typename First, typename... Rest>
static _inline Str format(
    Allocator& alloc, Str fmt_str, const First& first, const Rest&... rest)
{
    MeasureWriteTape measurement;
    format(&measurement, fmt_str, first, rest...);

    const u64 size = measurement.num_written;
    void*     ptr  = alloc.reserve(size);
    if (!ptr) return Str::NullStr;

    RawWriteTape output(Raw{ptr, size});
    format(&output, fmt_str, first, rest...);

    return Str((CStr)ptr, size, fmt_str[fmt_str.len - 1] == '\0');
}

template <typename First, typename... Rest>
static _inline char* format_cstr(
    Allocator& alloc, Str fmt_str, const First& first, const Rest&... rest)
{
    MeasureWriteTape measurement;
    format(&measurement, fmt_str, first, rest...);

    const u64 size = measurement.num_written;
    void*     ptr  = alloc.reserve(size + 1);
    if (!ptr) return 0;

    RawWriteTape output(Raw{ptr, size});
    format(&output, fmt_str, first, rest...);

    char* data = (char*)ptr;

    data[size] = 0;
    return (char*)data;
}

template <typename T>
struct TFmtHex {
    const T& value;
    TFmtHex(const T& value) : value(value) {}

    void _format(WriteTape* tape) const
    {
        umm v         = (umm)&value;
        u32 num_bytes = sizeof(T);

        const char* prefix = "0x";
        tape->write((void*)prefix, 2);

        for (int i = num_bytes - 1; i >= 0; --i) {
            char s[2];
            hex_of_byte(v[i], s[0], s[1]);
            tape->write(s, 2);
        }
    }
};

namespace FmtPolicy {
    enum : u32
    {
        WithoutQuotes = 0x00000000,
        WithQuotes    = 0x00000001,
    };

    enum : u32
    {
        Quotes = 0x00000001,
    };

    typedef u32 Type;
}  // namespace FmtPolicy

template <FmtPolicy::Type Policy>
struct TFmtStr {
    Str* s;
    TFmtStr(Str& str) : s(&str) {}
    void _format(WriteTape* tape) const
    {
        if constexpr (Policy & FmtPolicy::Quotes) {
            tape->write_char('\"');
        }

        Str& str = *s;
        for (u32 i = 0; i < str.len; ++i) {
            switch (str[i]) {
                case '\\':
                    tape->write_str(LIT(R"(\\)"));
                    break;
                case '\"':
                    tape->write_str(LIT(R"(\")"));
                    break;
                case '\t':
                    tape->write_str(LIT(R"(\t)"));
                    break;
                case '\r':
                    tape->write_str(LIT(R"(\r)"));
                    break;
                case '\f':
                    tape->write_str(LIT(R"(\f)"));
                    break;
                case '\n':
                    tape->write_str(LIT(R"(\n)"));
                    break;
                case '\b':
                    tape->write_str(LIT(R"(\b)"));
                    break;
                // @fixme: We shouldn't be able to get to this point
                // so, if this hits, then it means we're losing some null
                // terminators on other functions
                case '\0':
                    break;
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

struct FmtPath {
    Str in;
    FmtPath(Str in) : in(in) {}
};

PROC_FMT_IMPL(FmtPath)
{
    u64 i = 0;

    auto is_sep = [](char c) { return (c == '\\') || (c == '/'); };

    while (i < type.in.len) {
        if (is_sep(type.in[i])) {
            tape->write_char('/');

            while ((i < type.in.len) && is_sep(type.in[i])) {
                i++;
            }
        } else {
            tape->write_char(type.in[i]);
            i++;
        }
    }
}

template <typename T>
static _inline void fmt(WriteTape* tape, const TFmtHex<T>& type)
{
    type._format(tape);
}

template <FmtPolicy::Type Policy>
static _inline void fmt(WriteTape* tape, const TFmtStr<Policy>& type)
{
    type._format(tape);
}

static _inline Str null_terminate(
    const Str& str, Allocator& alloc = System_Allocator)
{
    if (!str.has_null_term) {
        return format(alloc, LIT("{}\0"), str);
    }
    return str;
}

static _inline Str make_folder_path(
    const Str& str, Allocator& alloc = System_Allocator)
{
    bool gen_slash = str[str.len - 1] != '/';

    return format(
        alloc,
        LIT("{}{}\0"),
        str,
        gen_slash ? LIT("/") : Str::NullStr);
}

/**
 * Format string with escaped characters
 */
struct StrFormatter {
    Str*       str;
    Allocator* alloc;
    StrFormatter(Str& str, Allocator* alloc = 0) : str(&str), alloc(alloc) {}
};

PROC_FMT_INL(StrFormatter)
{
    Str& str = *type.str;
    tape->write_char('\"');
    for (u32 i = 0; i < str.len; ++i) {
        switch (str[i]) {
            case '\\':
                tape->write_str(LIT(R"(\\)"));
                break;
            case '\"':
                tape->write_str(LIT(R"(\")"));
                break;
            case '\t':
                tape->write_str(LIT(R"(\t)"));
                break;
            case '\r':
                tape->write_str(LIT(R"(\r)"));
                break;
            case '\f':
                tape->write_str(LIT(R"(\f)"));
                break;
            case '\n':
                tape->write_str(LIT(R"(\n)"));
                break;
            case '\b':
                tape->write_str(LIT(R"(\b)"));
                break;

            default: {
                tape->write_char(str[i]);
            } break;
        }
    }
    tape->write_char('\"');
}

PROC_PARSE_INL(StrFormatter)
{
    return parse_escaped_string(tape, *result.str, allocator);
}
