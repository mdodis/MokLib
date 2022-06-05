#pragma once
#include "Base.h"
#include "Str.h"
#include "Memory/Base.h"
#include "Types.h"

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

template <FmtPolicy::Type Policy>
static _inline void fmt(Tape *tape, const TFmtStr<Policy> &type) {
    type._format(tape);
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
    ParseTape pt(tape);
    TArray<char> array(result.alloc);

    char c = pt.read_char();

    if (c != '\"') {
        goto PARSE_FAIL;
    }
    c = pt.read_char();

    while ((c != '\"') && c != EOF) {

        if (c == '\\') {
            char escape = pt.read_char();

            switch (escape) {
                case '\\': array.add('\\'); break;
                case '\"': array.add('\"'); break;
                case 't':  array.add('\t');  break;
                case 'r':  array.add('\r');  break;
                case 'f':  array.add('\f');  break;
                case 'n':  array.add('\n');  break;
                case 'b':  array.add('\b');  break;

                default: {
                    array.add(c);
                    array.add(escape);
                } break;
            }

        } else {
            array.add(c);
        }

        c = pt.read_char();
    }

    if (c == EOF) {
        goto PARSE_FAIL;
    }

    *result.str = Str((char*)array.data, array.size);
    return true;

PARSE_FAIL:
    pt.restore();
    return false;
}
