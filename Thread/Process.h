#pragma once
#include "Base.h"
#include "../Config.h"

struct MOKLIB_API Process {
    const char *program;
    const char *command_line;
#if OS_LINUX
    int pid;
#else
    #error "No Process support for this platform!"
#endif
};

bool MOKLIB_API create_process(Process &process);