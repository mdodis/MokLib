#include "Console.h"
#include "Test/Test.h"

DEFTEST_UNIT(BasicUnit);

DEFTEST_CASE(Add1And1, BasicUnit, "Make sure that 1 + 1 == 2", {
    return MPASSIF((1 + 1) == 2);
});

int main(int argc, char *argv[]) {
    return TEST_UNIT(BasicUnit).run();
}

#include "Test/Compile.inc"
#include "Compile.inc"
