#include "Containers/Map.h"

#include "Test/Test.h"

TEST_CASE("Lib/Containers/Map", "Map iteration")
{
    TMap<Str, int> map(System_Allocator);

    map.add(LIT("hello"), 1);
    map.add(LIT("world"), 2);

    auto begin = map.begin();
    auto end   = map.end();

    bool hello_found = false;
    bool world_found = false;

    for (auto p = begin; p != end; ++p) {
        auto pair = *p;
        if (pair.key == LIT("hello")) {
            REQUIRE(pair.val == 1, "");
            hello_found = true;
        }

        if (pair.key == LIT("world")) {
            REQUIRE(pair.val == 2, "");
            world_found = true;
        }
    }

    REQUIRE(hello_found, "");
    REQUIRE(world_found, "");

    return MPASSED();
}

TEST_CASE("Lib/Containers/Map", "Map iteration 2")
{
    TMap<Str, int> map(System_Allocator);

    map.add(LIT("hello"), 1);

    auto begin = map.begin();

    bool hello_found = false;

    for (auto p = begin; p != map.end(); ++p) {
        auto pair = *p;

        REQUIRE(!hello_found, "");
        REQUIRE(pair.key == LIT("hello"), "");
        REQUIRE(pair.val == 1, "");
        hello_found = true;
    }

    REQUIRE(hello_found, "");

    return MPASSED();
}
