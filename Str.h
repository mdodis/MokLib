#pragma once

#include "Base.h"
#include "Memory/Base.h"
#include "Debugging.h"

#ifndef MOK_STR_RANGE_CHECK
    #define MOK_STR_RANGE_CHECK 1
#endif

/**
 * An immutable utf8 string, that allocates NO memory, and thus doesn't
 * own it. See @StringBuilder to find out how to use it for creating strings
 *
 * @todo: Actual utf8 support (especially for .len)
 */
struct Str {
    Str();
    Str(const char *cstr);
    constexpr Str(const char *str, int32 len)
        : data((const uint8*)str), len(len) {}

    /**
     * Splits the string in question at @at , resulting into left and right strings
     *
     * @param   at      where to split the string
     * @param   left    Start of the string up to and including at
     * @param   right   Remainder of left
     *
     * @return true if the split operation was successful
     */
    bool split(int32 at, Str *left, Str *right) const;

    /**
     * Chops the string at the character, returning the left part
     * @see Str::split
     */
    _inline Str chop_left(int32 at) const {
        Str left, right;
        if (split(at, &left, &right)) {
            return right;
        } else {
            return Str();
        }
    }

    /**
     * Chops the string at the character, returning the right part
     * @see Str::split
     */
    _inline Str chop_right(int32 at) {
        Str left, right;
        if (split(at, &left, &right)) {
            return left;
        } else {
            return Str();
        }
    }

    /**
     * Finds the last instance of the specified character
     */
    int32 last_of(char c);

    /**
     * Finds the first instance of the specified character, starting
     * from @start
     */
    int32 first_of(char c, int32 start = 0);


    _inline char &operator[](int index) {
#if MOK_STR_RANGE_CHECK
        if (index < len) {
            return *((char*)(data) + index);
        } else {
            ASSERT(false);
            return Str::Null;
        }
#else
        return data[len];
#endif
    }

    _inline const char &operator[](int index) const {
#if MOK_STR_RANGE_CHECK
        if (index < len) {
            return *((const char*)(data)+index);
        } else {
            ASSERT(false);
            return Str::Null;
        }
#else
        return data[len];
#endif
    }


    const uint8 *data;
    int32 len;

    static const Str New_Line;
    static char Null;
    static const Str NullStr;
};

bool operator==(const Str &left, const Str &right);

#define STATIC_STR(s) Str((s), ARRAY_COUNT(s) - 1)