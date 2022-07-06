#include "Test/Test.h"
#include "Thread/ThreadContext.h"

int main(int argc, char **argv) {
    {
        ThreadContextBase::setup();
        BOOTSTRAP_THREAD(SimpleThreadContext);
    }

    return get_test_runner()->run();
}