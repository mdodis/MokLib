#pragma once
#include "../Base.h"
#include "../Host.h"

namespace Time {
    struct TimeSpec {
        uint64 time;

        uint64 to_ms(void);
        float to_s(void);

    };
    
    TimeSpec operator-(const TimeSpec &lhs, const TimeSpec &rhs);

    TimeSpec now();
};