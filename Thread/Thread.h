#pragma once
#include "../Base.h"
#include "../Config.h"

#if OS_MSWINDOWS
#include "../Compat/Win32.h"

#define PROC_THREAD(name) unsigned long name(void *data)

struct ThreadHandle {
    Win32::Handle internal_handle;
};

#elif OS_LINUX
#include <pthread.h>

#define PROC_THREAD(name) void *name(void *data)
struct ThreadHandle {
    pthread_t internal_handle;
};

#else

#error "No threading support for this platform"

#endif

typedef PROC_THREAD(ProcThread);

MOKLIB_API ThreadHandle create_thread(ProcThread *thread_proc, void *proc_data);
MOKLIB_API void sleep_ms(u32 milliseconds);