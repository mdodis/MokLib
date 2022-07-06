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

#elif OS_LINUX

Mutex create_mutex() {
    pthread_mutex_t mutex;
    int result = pthread_mutex_init(&mutex, 0);

    // @todo: refactor so that we can know if the mutex is valid
    return Mutex { mutex };
}

void reserve_mutex(Mutex &mutex) {
    pthread_mutex_lock(&mutex.handle);
}

void release_mutex(Mutex &mutex) {
    pthread_mutex_unlock(&mutex.handle);
}

#endif // OS_MSWINDOWS