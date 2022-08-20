#include "Test/Test.h"
#include "Tape.h"
#include "Memory/Arena.h"
#include "Types.h"
#include "FileSystem/Extras.h"

TEST_CASE("Lib/Types/Parsing",
    "positive integer, "
    "parse_i64, "
    "parses without positive/negative symbol") {

    Str s = LIT("1000");

    RawTape tape((Raw)s);

    i64 result = 0;
    REQUIRE(parse<i64>(&tape, result), "");
    REQUIRE(result == 1000, "");

    return MPASSED();
}
