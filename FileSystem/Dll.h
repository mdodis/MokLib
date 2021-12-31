#pragma once
#include "Base.h"
#include "Str.h"
#include "Time/Time.h"

#if OS_MSWINDOWS

struct HModule__ {
    int unused;
};

typedef struct HModule__ *HModule;

struct Dll {
    HModule handle;
    void *get_proc_address(const char *name);
};

#elif OS_LINUX

struct Dll {
    void *handle;
    void *get_proc_address(const char *name);
};

#else
#error "No Dll support for this platform!"
#endif

Dll load_dll(Str filename);
void unload_dll(Dll &dll);
