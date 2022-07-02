#pragma once
#include "Base.h"
#include "Str.h"
#include "Tape.h"
#include "Types.h"
#include "Containers/Map.h"
#include "Memory/Extras.h"
#include "Containers/Slice.h"

struct IArg {
    Str id;

    virtual void *ptr() = 0;
    virtual void parse(Tape *tape) = 0;
};

template <typename T>
struct TArg : IArg {
    T current;

    virtual void *ptr() override {
        return &current;
    }

    virtual void parse(Tape *tape) override {

    }
};

struct ArgCollection {

    ArgCollection(IAllocator &allocator = System_Allocator)
        : args(allocator)
        , allocator(allocator)
        {}

    TMap<Str, IArg*> args;
    IAllocator &allocator;

    template <typename T>
    bool register_arg(Str identifier, T default_value) {

        if (args.contains(identifier))
            return false;

        TArg<T> *arg = alloc<TArg<T>>(allocator);
        arg->current = default_value;

        args.add(identifier, arg);

        return true;
    }

    template <typename T>
    T &get_arg(Str identifier) {
        return *(T*)args[identifier]->ptr();
    }

    bool parse_args(const Slice<Str> &args);
};
