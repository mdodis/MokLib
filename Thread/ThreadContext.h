#pragma once
#include "../Base.h"

/**
 * Provides a common interface to TLS
 */
struct ThreadContextBase {

    umm alloc(uint64 size);

    static void setup(void);

    static void bootstrap_thread(uint64 size);

    static void set_context(uint32 value);
    static void *_get_context(void);

    template <typename T>
    static T *get(void) {
        return (T*)_get_context();
    }
};

/**
 * Call this in the initialization step of your thread
 * @param  Type ThreadContext type
 */
#define BOOTSTRAP_THREAD(Type) \
    { \
        ThreadContextBase::bootstrap_thread(sizeof(Type)); \
        void *p = Type::_get_context(); \
        Type *context = new (p) Type; \
    }

