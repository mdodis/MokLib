#include "DebugOutput.h"

#include "FileSystem/FileSystem.h"
#include "Tape.h"

struct SimpleDebugTape : public WriteTape {
    SimpleDebugTape() : WriteTape(write_proc, (void*)this) {}

    static PROC_WRITE_TAPE_WRITE(write_proc)
    {
        switch (mode) {
            case WriteTapeMode::Write: {
                auto stream = get_write_stream(Console::Output);
                stream.write(src, size);
                return size;
            } break;

            default: {
                return 0;
            } break;
        }
    }
};

static SimpleDebugTape _Simple_Debug_Tape;
static WriteTape*      Current_Debug_Tape = &_Simple_Debug_Tape;

WriteTape* get_debug_tape() { return Current_Debug_Tape; }
void       set_debug_tape(WriteTape* tape) { Current_Debug_Tape = tape; }