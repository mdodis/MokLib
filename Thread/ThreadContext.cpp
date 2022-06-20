#include "ThreadContext.h"
#include "../Host.h"
#include "Debugging/Assertions.h"

#if OS_MSWINDOWS
#include "../WinInc.h"

static uint32 Context_Index;

void ThreadContextBase::setup(void) {
    Context_Index = TlsAlloc();
    ASSERT(Context_Index != TLS_OUT_OF_INDEXES);
}

void ThreadContextBase::bootstrap_thread(uint64 size) {
    void *p = LocalAlloc(LPTR, size);
    TlsSetValue(Context_Index, p);
}

void ThreadContextBase::set_context(void *value) {
    Context_Index = *(uint32*)value;
}

void *ThreadContextBase::_get_context(void) {
    return TlsGetValue(Context_Index);
}

void *ThreadContextBase::get_context_index() {
    return &Context_Index;
}


umm ThreadContextBase::alloc(uint64 size) {
    umm ptr = (umm)LocalAlloc(LPTR, size);
    return ptr;
}

#elif OS_LINUX
#include <pthread.h>
#include <stdlib.h>

static pthread_key_t Key;

void ThreadContextBase::setup(void) {
    pthread_key_create(&Key, 0);
}

void ThreadContextBase::bootstrap_thread(uint64 size) {
    void *p = malloc(size);
    pthread_setspecific(Key, p);
}

void ThreadContextBase::set_context(void *value) {
    Key = *(pthread_key_t*)value;
}

void *ThreadContextBase::_get_context(void) {
    return pthread_getspecific(Key);
}

void *ThreadContextBase::get_context_index() {
    return (void*)&Key;
}

umm ThreadContextBase::alloc(uint64 size) {
    return (umm)malloc(size);
}

#else
#error "Unsupported TLS for Operating system type!"
#endif

