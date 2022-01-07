#pragma once
#include "../Config.h"
#include "../Base.h"

/**
 * Provides a common interface to TLS
 */
struct ThreadContextBase {

    MOKLIB_API umm alloc(uint64 size);

    MOKLIB_API static void setup(void);
    MOKLIB_API static void bootstrap_thread(uint64 size);
    MOKLIB_API static void set_context(void *value);
    MOKLIB_API static void *_get_context(void);
    MOKLIB_API static void *get_context_index();

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

