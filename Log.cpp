#include "Log.h"

#include "FileSystem/FileSystem.h"
#include "Str.h"

WriteTape* ConsoleLogContext::open()
{
    FileHandle fh = open_file(filepath, FileMode::ShareRead | FileMode::Write);

    ft = FileWriteTape<false>(fh);
    ft.seek_to_end();
    st = get_write_stream(Console::Output);

    return &tape;
}

void ConsoleLogContext::close() { close_file(ft.file); }

static ILogContext* Log_Context;

ILogContext* get_log_context() { return Log_Context; }

void set_log_context(ILogContext* context) { Log_Context = context; }
