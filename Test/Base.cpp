#include "Base.h"
#include "Log.h"
#include "Console.h"
#include "Str.h"
#include "FileSystem/Extras.h"
#include "Memory/Extras.h"

static TestRunner *Test_Runner = 0;

TestCase::TestCase(Str name, Str unit, ProcTest proc)
    : name(name)
    , unit(unit)
    , proc(proc)
{
    get_test_runner()->add_test(*this);
}

int TestRunner::run() {

    for (TestCase &test_case : test_cases) {
        Console::set_color(Console::Output, ConsoleColor::Yellow);
        print(LIT("Running "));
        Console::set_color(Console::Output);
        print(LIT("['$'] '$' ... "), test_case.unit, test_case.name);

        test_case.result = test_case.proc();

        if (test_case.result.passed) {
            Console::set_color(Console::Output, ConsoleColor::Green);
            print(LIT("Passed"));
            Console::set_color(Console::Output);
        } else {
            Console::set_color(Console::Output, ConsoleColor::Red + ConsoleColor::Bold);
            print(LIT("Failed\n\t$"), test_case.result.reason);
            Console::set_color(Console::Output);
        }
        print(LIT("\n"));
    }

    return 0;
}

TestRunner *get_test_runner() {
    if (Test_Runner == 0) {
        Test_Runner = alloc<TestRunner>(System_Allocator, System_Allocator);
    }

    return Test_Runner;
}