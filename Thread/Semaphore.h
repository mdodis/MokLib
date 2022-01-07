#pragma once
#include "Base.h"

#if OS_MSWINDOWS
#include "Compat/Win32.h"

struct Semaphore {
    Win32::Handle handle;
};

#endif

Semaphore create_semaphore(u32 initial_count);
void post_semaphore(Semaphore &sem);
void wait_semaphore(Semaphore &sem);