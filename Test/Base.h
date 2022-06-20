#pragma once
#include "Config.h"
#include "Containers/Array.h"
#include "Memory/Base.h"
#include "Str.h"

struct MOKLIB_API TestResult {
    bool passed;
    Str reason;
};

struct MOKLIB_API TestUnit {
    TArray<struct TestCase*> *cases;

    TestUnit() {}

    bool is_initialized = false;
    void add(struct TestCase* c) {
        if (!is_initialized) {
            cases = new TArray<struct TestCase*>(&System_Allocator);
            is_initialized = true;
        }
        cases->add(c);
    }
    int run();

    virtual Str get_name() = 0;
};

struct MOKLIB_API TestCase {
    TestResult result;
    virtual TestResult run() = 0;
    virtual Str get_desc() = 0;
};