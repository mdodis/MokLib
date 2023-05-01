#include "Types.h"

#include <stdio.h>

#include "Defer.h"
#include "Memory/AllocTape.h"
#include "Parsing.h"

bool parse_quoted_string(ReadTape* tape, Str& result, Allocator& allocator)
{
    // @todo: At some point, for some reason I don't quite see now, we'll need
    // to support escape characters, but for now, we'll resort to a simple
    // substring of "This Part" that excludes the quotes
    AllocTape output(allocator);

    char c = tape->read_char();

    if (c != '\'') {
        tape->seek(-1);
        allocator.release(output.ptr);
        return false;
    }

    c = tape->read_char();

    while (c != '\'') {
        output.write(&c, 1);
        c = tape->read_char();

        if ((c == EOF) || is_newline(c)) {
            tape->seek(-1);
            allocator.release(output.ptr);
            tape->seek(-i64(output.wr_offset));
            return false;
        }
    }

    result = Str((char*)output.ptr, output.wr_offset);
    return true;
}

bool parse_str(ReadTape* tape, Str& result, Allocator& allocator)
{
    char c = tape->read_char();
    if (c == '\'') {
        tape->seek(-1);
        return parse_quoted_string(tape, result, allocator);
    } else if (!is_valid_cid(c)) {
        tape->seek(-1);
        return false;
    }
    AllocTape output(allocator);

    do {
        output.write(&c, 1);
        c = tape->read_char();
    } while (is_valid_cid(c));

    if (c != EOF) tape->seek(-1);

    result = Str((char*)output.ptr, output.wr_offset);
    return true;
}

void format_u64(WriteTape* tape, const u64& type)
{
    constexpr u64     max_digits = 20ull;
    thread_local char digits[max_digits + 1];
    u32               i = max_digits;

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

bool parse_u64(ReadTape* tape, u64& result)
{
    result = 0;

    char c = tape->read_char();
    DEFER(tape->seek(-1););

    if (!is_digit(c)) {
        return false;
    }

    do {
        result *= 10;
        result += c - '0';
        c = tape->read_char();
    } while (is_digit(c));

    return true;
}

void format_i64(WriteTape* tape, const i64& type)
{
    i64 num = type;

    if (num < 0) {
        tape->write_str("-");
        num = -num;
    }

    u64 unum = (u64)num;
    fmt<u64>(tape, unum);
}

bool parse_i64(ReadTape* tape, i64& result)
{
    char c = tape->read_char();

    bool parsed_symbol = false;
    bool negate        = false;

    switch (c) {
        case '+':
        case '-': {
            negate        = c == '-';
            parsed_symbol = true;
        } break;

        default: {
            tape->seek(-1);
        } break;
    }

    u64 result_u64;
    if (!parse<u64>(tape, result_u64)) {
        if (parsed_symbol) tape->seek(-1);
        return false;
    }

    result = ((i64)result_u64) * (negate ? -1 : +1);
    return true;
}

void format_f64(WriteTape* tape, const f64& type)
{
    constexpr u32     buffer_size = 16;
    thread_local char buffer[buffer_size];

    snprintf(buffer, buffer_size, "%f", type);

    Str s(buffer);

    tape->write_str(s);
}

bool parse_f64(ReadTape* tape, f64& result)
{
    i32  back_amount   = 0u;
    bool decimal_point = false;
    char c             = tape->read_char();

    char buffer[32];
    i32  i = 0;

    switch (c) {
        case '+':
        case '-': {
            buffer[i++] = c;
            c           = tape->read_char();
            back_amount++;
        } break;

        default: {
        } break;
    }

    DEFER(tape->seek(-1));

    if (!(is_digit(c) || c == '.')) {
        tape->seek(-back_amount);
        return false;
    }

    do {
        if (c == '.') {
            if (decimal_point) {
                tape->seek(-back_amount);
                tape->seek(-i);
                return false;
            } else {
                decimal_point = true;
            }
        }

        buffer[i++] = c;
        c           = tape->read_char();
    } while (is_digit(c) || (c == '.'));

    result = strtof(buffer, 0);
    return true;
}