#pragma once
#include "Math/Base.h"
#include "Tape.h"

struct MOKLIB_API AllocWriteTape : public WriteTape {
    AllocWriteTape(IAllocator& allocator)
        : WriteTape(write_proc, (void*)this), allocator(allocator)
    {}

    void grow(u64 new_size)
    {
        if (new_size <= size) return;

        umm new_ptr = allocator.resize(ptr, size, new_size);
        ptr         = new_ptr;
        size        = new_size;
    }

    static PROC_WRITE_TAPE_WRITE(write_proc)
    {
        AllocWriteTape* self = (AllocWriteTape*)usr;

        switch (mode) {
            case WriteTapeMode::Write: {
                if (size <= 0) return 0;

                u64 sizeu64 = (u64)size;

                if ((sizeu64 + self->offset) > self->size) {
                    u64 by = self->size - (sizeu64 + self->offset);
                    self->grow(self->size + by);
                }

                memcpy(self->ptr + self->offset, src, sizeu64);

                return size;
            } break;

            case WriteTapeMode::Seek: {
                if (size < 0) {
                    u64 sizeu64 = (u64)(-size);

                    if (sizeu64 > self->offset) sizeu64 = self->offset;

                    self->offset -= sizeu64;

                    return -((i64)sizeu64);
                } else {
                    u64 sizeu64 = (u64)size;

                    if ((sizeu64 + self->offset) > self->size) {
                        u64 by = self->size - (sizeu64 + self->offset);
                        self->grow(self->size + by);
                    }

                    memset(self->ptr + self->offset, 0, sizeu64);
                    self->offset += sizeu64;

                    return (i64)sizeu64;
                }
            } break;

            case WriteTapeMode::End: {
                return self->offset == self->size;
            } break;

            default: {
                return 0;
            } break;
        }
    }

    umm         ptr    = 0;
    u64         size   = 0;
    u64         offset = 0;
    IAllocator& allocator;
};

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

    void        move(i64 offset) override {}
    void        release() { alloc.release(ptr); }
    IAllocator& alloc;
};
