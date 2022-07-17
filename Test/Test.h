#include "Base.h"
#include "Str.h"
#include "StringFormat.h"

#define TEST_CASE_UNIQUE_NAME_IMPL() MJOIN3(UID,__LINE__,__COUNTER__)
#define TEST_CASE_UNIQUE_NAME() TEST_CASE_UNIQUE_NAME_IMPL()

#define TEST_CASE_IMPL(id, tname, tunit)          \
    static TestResult MJOIN2(TestCaseProc_,id)(); \
    static TestCase MJOIN2(TestCase_,id) = {      \
        LIT(tname),                               \
        LIT(tunit),                               \
        MJOIN2(TestCaseProc_,id),                 \
    };                                            \
    TestResult MJOIN2(TestCaseProc_,id)()

#define TEST_CASE(unit, name) \
    TEST_CASE_IMPL(TEST_CASE_UNIQUE_NAME(), name, unit)

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
