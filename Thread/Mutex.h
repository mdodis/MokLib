#pragma once
#include "Base.h"
#include "Config.h"

#if OS_MSWINDOWS
#include "Compat/Win32.h"

struct MOKLIB_API Mutex {
    Win32::Handle handle;
};

MOKLIB_API Mutex create_mutex();
MOKLIB_API void reserve_mutex(Mutex &mutex);
MOKLIB_API void release_mutex(Mutex &mutex);

#endif

/**
 * Support Structures
 */

/** Locks the mutex. Automatically unlocks it at end of scope */
#define MUTEX_BLOCK(mutex) \
    MutexBlock MCONCAT(_mutex_,__LINE__)(mutex);

struct MutexBlock {
    Mutex &mutex;

    MutexBlock() = delete;
    MutexBlock(Mutex &mutex) : mutex(mutex) {
        reserve_mutex(mutex);
    }

    ~MutexBlock() {
        release_mutex(mutex);
    }
};