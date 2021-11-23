#include "Base.h"
#include "Str.h"

#define DEFTEST_UNIT_1(x, y) x##y
#define DEFTEST_UNIT(name) \
    struct DEFTEST_UNIT_1(TestUnitDecl_, name) : public TestUnit { \
        Str get_name() override { \
            return STATIC_STR(#name); \
        } \
    }; \
    DEFTEST_UNIT_1(TestUnitDecl_, name) DEFTEST_UNIT_1(TestUnit_, name)

#define TEST_UNIT_1(x, y) x##y
#define TEST_UNIT(name) TEST_UNIT_1(TestUnit_,name)

#define DEFTEST_CASE_1(x, y) x##y
#define DEFTEST_CASE(name, unit, desc, code) \
    struct DEFTEST_CASE_1(TestCaseDecl_, name) : public TestCase { \
        DEFTEST_CASE_1(TestCaseDecl_, name)() { \
            TEST_UNIT(unit).cases.add(this); \
        } \
        int run() override code \
        Str get_desc() override { return STATIC_STR(desc); }\
    }; \
    DEFTEST_CASE_1(TestCaseDecl_, name) DEFTEST_CASE_1(TestCase_, name)