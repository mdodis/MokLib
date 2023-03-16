#pragma once
#include "../Base.h"
#include "../Config.h"
#include "../Str.h"
#include "../StringFormat.h"

MOKLIB_API struct Tape* get_debug_tape();
MOKLIB_API void         set_debug_tape(struct Tape* tape);

static _inline void debug_print(Str fmt_str)
{
    get_debug_tape()->write_str(fmt_str);
}

template <typename First, typename... Rest>
static _inline void debug_print(
    Str fmt_str, const First& first, const Rest&... rest)
{
    format(get_debug_tape(), fmt_str, first, rest...);
}
