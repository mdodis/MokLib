#pragma once
#include "Math/Base.h"
#include "Memory/RawBuffer.h"
#include "Str.h"
#include <stdio.h>
#include <string.h>

struct Tape {
    // Interface
    virtual i64 read(void *destination, u32 amount) = 0;
    virtual bool write(const void *src, u32 num_bytes) = 0;
    virtual bool end() = 0;
    virtual void move(i64 offset) = 0;

    // Helpers
    template <typename T>
    bool read_struct(T *destination) {
        return read(destination, sizeof(T)) == sizeof(T);
    }

    char read_char() {
        char result;
        if (read(&result, 1) != 1) {
            return EOF;
        } else {
            return result;
        }
    }

    bool peek_char(char match) {
        char result;
        if (read(&result, 1) != 1) {
            return false;
        } else {

            if (result == match) {
                return true;
            } else {
                move(-1);
                return false;
            }
        }
    }

    int64 read_raw(Raw &destination) {
        return read(destination.buffer, u32(destination.size));
    }

    bool write_str(const Str &str) {
        return write(str.data, str.len);
    }
};

struct SizedTape : public Tape {
    // Interface
    u64 current_offset, size;
    virtual bool end() override {
        return current_offset >= size;
    }

    virtual void move(i64 offset) override {
        if (offset < 0) {
            current_offset -= u64(-offset);
        } else {
            current_offset += offset;
        }
    }

};

struct RawTape : public SizedTape {
    Raw raw;

    RawTape(const Raw &raw) : raw(raw) {
        current_offset = 0;
        size = raw.size;
    }

    i64 read(void *destination, u32 amount) override {
        if (current_offset == size) {
            return -1;
        }

        u64 offset = min(current_offset, size - current_offset);
        memcpy(destination, umm(raw.buffer) + offset, amount);
        return amount;
    }

    bool write(const void *src, u32 amount) override {
        if ((current_offset + amount) > size) {
            return false;
        }

        u64 offset = min(current_offset, size - current_offset);
        memcpy(raw.buffer, src, amount);
        return true;
    }
};