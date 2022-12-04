#pragma once
#include "../Config.h"
#include "Time.h"
#include "../Containers/Array.h"
#include "../Delegates.h"

struct MOKLIB_API TimerGroup {
public:
    using Callback = Delegate<void>;

    TimerGroup() {}

    TimerGroup(IAllocator &allocator) {
        init(allocator);
    }

    u32 set_timer(
        TimeSpec interval, 
        bool repeat, 
        Callback callback, 
        u32 id = 0);

    u32 set_timer(
        float interval_sec, 
        bool repeat, 
        Callback callback, 
        u32 id = 0) 
    {
        return set_timer(TimeSpec(interval_sec), repeat, callback, id);
    }
    
    void remove_timer(u32 id);

    void init(IAllocator &allocator = System_Allocator);
    void update();

    bool needs_update = false;
private:
    struct Entry {
        u32 id;
        TimeSpec last_sample;
        TimeSpec elapsed;
        TimeSpec interval;
        bool repeat;
        Callback callback;
        bool enabled;
    };
    TArray<Entry> entries;
    u32 next_timer_id = 1;

    Entry *find_timer_by_id(u32 id);
};