#pragma once
#include "Math/Base.h"
#include "Tape.h"

/**
 *
 */
struct MOKLIB_API AllocTape : public Tape {
    u64 size;
    u64 wr_offset;
    u64 rd_offset;
    umm ptr;

    AllocTape(IAllocator& alloc)
        : alloc(alloc), size(0ul), wr_offset(0ul), rd_offset(0ul), ptr(0)
    {}

    u64 read(void* destination, u64 amount) override
    {
        if (!ptr) return 0;

        u64 read_size = min(rd_offset + amount, size);

        memcpy(destination, ptr + rd_offset, read_size);
        rd_offset += read_size;
        return read_size;
    }

    bool write(const void* src, u64 num_bytes) override
    {
        if (!ptr) {
            ptr  = alloc.reserve(num_bytes);
            size = num_bytes;
        }

        if (wr_offset + num_bytes > size) {
            u64 extra   = (wr_offset + num_bytes) - size;
            umm new_ptr = alloc.resize(ptr, size, size + extra);
            if (!new_ptr) {
                alloc.release(ptr);
                return false;
            }
            ptr  = new_ptr;
            size = size + extra;
        }

        memcpy(ptr + wr_offset, src, num_bytes);

        wr_offset += num_bytes;

        return true;
    }

    bool end() override { return rd_offset == size; }

    void move(i64 offset) override {}

    IAllocator& alloc;
};
