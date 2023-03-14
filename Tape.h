#pragma once
#include <stdio.h>
#include <string.h>

#include "Config.h"
#include "Math/Base.h"
#include "Memory/RawBuffer.h"
#include "Str.h"
#include "Traits.h"

struct MOKLIB_API Tape {
    // Interface
    virtual u64  read(void* destination, u64 amount)   = 0;
    virtual bool write(const void* src, u64 num_bytes) = 0;
    virtual bool end()                                 = 0;
    virtual void move(i64 offset)                      = 0;

    char read_char();
    bool peek_char(char match);
    Str  read_line(IAllocator& alloc);

    // Helpers
    template <typename T>
    bool read_struct(T* destination)
    {
        return read(destination, sizeof(T)) == sizeof(T);
    }

    int64 read_raw(Raw& destination)
    {
        return read(destination.buffer, u32(destination.size));
    }

    bool write_char(char c) { return write(&c, 1); }

    bool write_str(const Str& str) { return write(str.data, str.len); }
};

struct SizedTape : public Tape {
    // Interface
    u64          current_offset, size;
    virtual bool end() override { return current_offset >= size; }

    virtual void move(i64 offset) override
    {
        if (offset < 0) {
            current_offset -= u64(-offset);
        } else {
            current_offset += offset;
        }
    }
};

struct RawTape : public SizedTape {
    Raw raw;

    RawTape(const Raw& raw) : raw(raw)
    {
        current_offset = 0;
        size           = raw.size;
    }

    RawTape(const Str& str) : RawTape(Raw((void*)str.data, str.len)) {}

    u64 read(void* destination, u64 amount) override
    {
        if (current_offset == size) {
            return NumProps<u64>::max;
        }

        u64 offset = current_offset;
        memcpy(destination, umm(raw.buffer) + offset, amount);
        current_offset += amount;
        return amount;
    }

    bool write(const void* src, u64 amount) override
    {
        if ((current_offset + amount) > size) {
            return false;
        }

        u64 offset = min(current_offset, size - current_offset);
        memcpy((umm)raw.buffer + current_offset, src, amount);
        current_offset += amount;
        return true;
    }
};

struct MeasureTape : public Tape {
    u64 num_written = 0;

    // Interface
    virtual u64  read(void* destination, u64 amount) override { return 0; }
    virtual bool write(const void* src, u64 num_bytes) override
    {
        num_written += num_bytes;
        return true;
    }
    virtual bool end() override { return false; }
    virtual void move(i64 offset) override {}
};

/**
 * Tape that keeps track of how many characters were read, and can reset the
 * included tape. Simplifies parsing.
 */
struct ParseTape : public Tape {
    i64   num_read = 0;
    Tape* tape;

    ParseTape(Tape* in_tape) : tape(in_tape) {}

    // Interface
    virtual u64 read(void* destination, u64 amount) override
    {
        u64 rd = tape->read(destination, amount);
        num_read -= rd;
        return rd;
    }

    virtual bool write(const void* src, u64 num_bytes) override
    {
        return false;
    }
    virtual bool end() override { return tape->end(); }
    virtual void move(i64 offset) override
    {
        tape->move(offset);
        num_read -= offset;
    }

    void restore() { tape->move(num_read); }
};
