#pragma once
#include "Config.h"
#include "Containers/Array.h"
#include "Containers/Map.h"
#include "Memory/Base.h"
#include "Str.h"
#include <functional>

struct MOKLIB_API TestResult {
    bool passed;
    Str reason;
};

#define PROC_TEST(name) TestResult name()
typedef PROC_TEST(ProcTest);

struct MOKLIB_API TestCase {
    TestCase(Str name, Str unit, ProcTest *proc);
    Str name, unit;
    ProcTest *proc;
    TestResult result;
};

struct MOKLIB_API TestRunner {

    TestRunner(IAllocator &allocator)
        : allocator(allocator)
        , test_cases(&allocator)
        {}


    void add_test(TestCase &test_case) {
        test_cases.add(test_case);
    }

    int run();

    IAllocator &allocator;
    TArray<TestCase> test_cases;
};

MOKLIB_API void init_test_runner();
MOKLIB_API TestRunner *get_test_runner();
