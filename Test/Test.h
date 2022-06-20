#include "Base.h"
#include "Str.h"
#include "StringFormat.h"

#define DEFTEST_UNIT(name)                                     \
    struct MCONCAT(TestUnitDecl_, name) : public TestUnit {    \
        Str get_name() override {                              \
            return LIT(#name);                                 \
        }                                                      \
    };                                                         \
    extern MCONCAT(TestUnitDecl_,name) MCONCAT(TestUnit_,name)

#define IMPL_TEST_UNIT(name) MCONCAT(TestUnitDecl_,name) MCONCAT(TestUnit_,name)
#define TEST_UNIT(name) MCONCAT(TestUnit_,name)

#define TEST_CASE_NAME(unit) MCONCAT(unit,TestCaseDecl_)__LINE__

#define TEST_CASE(unit, desc)                           \
    struct TEST_CASE_NAME(unit) : public TestCase {     \
        TEST_CASE_NAME(unit) () {                       \
            TEST_UNIT(unit).add(this);                  \
        }                                               \
        TestResult run() override;                      \
        Str get_desc() override { return LIT(desc); }   \
    };                                                  \
    TEST_CASE_NAME(unit) MCONCAT2(unit,TestCase_,name); \
    TestResult TEST_CASE_NAME(unit)::run()


#define STR(s) #s
#define CNCAT4(a,b,c,d) a b c STR(d)
#define MPASSED()       (TestResult { true })
#define MFAILED(desc)   (TestResult { false, LIT(desc)})

#define REQUIRE(expr, msg)                          \
    do {                                            \
        if (!(expr)) {                              \
            return TestResult {                     \
                false, format(                      \
                    System_Allocator,               \
                    LIT("'$' failed (At $:$) - $"), \
                    LIT(MSTR(expr)),                \
                    LIT(__FILE__),                  \
                    LIT(MSTR(__LINE__)),            \
                    LIT(msg))                       \
            };                                      \
        }                                           \
    } while (0)