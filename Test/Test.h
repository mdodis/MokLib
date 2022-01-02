#include "Base.h"
#include "Str.h"

#define DEFTEST_UNIT_1(x, y) x##y
#define DEFTEST_UNIT(name) \
    struct DEFTEST_UNIT_1(TestUnitDecl_, name) : public TestUnit { \
        Str get_name() override { \
            return LIT(#name); \
        } \
    }; \
    DEFTEST_UNIT_1(TestUnitDecl_, name) DEFTEST_UNIT_1(TestUnit_, name)

#define TEST_UNIT_1(x, y) x##y
#define TEST_UNIT(name) TEST_UNIT_1(TestUnit_,name)

#define DEFTEST_CASE_1(x, y) x##y
#define DEFTEST_CASE(name, unit, desc) \
    struct DEFTEST_CASE_1(TestCaseDecl_, name) : public TestCase { \
        DEFTEST_CASE_1(TestCaseDecl_, name)() { \
            TEST_UNIT(unit).cases.add(this); \
        } \
        TestResult run() override; \
        Str get_desc() override { return LIT(desc); }\
    }; \
    DEFTEST_CASE_1(TestCaseDecl_, name) DEFTEST_CASE_1(TestCase_, name)

#define TEST_CASE_1(x, y) x##y
#define TEST_CASE(name) TestResult TEST_CASE_1(TestCaseDecl_, name)::run()

#define STR(s) #s
#define CNCAT4(a,b,c,d) a b c STR(d)
#define MPASSED()       (TestResult { true })
#define MFAILED(desc)   (TestResult { false, LIT(desc)})
#define MPASSIF(expr)   ((expr) ? TestResult{true}  : TestResult{false, LIT(CNCAT4("At: ",__FILE__,":",__LINE__))})
#define MFAILIF(expr) \
    do { \
        if (expr) { \
            return TestResult {false, LIT(CNCAT4("At: ",__FILE__,":",__LINE__))}; \
        } \
    }while(0)
