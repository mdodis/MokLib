#include "Tests.h"
#include "Memory/Cache.h"

TEST_CASE(CacheTestUnit, "Initialized, Single Allocation, Allocates") {

	Cache cache(System_Allocator);
	umm ptr = cache.get(1, 1024);

	REQUIRE(ptr != 0, "Must allocate");
	return MPASSED();
};