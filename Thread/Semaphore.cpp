#include "Semaphore.h"

#if OS_MSWINDOWS
#include "WinInc.h"

Semaphore create_semaphore(u32 initial_count) {
    HANDLE result = CreateSemaphore(
        0,
        initial_count,
        ~0u,
        0);

    if (result != INVALID_HANDLE_VALUE) {
        return Semaphore {0};
    } else {
        return Semaphore {result};
    }
}

void post_semaphore(Semaphore &sem) {
    ReleaseSemaphore(sem.handle, 1, 0);
}

void wait_semaphore(Semaphore &sem) {
    WaitForSingleObjectEx(sem.handle, INFINITE, FALSE);
}


#endif
