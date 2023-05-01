#include "Time/TimerGroup.h"

void TimerGroup::init(Allocator& allocator)
{
    entries = TArray<TimerGroup::Entry>(&allocator);
}

u32 TimerGroup::set_timer(
    TimeSpec interval, bool repeat, TimerGroup::Callback callback, u32 id)
{
    TimerGroup::Entry* entry;

    if (id == 0) {
        entry     = entries.add();
        entry->id = next_timer_id++;
    } else {
        entry = find_timer_by_id(id);
        if (!entry) return 0;
    }

    entry->interval    = interval;
    entry->repeat      = repeat;
    entry->last_sample = now_time();
    entry->callback    = callback;
    entry->enabled     = true;
    entry->elapsed     = TimeSpec((u64)0);

    needs_update = true;

    return entry->id;
}

void TimerGroup::remove_timer(u32 id)
{
    TimerGroup::Entry* entry = find_timer_by_id(id);
    if (!entry) return;
    entry->enabled = false;
}

void TimerGroup::update()
{
    auto new_time = now_time();
    for (auto& entry : entries) {
        if (!entry.enabled) continue;

        TimeSpec elapsed  = (new_time - entry.last_sample);
        entry.last_sample = new_time;
        entry.elapsed     = entry.elapsed + elapsed;

        if (compare_time(entry.elapsed, entry.interval) >= 0) {
            entry.callback.call();

            // @todo: this is wrong. callback should be invoked multiple times
            // for each timer if the elapsed time is a multiple of the interval
            if (entry.repeat) {
                entry.interval = entry.elapsed - entry.interval;
                needs_update   = true;
            } else {
                entry.enabled = false;
            }
        }
    }

    // Delete marked entries
    for (u64 i = entries.size; i > 0; --i) {
        if (entries[i - 1].enabled) continue;

        entries.del(i - 1);
    }
}

TimerGroup::Entry* TimerGroup::find_timer_by_id(u32 id)
{
    for (auto& entry : entries) {
        if (entry.id == id) {
            return &entry;
        }
    }
    return 0;
}
