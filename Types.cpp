#include "Types.h"
#include "Parsing.h"
#include "Defer.h"

PROC_PARSE_IMPL(Str) {
    // @todo: At some point, for some reason I don't quite see now, we'll need
    // to support escape characters, but for now, we'll resort to a simple
    // substring of "This Part" that excludes the quotes
    IAllocator *alloc = get_system_allocator();
    AllocTape output(*alloc);

    char c = tape->read_char();

    if (c != '\'') {
        tape->move(-1);
        alloc->release(output.ptr);
        return false;
    }

    c = tape->read_char();

    while (c != '\'') {
        output.write(&c, 1);
        c = tape->read_char();

        if ((c == EOF) || is_newline(c)) {
            tape->move(-1);
            alloc->release(output.ptr);
            tape->move(-i64(output.wr_offset));
            return false;
        }
    }

    result = Str((char*)output.ptr, output.wr_offset);
    return true;
}

PROC_FMT_IMPL(u64) {
    constexpr u64 max_digits = 20ull;
    thread_local char digits[max_digits + 1];
    u32 i = max_digits;

    if (type == 0) {
        tape->write_str(LIT("0"));
        return;
    }

    u64 num = type;

    while (num != 0) {
        u8 digit = num % 10;

        digits[i] = '0' + digit;

        num = num / 10;
        --i;
    }

    tape->write_str(Str(digits + i + 1, max_digits - i));
}


PROC_PARSE_IMPL(u64) {
    result = 0;

    char c = tape->read_char();
    DEFER(tape->move(-1););

    if (!is_digit(c)) {
        return false;
    }

    do {
        result *= 10;
        result += c - '0';
        c = tape->read_char();
    } while(is_digit(c));

    return true;
}

PROC_FMT_IMPL(i64) {
    i64 num = type;

    if (num < 0) {
        tape->write_str("-");
        num = -num;
    }

    u64 unum = (u64)num;
    fmt<u64>(tape, unum);
}

PROC_PARSE_IMPL(i64) {
    char c = tape->read_char();

    bool parsed_symbol = false;
    bool negate = false;

    switch (c) {
        case '+':
        case '-': {
            negate = c == '-';
            parsed_symbol = true;
        } break;

        default: {} break;
    }

    u64 result_u64;
    if (!parse<u64>(tape, result_u64)) {
        if (parsed_symbol)
            tape->move(-1);
        return false;
    }

    result = ((i64)result_u64) * (negate ? -1 : +1);
    return true;
}

PROC_FMT_IMPL(f64) {
    constexpr u32 max_digits = 16;
    thread_local char buffer[_CVTBUFSIZE];

    _gcvt_s(buffer, _CVTBUFSIZE, type, max_digits);

    Str s(buffer);

    tape->write_str(s);
}

PROC_PARSE_IMPL(f64) {
    i32 back_amount = 0u;
    bool decimal_point = false;
    char c = tape->read_char();

    char buffer[32];
    i32 i = 0;

    switch (c) {
        case '+':
        case '-': {
            buffer[i++] = c;
            c = tape->read_char();
            back_amount++;
        } break;

        default: {} break;
    }

    DEFER(tape->move(-1));

    if (!(is_digit(c) || c == '.')) {
        tape->move(-back_amount);
        return false;
    }

    do {

        if (c == '.') {
            if (decimal_point) {
                tape->move(-back_amount);
                tape->move(-i);
                return false;
            } else {
                decimal_point = true;
            }
        }

        buffer[i++] = c;
        c = tape->read_char();
    } while (is_digit(c) || (c == '.'));

    result = strtof(buffer, 0);
    return true;
}