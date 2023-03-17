#include "Types.h"

#include "FileSystem/Extras.h"
#include "Memory/Arena.h"
#include "Tape.h"
#include "Test/Test.h"

TEST_CASE(
    "Lib/Types/Parsing",
    "positive integer, "
    "parse_i64, "
    "parses without positive/negative symbol")
{
    Str s = LIT("1000");

    RawTape tape((Raw)s);

    i64 result = 0;
    REQUIRE(parse<i64>(&tape, result), "");
    REQUIRE(result == 1000, "");

    return MPASSED();
}

namespace TestEnum {
    enum Type : u32
    {
        Zero = 0,
        One,
        Two,
        Three,
        Count
    };
}
typedef TestEnum::Type ETestEnum;

PROC_FMT_ENUM(TestEnum, {
    FMT_ENUM_CASE(TestEnum, Zero);
    FMT_ENUM_CASE(TestEnum, One);
    FMT_ENUM_CASE(TestEnum, Two);
    FMT_ENUM_CASE(TestEnum, Three);
    FMT_ENUM_DEFAULT_CASE_UNREACHABLE();
})

PROC_PARSE_ENUM(TestEnum, {
    PARSE_ENUM_CASE(TestEnum, Zero);
    PARSE_ENUM_CASE(TestEnum, One);
    PARSE_ENUM_CASE(TestEnum, Two);
    PARSE_ENUM_CASE(TestEnum, Three);
})

TEST_CASE("Lib/Types/Parsing", "Format & Parse simple enum works")
{
    auto test_strings =
        arr<Str>(LIT("Zero"), LIT("One"), LIT("Two"), LIT("Three"));

    for (u32 i = 0; i < test_strings.count(); ++i) {
        RawTape rt(test_strings[i]);

        ETestEnum test;
        REQUIRE(parse(&rt, test), "");

        REQUIRE(test == i, "");
    }

    return MPASSED();
}