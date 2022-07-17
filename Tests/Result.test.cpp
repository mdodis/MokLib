#include "Test/Test.h"
#include "Str.h"
#include "Result.h"

Result<Str, Str> do_thingy(bool fail) {
    if (!fail) {
        return Ok(LIT("All good"));
    } else {
        return Err(LIT("Pretty bad"));
    }
}

TEST_CASE("Lib/Result", "test") {

    auto str = do_thingy(false).unwrap();
    REQUIRE(str == LIT("All good"), "");

    auto result = do_thingy(true);
    REQUIRE(!result.ok(), "");
    REQUIRE(result.err() == LIT("Pretty bad"), "");

    return MPASSED();
}