#pragma once
#include "Config.h"
#include "Base.h"
#include "Str.h"
#include "Time/Time.h"

#if OS_MSWINDOWS

struct HModule__ {
    int unused;
};

typedef struct HModule__ *HModule;

struct MOKLIB_API Dll {
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

MOKLIB_API Dll load_dll(Str filename);
MOKLIB_API void unload_dll(Dll &dll);
