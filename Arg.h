#pragma once
#include "Base.h"
#include "Containers/Map.h"
#include "Containers/Slice.h"
#include "FileSystem/Extras.h"
#include "Memory/Extras.h"
#include "Reflection.h"
#include "Str.h"
#include "Tape.h"
#include "Types.h"

struct IArg {
    Str           id;
    Str           description;
    Str           type;
    virtual void* ptr()                                       = 0;
    virtual bool  parse(ReadTape* tape, Allocator& allocator) = 0;
};

template <typename T>
struct TArg : IArg {
    T current;

    virtual void* ptr() override { return &current; }

    virtual bool parse(ReadTape* tape, Allocator& allocator) override
    {
        return ::parse<T>(tape, current, allocator);
    }
};

/**
 * This specialization is made in favor of reducing allocations
 * as well as (and most importantly), reading the whole string properly
 * including the spaces that are inside of the tape
 */
template <>
struct TArg<Str> : IArg {
    Str current;

    virtual void* ptr() override { return &current; }

    virtual bool parse(ReadTape* tape, Allocator& allocator) override
    {
        char c     = 0;
        u64  count = 0;
        while (tape->read(&c, 1) == 1) {
            count++;
        }

        if (!tape->seek(-((i64)count))) return false;

        char* data = (char*)allocator.reserve(count);
        if (!data) return false;

        if (tape->read(data, count) != count) return false;

        current = Str(data, count);
        return true;
    }
};

template <>
struct TArg<bool> : IArg {
    bool current;

    virtual void* ptr() override { return &current; }

    virtual bool parse(ReadTape* tape, Allocator& allocator) override
    {
        return ::parse(tape, current, allocator);
    }
};

struct MOKLIB_API ArgCollection {
    ArgCollection(Allocator& allocator = System_Allocator)
        : allocator(allocator), args(&allocator)
    {}

    TArray<IArg*> args;
    Allocator&    allocator;

    template <typename T>
    bool register_arg(
        Str identifier, T default_value, Str description = Str::NullStr)
    {
        TArg<T>* arg     = alloc<TArg<T>>(allocator);
        arg->current     = default_value;
        arg->id          = identifier;
        arg->description = description;
        arg->type        = descriptor_of((T*)0)->type_name();
        args.add(arg);

        return true;
    }

    template <typename T>
    T* get_arg(Str identifier)
    {
        for (IArg* arg : args) {
            if (arg->id == identifier) {
                return (T*)arg->ptr();
            }
        }

        return 0;
    }

    bool parse_args(const Slice<Str>& params);

    void summary();
};