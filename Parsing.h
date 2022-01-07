#pragma once
#include "Config.h"
#include "Base.h"
#include "Containers/Array.h"
#include "Memory/Base.h"
#include "Str.h"
#include "Tape.h"
#include <stdlib.h>

/**
 * Character classes
 */

#define PROC_CHAR_IS_CLASS(name) bool name(char c)
typedef PROC_CHAR_IS_CLASS(ProcCharClassIs);

static _inline PROC_CHAR_IS_CLASS(is_whitespace) {
    return (c == ' ')
        || (c == '\t')
        || (c == '\n')
        || (c == '\r');
}

static _inline PROC_CHAR_IS_CLASS(isnt_whitespace) {
    return !is_whitespace(c);
}

static _inline PROC_CHAR_IS_CLASS(is_newline) {
    return (c == '\n')
        || (c == '\r');
}

static _inline PROC_CHAR_IS_CLASS(is_space) {
    return (c == ' ')
        || (c == '\t');
}

static _inline PROC_CHAR_IS_CLASS(is_digit) {
    return (c >= '0' && c <= '9');
}

static _inline PROC_CHAR_IS_CLASS(is_uppercase_letter) {
    return (c >= 'A' && c <= 'Z');
}

static _inline PROC_CHAR_IS_CLASS(is_lowercase_letter) {
    return (c >= 'a' && c <= 'z');
}

static _inline PROC_CHAR_IS_CLASS(is_letter) {
    return is_uppercase_letter(c)
        || is_lowercase_letter(c);
}

static _inline PROC_CHAR_IS_CLASS(is_alphanumeric) {
    return is_letter(c)
        || is_digit(c);
}

static _inline PROC_CHAR_IS_CLASS(is_math_op) {
    return (c == '~')
        || (c == '+')
        || (c == '-')
        || (c == '*')
        || (c == '/')
        || (c == '>')
        || (c == '<')
        || (c == '=')
        || (c == '!')
        || (c == '^');
}

static _inline PROC_CHAR_IS_CLASS(is_valid_cid_begin) {
    return (c >= 'A' && c <= 'Z')
        || (c >= 'a' && c <= 'z')
        || (c == '_');
}

static _inline PROC_CHAR_IS_CLASS(is_alpha_or_math) {
    return is_alphanumeric(c)
        || is_math_op(c);
}

static _inline PROC_CHAR_IS_CLASS(is_valid_cid) {
    return is_valid_cid_begin(c)
        || (c >= '0' && c <= '9');
}

/**
 * Per-character conversion functions
 */

static _inline char uppercase_of(char c) {
    return is_lowercase_letter(c)
        ? c - ('a' - 'A')
        : c;
}

/**
 * Parsing functions
 */

static u64 eat_whitespace(const Str &s, u64 i) {
    while (i < s.len && is_whitespace(s[i])) {
        i++;
    }
    return i;
}

static u64 eat_line(const Str &s, u64 i = 0) {
    while (i < s.len && !is_newline(s[i])) {
        i++;
    }

    return i + 1;
}

static u32 match_strings(Str to, Str *strings, u32 num_strings) {
    for (u32 i = 0; i < num_strings; ++i) {
        if (to == strings[i]) {
            return i;
        }
    }

    return num_strings;
}


MOKLIB_API u64 parse_cid(const Str &s, u64 i, Str &out);

/**
 * Tape parsing
 */
static void eat_whitespace(struct Tape *tape) {
    char c = tape->read_char();
    while ((c != EOF) && (is_whitespace(c))) {
        c = tape->read_char();
    }

    if (c != EOF)
        tape->move(-1);
}

static void eat_line(struct Tape *tape) {
    char c = tape->read_char();

    while ((c != EOF) && (!is_newline(c))) {
        c = tape->read_char();
    }

    if (c != EOF)
        tape->move(-1);
}

static void eat_space(struct Tape *tape) {
    char c = tape->read_char();
    while ((c != EOF) && (is_space(c))) {
        c = tape->read_char();
    }

    if (c != EOF)
        tape->move(-1);
}

static bool match_string(struct Tape *tape, const Str &string) {
    char c;
    int32 i = 0;

    if (string.len == 0) {
        return false;
    }

    while ((c = tape->read_char()) != EOF) {
        if (string[i++] != c) {
            break;
        }

        if (i == string.len)
            return true;
    }

    tape->move(-i);
    return false;
}

static u32 match_strings(struct Tape *tape, Str *strings, u32 num_strings) {
    for (u32 i = 0; i < num_strings; ++i) {
        if (match_string(tape, strings[i])) {
            return i;
        }
    }

    return num_strings;
}

MOKLIB_API Str parse_string(
    struct Tape *tape,
    IAllocator &alloc,
    ProcCharClassIs *predicate = isnt_whitespace);

static bool parse_num(struct Tape *tape, float &result) {
    char c;
    char buf[16];
    i32 i = 0;

    while ((c = tape->read_char()) != EOF) {

        if (is_digit(c) || (c == '.') || (c == '-')) {
            if (i < 15) {
                buf[i++] = c;
            }
        } else {
            break;
        }
    }

    tape->move(-1);

    if (i == 0) {
        return false;
    }

    buf[i] = 0;

    result = strtof(buf, 0);
    return true;
}

static bool parse_num(struct Tape *tape, u32 &result) {
    char c;
    char buf[16];
    i32 i = 0;

    while ((c = tape->read_char()) != EOF) {

        if (is_digit(c)) {
            if (i < 15) {
                buf[i++] = c;
            }
        } else {
            break;
        }
    }


    if (c != EOF)
        tape->move(-1);

    if (i == 0) {
        return false;
    }

    buf[i] = 0;
    result = strtol(buf, 0, 10);

    return true;
}

MOKLIB_API Str parse_cid(struct Tape *tape, IAllocator &alloc);