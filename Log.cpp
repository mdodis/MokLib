#include "Log.h"
#include "FileSystem/FileSystem.h"
#include "Str.h"

Tape *ConsoleLogContext::open() {
    FileHandle fh = open_file(
        filepath,
        FileMode::ShareRead |
        FileMode::Write);

    ft = FileTape(fh);
    ft.move_to_end();
    st = get_stream(Console::Output);

    return &tape;
}

void ConsoleLogContext::close() {
    close_file(ft.file);
}

static ILogContext *Log_Context;

ILogContext* get_log_context() {
    return Log_Context;
}

void set_log_context(ILogContext *context) {
    Log_Context = context;
}

