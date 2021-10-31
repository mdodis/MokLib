#include "Time.h"
#include "Debugging.h"

namespace Time {
#if OS_WINDOWS
#include "WinInc.h"

static LARGE_INTEGER The_Counter;

TimeSpec now() {

    TimeSpec result;
    ASSERT(QueryPerformanceCounter((LARGE_INTEGER*)&result.time));
    return result;
}

uint64 TimeSpec::to_ms(void) {
    static bool has_counter = false;
    if (!has_counter) {
        ASSERT(QueryPerformanceFrequency(&The_Counter));
        has_counter = true;
    }

    return (time * 1000) / The_Counter.QuadPart;
}

float TimeSpec::to_s(void) {
    uint64 ms = to_ms();

    return float((double)ms / 1000.0);
}

TimeSpec operator-(const TimeSpec &lhs, const TimeSpec &rhs) {
    TimeSpec result;
    result.time = (uint64)(lhs.time - rhs.time);
    return result;
}

#else
#endif

}