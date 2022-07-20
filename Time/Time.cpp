#include "Time.h"
#include "Debugging/Assertions.h"

#if OS_MSWINDOWS
#include "WinInc.h"

static LARGE_INTEGER The_Counter;
static bool _timespec_win32_has_counter = false;

static void query_frequency() {
    if (!_timespec_win32_has_counter) {
        ASSERT(QueryPerformanceFrequency(&The_Counter));
        _timespec_win32_has_counter = true;
    }
}

TimeSpec::TimeSpec(u64 ms) {
    query_frequency();
    time = (ms * The_Counter.QuadPart) / 1000;
}

TimeSpec now_time() {
    TimeSpec result;
    ASSERT(QueryPerformanceCounter((LARGE_INTEGER*)&result.time));
    return result;
}

uint64 TimeSpec::to_ms(void) {
    query_frequency();
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
    // @todo: is this even right ??
    ULARGE_INTEGER lhs_int, rhs_int;
    lhs_int.QuadPart = lhs.time;
    rhs_int.QuadPart = rhs.time;

    FILETIME lhs_time, rhs_time;
    lhs_time.dwHighDateTime = lhs_int.HighPart;
    lhs_time.dwLowDateTime  = lhs_int.LowPart;

    rhs_time.dwHighDateTime = rhs_int.HighPart;
    rhs_time.dwLowDateTime  = rhs_int.LowPart;

    return CompareFileTime(&rhs_time, &lhs_time);
}


#elif OS_LINUX

TimeSpec::TimeSpec(u64 ms) {
    time.tv_sec = ms / 1000;
    time.tv_nsec = (ms % 1000) * 1000;
}

TimeSpec now_time() {
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

TimeSpec operator+(const TimeSpec &lhs, const TimeSpec &rhs) {
    TimeSpec result;

    constexpr i64 billion = 1000000000;

    i64 sec = lhs.time.tv_sec + rhs.time.tv_sec;
    i64 nsec = lhs.time.tv_nsec + rhs.time.tv_nsec;
    if (nsec >= billion) {
        nsec -= billion;
        sec++;
    }

    result.time.tv_sec = sec;
    result.time.tv_nsec = nsec;
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