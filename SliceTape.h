#pragma once
#include "Tape.h"
#include "Containers/Slice.h"

struct MOKLIB_API SliceTape : Tape {
    Slice<Str> strings;
    u64 current_index;
    u64 current_start_offset;

    SliceTape(const Slice<Str> &strings)
        : strings(strings)
        , current_index(0)
        , current_start_offset(0)
        {}

    u64 get_size();

    virtual u64 read(void *destination, u64 amount) override;
    virtual void move(i64 offset) override;
    void move_forwards(u64 offset);
    void move_backwards(u64 offset);
    virtual bool end() override;
    virtual bool write(const void *src, u64 num_bytes) override;
};