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

template <typename T>
void fmt(Tape *tape, const T &type);

template <typename T>
bool parse(Tape *tape, T &result);

/** Declare an external template specialization for fmt */
#define PROC_FMT(T)     			    				\
    template <>         			    				\
    extern MOKLIB_API void fmt(Tape *tape, const T &type)
/** Define an inline template specialization for fmt */
#define PROC_FMT_IMPL(T)     							\
    template <>         								\
    void fmt(Tape *tape, const T &type)
/** Define an inline template specialization for fmt */
#define PROC_FMT_INL(T)     			    			\
    template <>         			    				\
    static _inline void fmt(Tape *tape, const T &type)

/** Declare an external template specialization for parse */
#define PROC_PARSE(T) 				 					\
    template <>						 					\
    extern MOKLIB_API bool parse(Tape *tape, T &result)
/** Define an inline template specialization for parse */
#define PROC_PARSE_IMPL(T) 				 				\
    template <>						 					\
    bool parse(Tape *tape, T &result)
/** Define an inline template specialization for parse */
#define PROC_PARSE_INL(T) 				 				\
    template <>						 					\
    static _inline bool parse(Tape *tape, T &result)

template <typename From, typename To>
bool _pass_parse(Tape *tape, To &to) {
	From from;
	if (!parse<From>(tape, from)) {
		return false;
	}

	to = (To)from;
	return true;
}

PROC_FMT_INL(CStr) { tape->write_str(Str(type)); }
PROC_FMT_INL(Str)  { tape->write_str(type); }
PROC_FMT(u64);
PROC_FMT(i64);
PROC_FMT(f64);
PROC_PARSE(Str);
PROC_PARSE(u64);
PROC_PARSE(i64);
PROC_PARSE(f64);

PROC_FMT_INL(u32) 	{ fmt<u64>(tape, (u64)type); }
PROC_PARSE_INL(u32) { return _pass_parse<u64, u32>(tape, result); }
PROC_FMT_INL(i32)	{ fmt<i64>(tape, (i64)type); }
PROC_PARSE_INL(i32)	{ return _pass_parse<i64, i32>(tape, result); }
PROC_FMT_INL(f32)	{ fmt<f64>(tape, (f64)type); }
PROC_PARSE_INL(f32)	{ return _pass_parse<f64, f32>(tape, result); }