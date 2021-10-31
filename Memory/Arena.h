#pragma once

#include "Base.h"

struct Arena {
	static constexpr uint64 Default_Size = MEGABYTES(1);

	static Arena create(IAllocator base, uint64 size = Default_Size);

	umm push(uint64 size);
	umm resize(umm ptr, uint64 prev_size, uint64 new_size);
	void release(umm ptr);
	void release_base();
	IAllocator to_alloc();

	IAllocator base;
	umm memory;
	uint64 capacity, used;

	uint64 last_offset = 0;
};

#define CREATE_SCOPED_ARENA(base, name, size) \
	Arena name = Arena::create(base, (size)); \
	ArenaScope name__##scope(&name);

#define CREATE_INLINE_ARENA(name, size) \
	static uint8 memory_of__##name[(size)]; \
	Arena name = { \
		IAllocator{0}, \
		memory_of__##name, \
		(size), \
		0, \
	};

struct ArenaScope {
	Arena *arena;
	ArenaScope(Arena *a) {
		arena = a;
	}

	~ArenaScope() {
		arena->release_base();
	}
};
