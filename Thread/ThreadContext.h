#pragma once
#include "../Base.h"
#include "../Config.h"
#include "../Memory/Arena.h"

/**
 * Provides a common interface to TLS
 */
struct MOKLIB_API ThreadContextBase {
    umm alloc(uint64 size);

    static void  setup(void);
    static void  bootstrap_thread(uint64 size);
    static void  set_context(void* value);
    static void* _get_context(void);
    static void* get_context_index();

    virtual Allocator& get_temp_allocator() = 0;

    template <typename T>
    static T* get(void)
    {
        return (T*)_get_context();
    }
};

/**
 * Simple Thread Context
 */
struct MOKLIB_API SimpleThreadContext : ThreadContextBase {
    Arena<ArenaMode::Fixed>      scratch_arena;
    const uint32 scratch_arena_size = MEGABYTES(2);

    Allocator& get_temp_allocator() override { return scratch_arena; }

    SimpleThreadContext()
    {
        scratch_arena.memory = this->alloc(scratch_arena_size);
        scratch_arena.used   = 0;
        scratch_arena.capacity = scratch_arena_size;
    }
};

/**
 * Call this in the initialization step of your thread
 * @param  Type ThreadContext type
 */
#define BOOTSTRAP_THREAD(Type)                             \
    {                                                      \
        ThreadContextBase::bootstrap_thread(sizeof(Type)); \
        void* p       = Type::_get_context();              \
        Type* context = new (p) Type;                      \
    }

#define CREATE_SIMPLE_THREAD_CONTEXT() BOOTSTRAP_THREAD(SimpleThreadContext)

#define GET_LOCAL_ARENA() \
    ThreadContextBase::get<ThreadContextBase>()->get_temp_allocator();

#define MAKE_TEMP_ARENA(name, size)                                         \
    CREATE_SCOPED_ARENA(                                                    \
        &ThreadContextBase::get<ThreadContextBase>()->get_temp_allocator(), \
        name,                                                               \
        size)