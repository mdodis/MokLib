#include "DebugOutput.h"
#include "Tape.h"
#include "FileSystem/FileSystem.h"

struct SimpleDebugTape : Tape {
    // Interface
    virtual u64 read(void *destination, u64 amount) override {
        return 0;
    }

    virtual bool write(const void *src, u64 num_bytes) override {
        auto out = get_stream(Console::Output);
        return out.write(src, num_bytes);
    }

    virtual bool end() override { return false; }
    virtual void move(i64 offset) override {}
};

static SimpleDebugTape _Simple_Debug_Tape;
static Tape *Current_Debug_Tape = &_Simple_Debug_Tape;

Tape *get_debug_tape() { return Current_Debug_Tape; }
void set_debug_tape(Tape *tape) { Current_Debug_Tape = tape; }