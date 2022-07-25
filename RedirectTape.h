#pragma once
#include "Tape.h"
#include "Containers/Array.h"

/**
 * Redirects writes to multiple sub-tapes
 */
template <u32 NumTapes>
struct RedirectTape : public Tape {

    TArr<Tape*, NumTapes> sub_tapes;

    RedirectTape(TArr<Tape*, NumTapes> sub_tapes)
        : sub_tapes(sub_tapes)
        {}

    // Interface
    virtual u64 read(void *destination, u64 amount) override { return 0; }
    virtual bool write(const void *src, u64 num_bytes) override {
        for (Tape *t : sub_tapes) {
            if (!t->write(src, num_bytes)) {
                return false;
            }
        }
        return true;
    }
    virtual bool end() override { return false; }
    virtual void move(i64 offset) override {}
};