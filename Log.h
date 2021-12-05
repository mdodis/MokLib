#pragma once
#include "Containers/List.h"
#include "FileSystem/FileSystem.h"
#include "Str.h"
#include "StringBuilder.h"

struct ILog {

    Str file;

    void set_file(const Str &path) {
        file = path;
    }

    void print(TList<Str> &what, bool newline = true);
    _inline void print(StringBuilder &what, bool newline = true) {
        TList<Str> list = what.to_list();
        print(list, newline);
    }

    virtual void add_output(const Str &what) = 0;
};

#ifndef MOK_INCLUDE_CONSOLE_OUTPUT_LOG
#define MOK_INCLUDE_CONSOLE_OUTPUT_LOG 1
#endif

#if MOK_INCLUDE_CONSOLE_OUTPUT_LOG

#ifndef MOK_CONSOLE_ARENA_SIZE
#define MOK_CONSOLE_ARENA_SIZE 2048
#endif

#include "Memory/Arena.h"

#include <stdio.h>
struct ConsoleOutputLog : ILog {
    virtual void add_output(const Str &what) override {
        printf("%.*s", what.len, what.data);
    }
};

static ConsoleOutputLog Global_Console_Output_Log;

#define PRINTLN(what) do { \
        CREATE_INLINE_ARENA(console_arena, MOK_CONSOLE_ARENA_SIZE); \
        Global_Console_Output_Log.print(StringBuilder(console_arena.to_alloc()) + what); \
    } while(0)

#define PRINT(what) do { \
        CREATE_INLINE_ARENA(console_arena, MOK_CONSOLE_ARENA_SIZE); \
        Global_Console_Output_Log.print(StringBuilder(console_arena.to_alloc()) + what, false); \
    } while (0)

#endif