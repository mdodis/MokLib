#include "ThreadContext.h"
#include "../Host.h"
#include "../Debugging.h"

#if OS_WINDOWS
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

void ThreadContextBase::setup(void) {
    ASSERT(false);
}

void ThreadContextBase::bootstrap_thread(uint64 size) {
    ASSERT(false);
}

void ThreadContextBase::set_context(void *value) {
    ASSERT(false);
}

void *ThreadContextBase::_get_context(void) {
    return 0;
}

void *ThreadContextBase::get_context_index() {
    return 0;
}

umm ThreadContextBase::alloc(uint64 size) {
    return 0;
}

#else
#error "Unsupported TLS for Operating system type!"
#endif

