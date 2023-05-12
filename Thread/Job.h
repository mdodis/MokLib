#pragma once
#include "Base.h"
#include "Delegates.h"
#include "Memory/Base.h"
#include "Thread/Semaphore.h"
#include "Thread/Thread.h"

#define PROC_JOB(name) i32 name(void* data)
typedef PROC_JOB(ProcJob);

struct JobQueue {
    struct Entry {
        Delegate<void> delegate;
    };

    volatile u32 completion_target = 0;
    volatile u32 completion_count  = 0;
    volatile u32 next_write_entry  = 0;
    volatile u32 next_read_entry   = 0;
    Semaphore    signal_work_available;

    Entry*     entries;
    u32        num_entries;
    Allocator* alloc;

    void init(Allocator& alloc, u32 max_entries = 128);
    /** Add a new job. Should only be done from a single thread (the main
     * thread) */
    void add_job(Delegate<void>&& delegate);

    static PROC_THREAD(thread_proc);
};