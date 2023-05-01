#include "Arg.h"

#include "Containers/Array.h"
#include "Memory/Arena.h"
#include "Test/Test.h"

TEST_CASE(
    "Lib/Arg", "{-param1 \"hello world\" -p2 2}, Invoked, Parses correctly")
{
    CREATE_SCOPED_ARENA(System_Allocator, temp, KILOBYTES(1));

    TArr<Str, 4> params = {
        LIT("-param1"),
        LIT("hello world"),
        LIT("-p2"),
        LIT("2"),
    };

    ArgCollection collection(temp);
    collection.register_arg<Str>(LIT("param1"), LIT(""));
    collection.register_arg<u32>(LIT("p2"), 0);

    collection.parse_args(slice(params));

    REQUIRE(
        *(collection.get_arg<Str>(LIT("param1"))) == LIT("hello world"),
        "");
    REQUIRE(*(collection.get_arg<u32>(LIT("p2"))) == 2, "");

    return MPASSED();
}

TEST_CASE("Lib/Arg", "{-param1 2 unnamed_arg}, Invoked, Parses correctly")
{
    CREATE_SCOPED_ARENA(System_Allocator, temp, KILOBYTES(1));

    TArr<Str, 3> params = {
        LIT("-param1"),
        LIT("2"),
        LIT("unnamed_arg"),
    };

    ArgCollection collection(temp);
    collection.register_arg<u32>(LIT("param1"), 0);
    collection.register_arg<Str>(LIT("_"), LIT(""));

    collection.parse_args(slice(params));

    REQUIRE(*(collection.get_arg<u32>(LIT("param1"))) == 2, "");
    REQUIRE(*(collection.get_arg<Str>(LIT("_"))) == LIT("unnamed_arg"), "");

    return MPASSED();
}