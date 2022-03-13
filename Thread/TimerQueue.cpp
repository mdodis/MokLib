#include "TimerQueue.h"
#include "Thread/Atomic.h"
#include "Math/Base.h"

static i32 find_timer_by_id(const TArray<TimerQueue::Timer> &timers, u32 id);
static void check_timers(TimerQueue *queue, TimeSpec now);
static void recalculate_ms(TimerQueue *queue);

PROC_THREAD(TimerQueue::timer_thread_proc) {

    TimerQueue *queue = (TimerQueue*)data;

    while (1) {

        queue->last_timespec = now_time();

        const auto exit_reason = wait_timeout_semaphore(
            queue->sem_time,
            queue->max_ms);
        ASSERT(exit_reason != SemaphoreWaitExitReason::Error);

        auto new_timespec = now_time();
        check_timers(queue, new_timespec);

        MEMORY_BARRIER();
        queue->last_timespec = new_timespec;

    }
}

static void recalculate_ms(TimerQueue *queue) {
    MUTEX_BLOCK(queue->timers_lock);
    queue->max_ms = ~0u;
    for (auto &timer : queue->timers) {
        const u32 ms = u32((timer.interval) / 1000.f);
        queue->max_ms = min(ms, queue->max_ms);
    }
}

static void check_timers(TimerQueue *queue, TimeSpec now) {
    MUTEX_BLOCK(queue->timers_lock);

    for (auto &timer : queue->timers) {
        u64 ms = (now - queue->last_timespec).to_ms();

        if (ms >= secs_to_msecs(timer.interval)) {
            timer.callback.call(now);
        }
    }

    // @note: we do a reverse traversal since deleting a timer using fast delete
    // will bring the last element back
    for (i32 i = (queue->timers.size - 1); i >= 0; --i) {
        if (!queue->timers[i].valid) {
            queue->timers.del(i);
        }
    }
}

void TimerQueue::init() {
    max_ms = 9000;
    sem_time = create_semaphore(0);
    thread_handle = create_thread(timer_thread_proc, (void*)this);
    timers_lock = create_mutex();
}

u32 TimerQueue::set_timer(TimerDelegate del, float interval, u32 id) {
    {
        MUTEX_BLOCK(timers_lock);

        if (id != 0) {
            i32 timer_index = find_timer_by_id(timers, id);
            if (timer_index == -1) {
                return 0;
            }
        }

        id = ++next_timer_id;

        Timer *timer = timers.add();
        timer->elapsed = 0.f;
        timer->callback = del;
        timer->interval = interval;
        timer->valid = true;
        timer->id = id;

        const u32 interval_ms = (u32)truncate(interval / 1000.f);
        max_ms = min(max_ms, interval_ms);
    }

    post_semaphore(sem_time);

    return id;
}

void TimerQueue::remove_timer(u32 id) {

    i32 index = find_timer_by_id(timers, id);
    if (index == -1) {
        return;
    }

    timers[index].valid = false;

    post_semaphore(sem_time);
}

static i32 find_timer_by_id(const TArray<TimerQueue::Timer> &timers, u32 id) {
    for (i32 i = 0; i < timers.size; ++i) {
        if (timers[i].id == id) {
            return i;
        }
    }

    return -1;
}