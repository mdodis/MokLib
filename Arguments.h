#pragma once
#include "Config.h"
#include "Types.h"
#include "Containers/Slice.h"

struct MOKLIB_API IArgument {
    Str name;
    virtual bool parse_argument(Tape *tape) = 0;
};

template <typename T>
struct TArgument : public IArgument {
    T data;

    constexpr TArgument(Str name, T default_value) {
        this->name = name;
        this->data = default_value;
    }

    bool parse_argument(Tape *tape) override {
        return parse<T>(tape, data);
    }
};

/**
 * Parse argument array from a plain text string
 *
 * @todo: make this support a List<Str> since we're expecting argc, argv style
 * options in other operating systems
 */
MOKLIB_API void parse_arguments(const Str &s, IArgument **args, int32 num_args);

static _inline void parse_arguments(
    const Slice<Str> &strings,
    IArgument **args,
    u32 num_args)
{
    for (const Str &s : strings) {
        parse_arguments(s, args, (i32)num_args);
    }
}

// MOKLIB_API void parse_args(const Slice<Str> &argv, Slice<IArgument*> args);