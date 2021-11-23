#include "Base.h"
#include "Log.h"

int TestUnit::run() {
    for (TestCase *test_case : cases) {
        // test_case->return_code = test_case->run();
        PRINT(test_case->get_desc());
    }

    return 1;
}
