#include "Job.h"
#include "Debugging/Assertions.h"
#include "Semaphore.h"
#include "Thread.h"
#include "Atomic.h"

static bool do_next_job_entry(JobQueue *queue);

PROC_THREAD(JobQueue::thread_proc) {
    JobQueue *queue = (JobQueue*)data;

    while (1) {
        if (do_next_job_entry(queue)) {
            wait_semaphore(queue->signal_work_available);
        }
    }

    return 0;
}

static bool do_next_job_entry(JobQueue *queue) {

    bool should_sleep = false;

    u32 original_next_read_entry = queue->next_read_entry;
    u32 new_next_read_entry = (original_next_read_entry + 1) % queue->num_entries;

    if (original_next_read_entry != queue->next_write_entry) {

        u32 index = compare_and_swapu32(
            &queue->next_read_entry,
            new_next_read_entry,
            original_next_read_entry);

        if (index == original_next_read_entry) {
            JobQueue::Entry *entry = queue->entries + index;

            entry->job(entry->data);

            inc_and_fetch(&queue->completion_count);
        }
    } else {
        should_sleep = true;
    }

    return should_sleep;
}

void JobQueue::init(IAllocator &alloc, u32 max_entries) {
    this->alloc = &alloc;
    num_entries = max_entries;
    entries = (JobQueue::Entry*)alloc.reserve(num_entries * sizeof(JobQueue::Entry));

    signal_work_available = create_semaphore(0);
    ASSERT(entries);
}

void JobQueue::add_job(ProcJob *job, void *data) {

    u32 new_next_write_entry = (next_write_entry + 1) % num_entries;
    ASSERT(new_next_write_entry != next_read_entry);

    JobQueue::Entry *entry = entries + next_write_entry;
    entry->job  = job;
    entry->data = data;

    completion_target = completion_target + 1;

    MEMORY_BARRIER();

    next_write_entry = new_next_write_entry;
    post_semaphore(signal_work_available);
}
