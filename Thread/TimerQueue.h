#pragma once
#include "Base.h"
#include "Config.h"
#include "Thread/Thread.h"
#include "Thread/Semaphore.h"
#include "Thread/Mutex.h"
#include "Delegates.h"
#include "Containers/Array.h"
#include "Time/Time.h"

#define PROC_TIMER_TIME(name) void name(TimeSpec now)
typedef PROC_TIMER_TIME(ProcTimerTime);

struct MOKLIB_API TimerQueue {

    typedef Delegate<void, TimeSpec> TimerDelegate;

    struct Timer {
        float elapsed;
        /** Interval, in seconds */
        float interval;
        bool valid;
        u32 id;
        TimerDelegate callback;

        u32 ms;
    };

    ThreadHandle thread_handle;
    Semaphore sem_time;
    Mutex timers_lock;
    u32 next_timer_id = 1;
    TimeSpec last_timespec;
    u32 max_ms;

    TArray<Timer> timers;

    void init();
    u32 set_timer(TimerDelegate del, float interval, u32 id = 0);
    void remove_timer(u32 id);
    static PROC_THREAD(timer_thread_proc);
};