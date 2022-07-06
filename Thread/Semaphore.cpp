#include "Semaphore.h"
#include "Traits.h"

#if OS_MSWINDOWS
#include "WinInc.h"

Semaphore create_semaphore(u32 initial_count) {

    HANDLE result = CreateSemaphore(
        0,
        initial_count,
        NumProps<i32>::max,
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

#elif OS_LINUX
#include <fcntl.h>
#include <sys/stat.h>

Semaphore create_semaphore(u32 initial_count) {

    sem_t *handle = sem_open(0, O_CREAT, 0666, initial_count);

    return Semaphore {
        handle,
        handle != SEM_FAILED
    };

}

void post_semaphore(Semaphore &sem) {
    sem_close(sem.handle);
}

void wait_semaphore(Semaphore &sem) {
    sem_wait(sem.handle);
}

SemaphoreWaitExitReason wait_timeout_semaphore(Semaphore &sem, u32 ms) {
    // @todo: timeout
    // DWORD reason = WaitForSingleObjectEx(sem.handle, ms, FALSE);

    // SemaphoreWaitExitReason result;
    // switch (reason) {
    //     case WAIT_OBJECT_0:
    //         result = SemaphoreWaitExitReason::Post;
    //         break;
    //     case WAIT_TIMEOUT:
    //         result = SemaphoreWaitExitReason::Timeout;
    //         break;
    //     default:
    //         result = SemaphoreWaitExitReason::Error;
    //         break;
    // }

    // return result;
}


#endif
