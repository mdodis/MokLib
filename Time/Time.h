#pragma once
#include "../Base.h"
#include "../Host.h"

namespace Time {

    #if OS_LINUX
        #include <stddef.h>
        #include <time.h>
    #endif

    struct TimeSpec {

        #if OS_WINDOWS
            uint64 time;
        #elif OS_LINUX
            timespec time;
        #endif

        uint64 to_ms(void);
        float to_s(void);

    };
    
    TimeSpec operator-(const TimeSpec &lhs, const TimeSpec &rhs);
    int compare_time(const TimeSpec &lhs, const TimeSpec &rhs);

    TimeSpec now();
};