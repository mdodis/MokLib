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

    return Str((CStr)ptr, size);
}