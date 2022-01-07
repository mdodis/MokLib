#include "Thread.h"

#if OS_MSWINDOWS
#include "WinInc.h"

ThreadHandle create_thread(ProcThread *thread_proc, void *proc_data) {

    HANDLE result = CreateThread(
        0,
        0,
        thread_proc,
        proc_data,
        0,
        0);

    if (result == INVALID_HANDLE_VALUE) {
        return ThreadHandle {0};
    } else {
        return ThreadHandle {result};
    }
}

#endif