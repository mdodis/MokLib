#include "Semaphore.h"

#if OS_MSWINDOWS
#include "WinInc.h"

Semaphore create_semaphore(u32 initial_count) {

    HANDLE result = CreateSemaphore(
        0,
        initial_count,
        I32::Max,
        0);

    return Semaphore {
        result,
        result != INVALID_HANDLE_VALUE
    };
}

void post_semaphore(Semaphore &sem) {
    ReleaseSemaphore(sem.handle, 1, 0);
}

void wait_semaphore(Semaphore &sem) {
    WaitForSingleObjectEx(sem.handle, INFINITE, FALSE);
}

SemaphoreWaitExitReason wait_timeout_semaphore(Semaphore &sem, u32 ms) {
    DWORD reason = WaitForSingleObjectEx(sem.handle, ms, FALSE);

    SemaphoreWaitExitReason result;
    switch (reason) {
        case WAIT_OBJECT_0:
            result = SemaphoreWaitExitReason::Post;
            break;
        case WAIT_TIMEOUT:
            result = SemaphoreWaitExitReason::Timeout;
            break;
        default:
            result = SemaphoreWaitExitReason::Error;
            break;
    }

    return result;
}

#endif
