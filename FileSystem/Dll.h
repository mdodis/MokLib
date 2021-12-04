#pragma once
#include "Base.h"
#include "Str.h"
#include "Time/Time.h"

#if OS_WINDOWS
#include "WinInc.h"

struct Dll {
    MTime::TimeSpec last_time;
    HMODULE handle;
    void *get_proc_address(const char *name);
};

#elif OS_LINUX

struct Dll {
    MTime::TimeSpec last_time;
    void *handle;
    void *get_proc_address(const char *name);
};

#else
#error "No Dll support for this platform!"
#endif

Dll load_dll(Str filename);
void unload_dll(Dll &dll);