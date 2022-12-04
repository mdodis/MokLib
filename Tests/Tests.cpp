#include "Test/Test.h"
#include "Thread/ThreadContext.h"
#include "FileSystem/Extras.h"

int main(int argc, char **argv) {
    {
        ThreadContextBase::setup();
        BOOTSTRAP_THREAD(SimpleThreadContext);
    }

    print(LIT("Running tests {} yo\n"), LIT("haha"));

    int result = get_test_runner()->run_tests();
    if (result != 0) {
        print(LIT("{} Tests failed\n"), result);
        return result;
    }

    get_test_runner()->run_benchmarks();
}