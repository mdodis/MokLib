#pragma once
#include "../Config.h"
#include "Base.h"
#include "Defer.h"

struct MOKLIB_API Arena : IAllocator {
	static constexpr u64 Default_Block_Size = MEGABYTES(1);
	static constexpr u64 Remaining 			= ~0ull;

	Arena() {}
	constexpr Arena(umm data, u64 size)
		: base(&Null_Allocator)
		, memory(data)
		, used(0)
		, capacity(size)
		, min_block_size(0) {}
	Arena(IAllocator *base, u64 size = Default_Block_Size);

	~Arena() {

	}

	virtual _inline PROC_MEMORY_RESERVE(reserve) override {
	    return push(size);
	}

	virtual PROC_MEMORY_RESIZE(resize) override;
	virtual PROC_MEMORY_RELEASE(release) override;
	virtual PROC_MEMORY_RELEASE_BASE(release_base) override;

	umm push(u64 size);
	IAllocator to_alloc();

	bool stretch(u64 required_size);

	umm get_block_data();

	IAllocator *base;
	umm memory;
	u64 capacity, used;
	u64 min_block_size;

	u64 last_offset = 0;
};

#define CREATE_SCOPED_ARENA(base, name, size) \
	Arena name = Arena(base, (size)); \
	DEFER(name.release_base());

#define CREATE_GLOBAL_ARENA(name, size) \
	static uint8 memory_of__##name[(size)]; \
	static Arena name = { \
		Null_Allocator, \
		memory_of__##name, \
		(size), \
		0, \
	};

#define CREATE_INLINE_ARENA(name, size) \
	static uint8 memory_of__##name[(size)]; \
	Arena name(memory_of__##name, size);

#define SAVE_ARENA(arena) ArenaSave MCONCAT(_arena_save, __LINE__) (arena)

struct MOKLIB_API ArenaSave {
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