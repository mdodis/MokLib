#pragma once
#include "Base.h"
#include "Config.h"
#include "Memory/Base.h"
#include "Memory/RawBuffer.h"
#include "Traits.h"

/**
 * An immutable utf8 string, that allocates NO memory, and thus doesn't
 * own it.
 */
struct MOKLIB_API Str {
    Str() : data(0), len(0) {}
    Str(const char* cstr);
    constexpr Str(void* str, u64 len, bool with_null_term = false)
        : data((char*)str), len(len), has_null_term(with_null_term)
    {}
    constexpr Str(const char* data, u64 len, bool with_null_term = false)
        : Str((void*)data, len, with_null_term)
    {}
    constexpr Str(const Raw& raw) : Str(raw.buffer, raw.size) {}

    /**
     * Splits the string in question at @at , resulting into left and right
     * strings
     *
     * @param   at      where to split the string
     * @param   left    Start of the string up to and including at
     * @param   right   Remainder of left
     *
     * @return true if the split operation was successful
     */
    bool split(u64 at, Str* left, Str* right) const;

    /**
     * Chops the string at the character, returning the right part
     * @see Str::split
     */
    _inline Str chop_left(u64 at) const
    {
        if (at < 0) {
            return Str::NullStr;
        }

        Str left, right;
        if (split(at, &left, &right)) {
            return right;
        } else {
            return Str::NullStr;
        }
    }

    /**
     * Chops the string at the character, returning the left part
     * @see Str::split
     */
    _inline Str chop_right(u64 at) const
    {
        if (at < 0) {
            return Str::NullStr;
        }

        Str left, right;
        if (split(at, &left, &right)) {
            return left;
        } else {
            return Str::NullStr;
        }
    }

    /**
     * Chops the string between left and right
     */
    _inline Str chop_middle(u64 left, u64 right) const
    {
        return Str(data + left, right - left);
    }

    /**
     * Finds the last instance of the specified character
     */
    u64 last_of(char c) const;

    /**
     * Finds the first instance of the specified character, starting
     * from @start
     */
    u64 first_of(char c, u64 start = 0) const;

    /**
     * Finds the last occurrence of the specified string, starting
     * from @start
     */
    u64 last_of(const Str& s, u64 start = Str::StartEnd) const;

    /**
     * Find the first occurence of the specified string, starting
     * from @start
     */
    u64 first_of(const Str& s, u64 start = 0) const;

    _inline u64 count_of(char c, u64 start = 0) const
    {
        u64 result = 0;
        for (u64 i = start; i < len; ++i) {
            if (data[i] == c) result++;
        }
        return result;
    }

    /** Shorthand for chop_left(last_of(c) - 1) */
    _inline Str chop_left_last_of(char c) const
    {
        return chop_left(last_of(c) - 1);
    }

    /** Shorthand for chop_left(first_of(c) - 1) */
    _inline Str chop_left_first_of(char c) const
    {
        return chop_left(first_of(c) - 1);
    }

    _inline Str chop_right_last_of(const Str& s) const
    {
        return chop_right(last_of(s) - 1);
    }

    /** True when the string starts with the specified needle */
    bool starts_with(const Str& needle) const;

    /** True when the string ends with the specified needle */
    bool ends_with(const Str& needle) const;

    /** Converts letter characters to upper-case in-place. */
    Str& to_upper();

    _inline Str part(u64 begin, u64 end) const
    {
        return Str(data + begin, end - begin);
    }

    Str clone(Allocator& alloc) const;

    _inline char& operator[](u64 index)
    {
        if (index < len) {
            return *((char*)(data) + index);
        } else {
            return Str::Null;
        }
    }

    _inline const char& operator[](u64 index) const
    {
        if (index < len) {
            return *((const char*)(data) + index);
        } else {
            return Str::Null;
        }
    }

    _inline operator Raw() const { return Raw{data, len}; }

    char* data;
    u64   len;
    bool  has_null_term = false;

    static constexpr u64 StartEnd = NumProps<u64>::max;
    static char          Null;
    static const Str     NullStr;
};

MOKLIB_API bool     operator==(const Str& left, const Str& right);
static _inline bool operator!=(const Str& left, const Str& right)
{
    return !(left == right);
}

/** This is a hack since we can't tell string literals and cstrings apart */
#define LIT(s) Str((s), ARRAY_COUNT(s) - 1, true)
