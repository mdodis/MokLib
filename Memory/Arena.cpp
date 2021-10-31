#include "Arena.h"
#include <string.h>

Arena Arena::create(IAllocator base, uint64 size) {
	Arena result = {};
	result.base = base;
	result.capacity = size;
	result.memory = base.reserve(base.context, size);
	result.last_offset = 0u;
	return result;
}

umm Arena::push(uint64 size) {
	umm result = 0;
	if (used + size <= capacity) {
		result = memory + used;
		last_offset = used;
		used += size;
	}

	return result;
}

umm Arena::resize(umm ptr, uint64 prev_size, uint64 new_size) {
	umm last_ptr = memory + last_offset;

	if (ptr == 0) return push(new_size);
	if (new_size <= prev_size) return ptr;

	if ((last_ptr == ptr) && ((last_offset + prev_size) >= used)) {
		umm result = memory + last_offset;
		used = last_offset;
		used += new_size;
		return last_ptr;
	} else {

		umm new_ptr = push(new_size);
		memcpy(new_ptr, ptr, prev_size);
		return new_ptr;
	}
}


void Arena::release(umm ptr) {
	umm last_ptr = memory + last_offset;

	if (last_ptr == ptr) {
		used = last_offset;
		last_offset = 0;
	}
}

void Arena::release_base() {
	base.release(base.context, memory);
}

// IAllocator interface
PROC_MEMORY_RESERVE(arena_reserve) {
	Arena *arena = (Arena*)context;
	return arena->push(size);
}

PROC_MEMORY_RESIZE(arena_resize) {
	Arena *arena = (Arena*)context;
	return arena->resize(ptr, prev_size, new_size);
}

PROC_MEMORY_RELEASE(arena_release) {
	Arena *arena = (Arena*)context;
	arena->release(ptr);
}

PROC_MEMORY_RELEASE_BASE(arena_release_base) {
	Arena *arena = (Arena*)context;
	arena->release_base();
}

IAllocator Arena::to_alloc() {
	return IAllocator {
		arena_reserve,
		arena_resize,
		arena_release,
		arena_release_base,
		this
	};
}
