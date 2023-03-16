#include "Str.h"

#include "Test/Test.h"

TEST_CASE("Lib/Str", "'hello', chop_right_last_of('llo'), 'he'")
{
    Str text   = LIT("hello");
    Str result = text.chop_right_last_of(LIT("llo"));
    REQUIRE(result == LIT("he"), "");

    return MPASSED();
}
