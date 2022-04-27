#pragma once
#include "Config.h"
#include "Containers/List.h"
#include "Math/Base.h"
#include "Memory/RawBuffer.h"
#include "Str.h"
#include <stdio.h>
#include <string.h>

struct MOKLIB_API Tape {
    // Interface
    virtual u64  read(void *destination, u64 amount) = 0;
    virtual bool write(const void *src, u64 num_bytes) = 0;
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

    Str read_line(IAllocator &alloc) {
        const u32 initial_buffer_size = 1024;

        u32 buffer_size = initial_buffer_size;
        char *data = (char*)alloc.reserve(initial_buffer_size);

        i64 num_read = read(data, initial_buffer_size);
        if (num_read == -1) {
            alloc.release((umm)data);
            return Str::NullStr;
        } else {

            if ((data[num_read - 1]) == '\r' || (data[num_read - 1]) == '\n')
                num_read--;
            if ((data[num_read - 1]) == '\r' || (data[num_read - 1]) == '\n')
                num_read--;

            return Str(data, num_read);
        }
    }

    bool write_str(const Str &str) {
        return write(str.data, str.len);
    }

    bool write_str(const TList<Str> &strs) {
        for (const Str &str : strs) {
            if (!write(str.data, str.len)) {
                return false;
            }
        }

        return true;
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

    u64 read(void *destination, u64 amount) override {
        if (current_offset == size) {
            return U64::Max;
        }

        u64 offset = current_offset;
        memcpy(destination, umm(raw.buffer) + offset, amount);
        current_offset += amount;
        return amount;
    }

    bool write(const void *src, u64 amount) override {
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
    virtual u64 read(void *destination, u64 amount) override { return 0; }
    virtual bool write(const void *src, u64 num_bytes) override {
        num_written += num_bytes;
        return true;
    }
    virtual bool end() override { return false; }
    virtual void move(i64 offset) {}
};