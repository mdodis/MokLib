#include "Thread.h"

#if OS_MSWINDOWS
#include "../WinInc.h"

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

void sleep_ms(u32 milliseconds) {
    Sleep(milliseconds);
}

#elif OS_LINUX
#include <time.h>
#include <errno.h>

ThreadHandle create_thread(ProcThread *thread_proc, void *proc_data) {
    pthread_t hnd;
    if (pthread_create(&hnd, 0, thread_proc, proc_data) != 0) {
        return {0};
    }
    return ThreadHandle { hnd };
}

void sleep_ms(u32 milliseconds) {
    struct timespec ts;
    int res;

    if (milliseconds < 0)
    {
        errno = EINVAL;
        return;
    }

    ts.tv_sec = milliseconds / 1000;
    ts.tv_nsec = (milliseconds % 1000) * 1000000;

    do {
        res = nanosleep(&ts, &ts);
    } while (res && errno == EINTR);
}

#endif