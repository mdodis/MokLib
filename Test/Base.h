#pragma once
#include "Containers/Array.h"
#include "Memory/Base.h"
#include "Str.h"

struct TestResult {
    bool passed;
    Str reason;
};

struct TestUnit {
    TArray<struct TestCase*> cases;

    TestUnit() {
        cases.alloc = get_system_allocator();
    }

    int run();

    virtual Str get_name() = 0;
};

struct TestCase {
    TestResult result;
    virtual TestResult run() = 0;
    virtual Str get_desc() = 0;
};