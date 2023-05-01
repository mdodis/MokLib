#include "Base.h"

#include "Console.h"
#include "FileSystem/Extras.h"
#include "Log.h"
#include "Memory/Extras.h"
#include "Str.h"

static TestRunner* Test_Runner = 0;

TestCase::TestCase(Str name, Str unit, ProcTest proc)
    : name(name), unit(unit), proc(proc)
{
    get_test_runner()->add_test(*this);
}

Benchmark::Benchmark(
    Str                        name,
    Str                        unit,
    ProcBenchmark*             proc,
    const BenchmarkInputRange& input_range)
    : name(name), unit(unit), proc(proc), input_range(input_range)
{
    get_test_runner()->add_benchmark(*this);
}

int TestRunner::run_tests()
{
    u64 num_passed = 0;
    u64 num_failed = 0;
    for (TestCase& test_case : test_cases) {
        Console::set_color(Console::Output, ConsoleColor::Yellow);
        print(LIT("Running "));
        Console::set_color(Console::Output);
        print(LIT("[\"{}\"] \"{}\" ... "), test_case.unit, test_case.name);

        test_case.result = test_case.proc();

        if (test_case.result.passed) {
            Console::set_color(Console::Output, ConsoleColor::Green);
            print(LIT("Passed"));
            Console::set_color(Console::Output);
            num_passed++;
        } else {
            Console::set_color(
                Console::Output,
                ConsoleColor::Red + ConsoleColor::Bold);
            print(LIT("Failed\n\t{}"), test_case.result.reason);
            Console::set_color(Console::Output);
            num_failed++;
        }
        print(LIT("\n"));
    }

    if (num_passed == test_cases.size) {
        print(LIT("Passed {}/{}\n"), num_passed, test_cases.size);
    } else {
        print(LIT("Failed {}/{}\n"), num_passed, test_cases.size);
    }

    return 0;
}

void TestRunner::run_benchmarks()
{
    for (Benchmark& benchmark : benchmarks) {
        print(LIT("Benchmarking "));
        print(
            LIT("[\"{}\"] \"{}\" ({}-{}-{}) ... \n"),
            benchmark.unit,
            benchmark.name,
            benchmark.input_range.start,
            benchmark.input_range.step,
            benchmark.input_range.end);

        for (u64 iteration = benchmark.input_range.start;
             (iteration <= benchmark.input_range.end);
             (iteration += benchmark.input_range.step))
        {
            BenchmarkResult result = {};
            result.iteration       = iteration;
            TimeSpec begin_time    = now_time();
            benchmark.proc(result);
            TimeSpec end_time = now_time();
            float    s        = (end_time - begin_time).to_s();

            float units_per_second =
                (s <= 0.0001f) ? 0.0f : result.units_processed / s;
            print(
                LIT("Iteration {} ({} units), {}s {} units/s\n"),
                iteration,
                result.units_processed,
                s,
                units_per_second);
        }
    }
}

TestRunner* get_test_runner()
{
    if (Test_Runner == 0) {
        Test_Runner = new TestRunner();
    }

    return Test_Runner;
}