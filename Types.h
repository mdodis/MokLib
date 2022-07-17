#pragma once
#include "Config.h"
#include "Base.h"
#include "Memory/Base.h"
#include "Str.h"
#include "Containers/List.h"
#include <string.h>
#include <stdio.h>
#include "Tape.h"
#include "Memory/AllocTape.h"
#include "Parsing.h"

template <typename T>
static _inline void fmt(Tape *tape, const T &type);

template <typename T>
static _inline bool parse(
    Tape *tape,
    T &result,
    IAllocator &allocator = System_Allocator);

/** Declare an external template specialization for fmt */
#define PROC_FMT(T)     			    				\
    template <>         			    				\
    MOKLIB_API void fmt(Tape *tape, const T &type)
/** Define an inline template specialization for fmt */
#define PROC_FMT_IMPL(T)     							\
    template <>         								\
    MOKLIB_API void fmt(Tape *tape, const T &type)
/** Define an inline template specialization for fmt */
#define PROC_FMT_INL(T)     			    			\
    template <>         			    				\
    void fmt(Tape *tape, const T &type)

/** Declare an external template specialization for parse */
#define PROC_PARSE(T) 				 					                \
    template <>						 					                \
    MOKLIB_API bool parse(Tape *tape, T &result, IAllocator &allocator)
/** Define an inline template specialization for parse */
#define PROC_PARSE_IMPL(T) 				 				                \
    template <>						 					                \
    MOKLIB_API bool parse(Tape *tape, T &result, IAllocator &allocator)
/** Define an inline template specialization for parse */
#define PROC_PARSE_INL(T) 				 				     \
    template <>						 					     \
    bool parse(Tape *tape, T &result, IAllocator &allocator)

template <typename From, typename To>
bool _pass_parse(Tape *tape, To &to, IAllocator &allocator) {
	From from;
	if (!parse<From>(tape, from)) {
		return false;
	}

	to = (To)from;
	return true;
}

MOKLIB_API void format_u64(Tape *tape, const u64 &type);
MOKLIB_API void format_i64(Tape *tape, const i64 &type);
MOKLIB_API void format_f64(Tape *tape, const f64 &type);
MOKLIB_API bool parse_u64(Tape *tape, u64 &result);
MOKLIB_API bool parse_i64(Tape *tape, i64 &result);
MOKLIB_API bool parse_f64(Tape *tape, f64 &result);
MOKLIB_API bool parse_str(Tape *tape, Str &result, IAllocator &allocator);

PROC_FMT_INL(CStr) { tape->write_str(Str(type)); }
PROC_FMT_INL(Str)  {
    if (type[type.len - 1] == 0)
        tape->write_str(type.chop_right(type.len - 2));
    else
        tape->write_str(type);
}
PROC_FMT_INL(u64)  { format_u64(tape, type); }
PROC_FMT_INL(i64)  { format_i64(tape, type); }
PROC_FMT_INL(f64)  { format_f64(tape, type); }
PROC_FMT_INL(bool) { tape->write_str(type ? LIT("true") : LIT("false")); }
PROC_FMT_INL(char) { tape->write(&type, 1); }

PROC_PARSE_INL(Str) {return parse_str(tape, result, allocator);}
PROC_PARSE_INL(u64) {return parse_u64(tape, result);}
PROC_PARSE_INL(i64) {return parse_i64(tape, result);}
PROC_PARSE_INL(f64) {return parse_f64(tape, result);}

PROC_FMT_INL(u32) 	{ fmt<u64>(tape, (u64)type); }
PROC_PARSE_INL(u32) { return _pass_parse<u64, u32>(tape, result, allocator); }
PROC_FMT_INL(i32)	{ fmt<i64>(tape, (i64)type); }
PROC_PARSE_INL(i32)	{ return _pass_parse<i64, i32>(tape, result, allocator); }
PROC_FMT_INL(f32)	{ fmt<f64>(tape, (f64)type); }
PROC_PARSE_INL(f32)	{ return _pass_parse<f64, f32>(tape, result, allocator); }
PROC_PARSE_INL(bool) {
    Str bstring = parse_string_no_quotes(tape, allocator);
    if (bstring == LIT("true")) {
        result = true;
    } else if (bstring == LIT("false")) {
        result = false;
    } else {
        return false;
    }
    return true;
}