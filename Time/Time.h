#pragma once
#include "../Config.h"
#include "../Base.h"
#include "../Host.h"

#if OS_LINUX
    #include <stddef.h>
    #include <time.h>
#endif

struct MOKLIB_API TimeSpec {
    TimeSpec() {}
    TimeSpec(u64 ms);
    #if OS_MSWINDOWS
        uint64 time;
    #elif OS_LINUX
        TimeSpec(struct timespec time) : time(time) {}
        timespec time;
    #endif

    uint64 to_ms(void);
    float to_s(void);

};

MOKLIB_API TimeSpec operator-(const TimeSpec &lhs, const TimeSpec &rhs);
MOKLIB_API TimeSpec operator+(const TimeSpec &lhs, const TimeSpec &rhs);
MOKLIB_API int compare_time(const TimeSpec &lhs, const TimeSpec &rhs);
MOKLIB_API TimeSpec now_time();

static _inline u32 secs_to_msecs(f32 seconds) {
    return u32(seconds / 1000.f);
}