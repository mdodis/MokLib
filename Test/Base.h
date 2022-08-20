#pragma once
#include "Config.h"
#include "Containers/Array.h"
#include "Containers/Map.h"
#include "Memory/Base.h"
#include "Str.h"
#include <functional>
#include "Time/Time.h"

struct MOKLIB_API TestResult {
    bool passed;
    Str reason;
};

namespace BenchmarkUnit {
    enum Type {
        None = 0,
        Bytes,
    };
};
typedef BenchmarkUnit::Type EBenchmarkUnit;

struct MOKLIB_API BenchmarkInputRange {
    u64 start;
    u64 step;
    u64 end;
};

struct MOKLIB_API BenchmarkResult {
    EBenchmarkUnit unit;
    u64 iteration;
    u64 units_processed;
    TimeSpec elapsed_time;
};

#define PROC_TEST(name) TestResult name()
typedef PROC_TEST(ProcTest);

#define PROC_BENCHMARK(name) void name(BenchmarkResult &result);
typedef PROC_BENCHMARK(ProcBenchmark);

struct MOKLIB_API TestCase {
    TestCase(Str name, Str unit, ProcTest *proc);
    Str name, unit;
    ProcTest *proc;
    TestResult result;
};

struct MOKLIB_API Benchmark {
    Benchmark(
        Str name,
        Str unit,
        ProcBenchmark *proc,
        const BenchmarkInputRange &input_range);
    Str name, unit;
    ProcBenchmark *proc;
    BenchmarkResult result;
    BenchmarkInputRange input_range;
};

struct MOKLIB_API TestRunner {

    TestRunner(IAllocator &allocator)
        : allocator(allocator)
        , test_cases(&allocator)
        , benchmarks(&allocator)
        {}


    void add_test(TestCase &test_case) {
        test_cases.add(test_case);
    }

    void add_benchmark(Benchmark &benchmark) {
        benchmarks.add(benchmark);
    }

    int run_tests();
    void run_benchmarks();

    IAllocator &allocator;
    TArray<TestCase> test_cases;
    TArray<Benchmark> benchmarks;
};

MOKLIB_API TestRunner *get_test_runner();
