#pragma once
#include "Containers/Array.h"
#include "Tape.h"

/**
 * Redirects writes to multiple sub-tapes
 */
template <u32 NumTapes>
struct RedirectTape : public Tape {
    TArr<Tape*, NumTapes> sub_tapes;

    RedirectTape(TArr<Tape*, NumTapes> sub_tapes) : sub_tapes(sub_tapes) {}

    // Interface
    virtual u64  read(void* destination, u64 amount) override { return 0; }
    virtual bool write(const void* src, u64 num_bytes) override
    {
        for (Tape* t : sub_tapes) {
            if (!t->write(src, num_bytes)) {
                return false;
            }
        }
        return true;
    }
    virtual bool end() override { return false; }
    virtual void move(i64 offset) override {}
};

template <u32 Count>
struct RedirectWriteTape : public WriteTape {
    RedirectWriteTape(TArr<WriteTape*, Count> sub_tapes)
        : sub_tapes(sub_tapes), WriteTape(write_proc, (void*)this)
    {}

    static PROC_WRITE_TAPE_WRITE(write_proc)
    {
        RedirectWriteTape* self = (RedirectWriteTape*)usr;
        switch (mode) {
            case WriteTapeMode::Write: {
                for (WriteTape* t : self->sub_tapes) {
                    t->write(src, size);
                }
                return size;
            } break;
            default: {
            } break;
        }

        return 0;
    }

    TArr<WriteTape*, Count> sub_tapes;
};
