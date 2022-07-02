#include "Tests.h"
#include "Thread/ThreadContext.h"

IMPL_TEST_UNIT(CacheTestUnit);
IMPL_TEST_UNIT(TapeTestUnit);

int main(int argc, char **argv) {
    {
        ThreadContextBase::setup();
        BOOTSTRAP_THREAD(SimpleThreadContext);
    }

    return
        !TEST_UNIT(CacheTestUnit).run() &&
        !TEST_UNIT(TapeTestUnit).run();
}