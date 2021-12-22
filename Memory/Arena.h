#pragma once

#include "Base.h"

struct Arena {
	static constexpr u64 Default_Block_Size = MEGABYTES(1);
	static constexpr u64 Remaining 			= ~0ull;

	static Arena create(IAllocator base, u64 size = Default_Block_Size);

	umm push(u64 size);
	umm resize(umm ptr, u64 prev_size, u64 new_size);
	void release(umm ptr);
	void release_base();
	IAllocator to_alloc();

	bool stretch(u64 required_size);

	umm get_block_data();

	IAllocator base;
	umm memory;
	u64 capacity, used;
	u64 min_block_size;

	u64 last_offset = 0;
};

#define CREATE_SCOPED_ARENA(base, name, size) \
	Arena name = Arena::create(base, (size)); \
	ArenaScope name__##scope(&name);

#define CREATE_INLINE_ARENA(name, size) \
	static uint8 memory_of__##name[(size)]; \
	Arena name = { \
		Null_Allocator, \
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

#define SAVE_ARENA(arena) ArenaSave MCONCAT(_arena_save, __LINE__) (arena)

struct ArenaSave {
	Arena *arena;
	u64 capacity, used, last_offset;
	ArenaSave(Arena *a) {
		arena = a;
		capacity    = arena->capacity;
		used        = arena->used;
		last_offset = arena->last_offset;
	}

	~ArenaSave() {
		arena->capacity    = capacity;
		arena->used        = used;
		arena->last_offset = last_offset;
	}
};