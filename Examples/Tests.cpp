#pragma once
#include "Console.h"
#include "Test/Test.h"
#include <corecrt_wstdio.h>

DEFTEST_UNIT(BasicUnit);

DEFTEST_CASE(Add1And1, BasicUnit, "Make sure that 1 + 1 == 2", {
    return (1 + 1) == 2;
});

int main(int argc, char *argv[]) {
    Console::set_color(Console::Output, ConsoleColor::Red);
    puts("hello, world!");
    Console::set_color(Console::Output);
    return TEST_UNIT(BasicUnit).run();
}

#include "Test/Compile.inc"
#include "Compile.inc"