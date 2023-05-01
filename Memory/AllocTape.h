#pragma once
#include "Math/Base.h"
#include "Tape.h"
#include "Memory/Base.h"

struct MOKLIB_API AllocWriteTape : public WriteTape {
    AllocWriteTape(Allocator& allocator)
        : WriteTape(write_proc, (void*)this), allocator(allocator)
    {}

    void grow(u64 new_size)
    {
        if (new_size <= size) return;

        if (!ptr) {
            ptr  = (umm)allocator.reserve(new_size);
            size = new_size;
            return;
        }

        umm new_ptr = (umm)allocator.resize(ptr, size, new_size);
        ptr         = new_ptr;
        size        = new_size;
    }

    void release()
    {
        allocator.release(ptr);
        ptr  = 0;
        size = 0;
    }

    static PROC_WRITE_TAPE_WRITE(write_proc)
    {
        AllocWriteTape* self = (AllocWriteTape*)usr;

        switch (mode) {
            case WriteTapeMode::Write: {
                if (size <= 0) return 0;

                u64 sizeu64 = (u64)size;
                u64 add     = sizeu64 + self->offset;

                if (add > self->size) {
                    u64 by = add - self->size;
                    self->grow(self->size + by);
                }

                memcpy(self->ptr + self->offset, src, sizeu64);
                self->offset += sizeu64;

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
                    u64 add     = sizeu64 + self->offset;

                    if (add > self->size) {
                        u64 by        = add - self->size;
                        u64 prev_size = self->size;
                        self->grow(self->size + by);

                        memset(self->ptr + prev_size, 0, by);
                    }

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

    umm        ptr    = 0;
    u64        size   = 0;
    u64        offset = 0;
    Allocator& allocator;
};

/**
 *
 */
struct MOKLIB_API AllocTape : public Tape {
    u64 size;
    u64 wr_offset;
    u64 rd_offset;
    umm ptr;

    AllocTape(Allocator& alloc)
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
            ptr  = (umm)alloc.reserve(num_bytes);
            size = num_bytes;
        }

        if (wr_offset + num_bytes > size) {
            u64 extra   = (wr_offset + num_bytes) - size;
            umm new_ptr = (umm)alloc.resize(ptr, size, size + extra);
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

    void       move(i64 offset) override {}
    void       release() { alloc.release(ptr); }
    Allocator& alloc;
};
