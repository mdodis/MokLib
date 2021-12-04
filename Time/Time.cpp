#include "Time.h"
#include "Debugging.h"

namespace MTime {
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

int compare_time(const TimeSpec &lhs, const TimeSpec &rhs) {
    TimeSpec diff = lhs - rhs;
    return CompareFileTime((FILETIME*)&rhs.time, (FILETIME*)&lhs.time);
}


#elif OS_LINUX

TimeSpec now() {
    struct timespec result;
    clock_gettime(CLOCK_MONOTONIC, &result);

    return TimeSpec {result};
}

uint64 TimeSpec::to_ms(void) {
    uint64 ms =
        uint64((double) time.tv_nsec / (double) 1000000.0) +
        time.tv_sec * 1000;
    return ms;
}

float TimeSpec::to_s(void) {
    double result =
        (double) time.tv_nsec / (double) 1000000000.0 +
        time.tv_sec;

    return float(result);
}

TimeSpec operator-(const TimeSpec &lhs, const TimeSpec &rhs) {
    TimeSpec result;

    int64 nsec_diff = lhs.time.tv_nsec - rhs.time.tv_nsec;
    int64 sec_diff  = lhs.time.tv_sec  - rhs.time.tv_sec;
    if (nsec_diff < 0) {
        result.time.tv_sec = sec_diff - 1;
        result.time.tv_nsec = 1000000000 + nsec_diff;
    } else {
        result.time.tv_nsec = nsec_diff;
        result.time.tv_sec = sec_diff;
    }

    return result;
}

int compare_time(const TimeSpec &lhs, const TimeSpec &rhs) {
    TimeSpec diff = lhs - rhs;

    if ((diff.time.tv_sec == 0) && (diff.time.tv_nsec == 0)) {
        return 0;
    } else {
        return diff.time.tv_sec == 0
            ? int32(diff.time.tv_nsec)
            : int32(diff.time.tv_sec);
    }
}


#else
#endif

}