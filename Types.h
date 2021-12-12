#pragma once
#include "Base.h"
#include "Memory/Base.h"
#include "Str.h"
#include "Containers/List.h"
#include <string.h>
#include <stdio.h>

#define PROC_STRINGIFY(name) void name(umm ptr, TList<Str> &output, IAllocator &allocator)
#define PROC_DESTRINGIFY(name) int32 name(umm ptr, Str input)
typedef PROC_STRINGIFY(ProcStringify);
typedef PROC_DESTRINGIFY(ProcDestringify);

struct IType {
	const char *name;
	ProcStringify *stringify;
	ProcDestringify *destringify;
};


// uint32
PROC_STRINGIFY(stringify_uint32);
PROC_DESTRINGIFY(destringify_uint32);
static constexpr _inline IType type_of(const uint32 &i) {
	return {
		"uint32",
		stringify_uint32,
		destringify_uint32
	};
}


// int32
PROC_STRINGIFY(stringify_int32);
static constexpr _inline IType type_of(const int32& i) {
	return {
		"int32",
		stringify_int32
	};
}

// float
PROC_STRINGIFY(stringify_float);
PROC_DESTRINGIFY(destringify_float);
static constexpr _inline IType type_of(const float &f) {
	return {
		"float",
		stringify_float,
		destringify_float
	};
}

// Str
static PROC_STRINGIFY(stringify_str);
static PROC_DESTRINGIFY(destringify_str);
static constexpr _inline IType type_of(const Str& s) {
	return {
		"Str",
		stringify_str,
		destringify_str
	};
}

static _inline PROC_STRINGIFY(stringify_str) {
	output.append(*((Str*)ptr));
}

static _inline PROC_DESTRINGIFY(destringify_str) {
	// @todo: At some point, for some reason I don't quite see now, we'll need
	// to support escape characters, but for now, we'll resort to a simple
	// substring of "This Part" that excludes the quotes

	if (input[0] != '\'')
		return 0;

	int32 start_quote = 0;
	int32 end_quote = input.first_of('\'', start_quote + 1);

	if ((start_quote == -1) || (end_quote == -1))
		return 0;

	if ((start_quote + 1) == end_quote) {
		*((Str*)ptr) = Str(0, 0);
		return 2;
	}

	*((Str*)ptr) = Str(&input[start_quote + 1], end_quote - start_quote - 1);
	return end_quote + 1;
}

// CStr
static PROC_STRINGIFY(stringify_cstr);
static PROC_STRINGIFY(stringify_cstr) {
	const char *cstr = (char*)ptr;
	output.append(Str(cstr));
}

static constexpr _inline IType type_of(const char * s) {
	return {
		"CStr",
		stringify_cstr,
		// We can't really destringify cstrings, since that would require
		// us to write a null terminator somewhere, so this is forbidden.
		0
	};
}

static constexpr _inline IType type_of(char * s) {
	return type_of((const char*)s);
}
