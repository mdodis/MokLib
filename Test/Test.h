#include "Base.h"
#include "Str.h"
#include "StringFormat.h"

#define TEST_CASE_UNIQUE_NAME_IMPL() \
    MJOIN3(TEST_CASE_UID, __LINE__, __COUNTER__)
#define TEST_CASE_UNIQUE_NAME() TEST_CASE_UNIQUE_NAME_IMPL()

#define TEST_CASE_IMPL(id, tname, tunit)           \
    static TestResult MJOIN2(TestCaseProc_, id)(); \
    static TestCase   MJOIN2(TestCase_, id) = {    \
        LIT(tname),                              \
        LIT(tunit),                              \
        MJOIN2(TestCaseProc_, id),               \
    };                                           \
    TestResult MJOIN2(TestCaseProc_, id)()

#define TEST_CASE(unit, name) \
    TEST_CASE_IMPL(TEST_CASE_UNIQUE_NAME(), name, unit)

#define STR(s) #s
#define CNCAT4(a, b, c, d) a b c STR(d)
#define MPASSED() (TestResult{true})
#define MFAILED(desc) (TestResult{false, LIT(desc)})

#define REQUIRE(expr, msg)                              \
    do {                                                \
        if (!(expr)) {                                  \
            return TestResult{                          \
                false,                                  \
                format(                                 \
                    System_Allocator,                   \
                    LIT("'{}' failed (At {}:{}) - {}"), \
                    LIT(MSTR(expr)),                    \
                    LIT(__FILE__),                      \
                    LIT(MSTR(__LINE__)),                \
                    LIT(msg))};                         \
        }                                               \
    } while (0)

#define BENCHMARK_UNIQ_NAME_IMPL() MJOIN3(BMARK_CASE_UID, __LINE__, __COUNTER__)
#define BENCHMARK_UNIQ_NAME() BENCHMARK_UNIQ_NAME_IMPL()

#define BENCHMARK_IMPL(id, unit, name, begin, step, end)                      \
    static void MJOIN2(BenchmarkProc_, id)(BenchmarkResult & result);         \
    static BenchmarkInputRange MJOIN2(BenchmarkIR_, id) = {begin, step, end}; \
    static Benchmark           MJOIN2(Benchmark_, id)   = {                   \
        LIT(name),                                                \
        LIT(unit),                                                \
        MJOIN2(BenchmarkProc_, id),                               \
        MJOIN2(BenchmarkIR_, id),                                 \
    };                                                            \
    void MJOIN2(BenchmarkProc_, id)(BenchmarkResult & result)

#define BENCHMARK(unit, name, begin, step, end) \
    BENCHMARK_IMPL(BENCHMARK_UNIQ_NAME(), unit, name, begin, step, end)
