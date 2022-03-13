#include "Mutex.h"

#if OS_MSWINDOWS
#include "WinInc.h"

Mutex create_mutex() {
    HANDLE result = CreateMutexA(0, false, 0);

    return Mutex { result };
}

void reserve_mutex(Mutex &mutex) {
    WaitForSingleObject(mutex.handle, INFINITE);
}

void release_mutex(Mutex &mutex) {
    ReleaseMutex(mutex.handle);
}

#endif // OS_MSWINDOWS