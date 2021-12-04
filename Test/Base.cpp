#include "Base.h"
#include "Log.h"
#include "Console.h"
#include "Str.h"

int TestUnit::run() {

    Console::set_color(Console::Output, ConsoleColor::Yellow + ConsoleColor::Bold);
    PRINTLN("Running Unit: \'" + get_name() + "\'");
    Console::set_color(Console::Output);

    for (TestCase *test_case : cases) {
        test_case->result = test_case->run();
    }

    bool successful = true;

    for (TestCase *test_case : cases) {

        ConsoleColor::Style color = test_case->result.passed
            ? ConsoleColor::Green
            : ConsoleColor::Red + ConsoleColor::Bold;

        Str status = test_case->result.passed
            ? STATIC_STR("PASSED")
            : STATIC_STR("FAILED");

        PRINT("[");
        Console::set_color(Console::Output, color);
        PRINT(status);
        Console::set_color(Console::Output);
        PRINT("]");

        PRINTLN(": \'" + test_case->get_desc() + "\'");
        if (!test_case->result.passed) {
            Console::set_color(Console::Output, ConsoleColor::White + ConsoleColor::Underline);
            PRINTLN("          " + test_case->result.reason);
            Console::set_color(Console::Output);
        }

        successful = successful && test_case->result.passed;
    }

    return successful;
}
