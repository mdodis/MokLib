#pragma once
#include "../Config.h"
#include "../Base.h"
#include "../Host.h"

namespace MTime {

    #if OS_LINUX
        #include <stddef.h>
        #include <time.h>
    #endif

    struct MOKLIB_API TimeSpec {

        #if OS_MSWINDOWS
            uint64 time;
        #elif OS_LINUX
            timespec time;
        #endif

        uint64 to_ms(void);
        float to_s(void);

    };
    
    MOKLIB_API TimeSpec operator-(const TimeSpec &lhs, const TimeSpec &rhs);
    MOKLIB_API int compare_time(const TimeSpec &lhs, const TimeSpec &rhs);
    MOKLIB_API TimeSpec now();
};