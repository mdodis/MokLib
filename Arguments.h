#pragma once

#include "Types.h"

struct IArgument {
    Str name;
    IType type;
    virtual umm get_data_ptr() = 0;
};

template <typename T>
struct TArgument : public IArgument {

    constexpr TArgument(Str name, T default_value) {
        this->name = name;
        this->type = type_of(default_value);
        this->data = default_value;
    }

    virtual umm get_data_ptr() override  { return (umm)&data; }
    T data;
};

/**
 * Parse argument array from a plain text string
 *
 * @todo: make this support a List<Str> since we're expecting argc, argv style
 * options in other operating systems
 */
void parse_arguments(const Str &s, IArgument **args, int32 num_args);