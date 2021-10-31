#include "Base.h"
#include <stdlib.h>

PROC_MEMORY_RESERVE(system_allocator_reserve);
PROC_MEMORY_RESIZE(system_allocator_resize);
PROC_MEMORY_RELEASE(system_allocator_release);
PROC_MEMORY_RELEASE_BASE(system_allocator_release_base);

IAllocator get_system_allocator(void) {
	static IAllocator system_allocator = {
		system_allocator_reserve,
		system_allocator_resize,
		system_allocator_release,
		system_allocator_release_base,
	};

	return system_allocator;
}

PROC_MEMORY_RESERVE(system_allocator_reserve) {
	return (umm)malloc(size);
}

PROC_MEMORY_RESIZE(system_allocator_resize) {
	return (umm)realloc(ptr, new_size);
}

PROC_MEMORY_RELEASE(system_allocator_release) {
	free(ptr);
}

PROC_MEMORY_RELEASE_BASE(system_allocator_release_base) {
	return;
}
