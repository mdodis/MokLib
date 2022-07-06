#pragma once
#include "Base.h"
#include "Config.h"

#if OS_MSWINDOWS
#include "Compat/Win32.h"

struct MOKLIB_API Semaphore {
    Win32::Handle handle;
    bool valid;
};

#elif OS_LINUX
#include <semaphore.h>

struct MOKLIB_API Semaphore {
    sem_t *handle;
    bool valid;
};

#endif

enum class SemaphoreWaitExitReason {
    Post,       // The Semaphore was posted by another thread
    Timeout,    // The wait has timed-out
    Error,      // Some unknown error has occured. Probably if you haven't initialized the semaphore
};

MOKLIB_API Semaphore create_semaphore(u32 initial_count);
MOKLIB_API void post_semaphore(Semaphore &sem);
MOKLIB_API void wait_semaphore(Semaphore &sem);
MOKLIB_API SemaphoreWaitExitReason wait_timeout_semaphore(Semaphore &sem, u32 ms);