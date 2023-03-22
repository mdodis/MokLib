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
    virtual void* ptr()                                    = 0;
    virtual bool  parse(Tape* tape, IAllocator& allocator) = 0;
};

template <typename T>
struct TArg : IArg {
    T current;

    virtual void* ptr() override { return &current; }

    virtual bool parse(Tape* tape, IAllocator& allocator) override
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

    virtual bool parse(Tape* tape, IAllocator& allocator) override
    {
        if (IS_A(tape, RawTape)) {
            RawTape* rtape = (RawTape*)tape;
            Raw      raw   = rtape->raw;

            current =
                Str((char*)raw.buffer,
                    raw.size,
                    ((char*)raw.buffer)[raw.size - 1] == 0);
            return true;
        } else {
            return parse_str(tape, current, allocator);
        }
    }
};

template <>
struct TArg<bool> : IArg {
    bool current;

    virtual void* ptr() override { return &current; }

    virtual bool parse(Tape* tape, IAllocator& allocator) override
    {
        return ::parse(tape, current, allocator);
    }
};

struct MOKLIB_API ArgCollection {
    ArgCollection(IAllocator& allocator = System_Allocator)
        : allocator(allocator), args(&allocator)
    {}

    TArray<IArg*> args;
    IAllocator&   allocator;

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