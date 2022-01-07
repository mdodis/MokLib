#pragma once
#include "Base.h"

#if OS_MSWINDOWS
#include "Compat/Win32.h"

#define PROC_THREAD(name) unsigned long name(void *data)

struct ThreadHandle {
    Win32::Handle internal_handle;
};

#endif

typedef PROC_THREAD(ProcThread);

ThreadHandle create_thread(ProcThread *thread_proc, void *proc_data);