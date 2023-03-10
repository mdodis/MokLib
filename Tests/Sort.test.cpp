#include "Sort.h"

#include "Containers/Array.h"
#include "FileSystem/Extras.h"
#include "Test/Test.h"

TEST_CASE("Lib/Sort", "Quicksort integers")
{
    auto a = arr<int>(5, 6, 10, 9, 2, 3, 1, 4, 8, 7);

    auto a_slice = slice(a);

    sort::quicksort(a_slice);

    for (int i = 0; i < 10; ++i) {
        REQUIRE(a_slice[i] == (i + 1), "");
    }

    return MPASSED();
}
