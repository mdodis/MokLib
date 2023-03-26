#include "Containers/Map.h"

#include "Test/Test.h"

TEST_CASE("Lib/Containers/Map", "Map iteration")
{
    TMap<Str, int> map(System_Allocator);

    map.add(LIT("hello"), 1);
    map.add(LIT("world"), 2);

    auto begin = map.begin();
    auto end   = map.end();

    for (auto p = begin; p != end; ++p) {
        auto pair = *p;
        if (pair.key == LIT("hello")) {
            REQUIRE(pair.val == 1, "");
        }

        if (pair.key == LIT("world")) {
            REQUIRE(pair.val == 2, "");
        }
    }

    return MPASSED();
}