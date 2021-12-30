#include "Arena.h"
#include "Debugging/Base.h"
#include "Math/Base.h"
#include <string.h>

struct _ArenaBlockHdr {
	umm memory;
	u64 capacity;
	u64 used;
	u64 last_offset;
};

#define GET_ARENA_BLOCK_HEADER(base) (_ArenaBlockHdr*)(base)

Arena::Arena(IAllocator *base, u64 size) : base(base) {
	size += sizeof(_ArenaBlockHdr);

	capacity = size;
	min_block_size = size;
	memory = base->reserve(size);
	used = 0;
	last_offset = 0u;

	_ArenaBlockHdr *hdr = (_ArenaBlockHdr*)push(sizeof(_ArenaBlockHdr));
	hdr->memory = 0;
}

umm Arena::push(uint64 size) {
	umm result = 0;

	if ((used + size) > capacity) {
		if (!stretch(size)) {
			return 0;
		}
	}

	result = memory + used;
	last_offset = used;
	used += size;

	return result;
}

PROC_MEMORY_RESIZE(Arena::resize) {
	umm last_ptr = memory + last_offset;

	if (ptr == 0) return push(new_size);
	if (new_size <= prev_size) return ptr;

	if ((last_ptr == ptr) && ((last_offset + new_size) <= capacity)) {

		umm result = memory + last_offset;
		used = last_offset + new_size;
		return result;
	} else {

		if ((used + new_size) > capacity) {
			if (!stretch(used + new_size)) {
				return 0;
			}
		}

		umm new_ptr = push(new_size);
		memcpy(new_ptr, ptr, prev_size);
		return new_ptr;
	}
}


PROC_MEMORY_RELEASE(Arena::release) {
	umm last_ptr = memory + last_offset;

	if (last_ptr == ptr) {
		used = last_offset;
		last_offset = 0;
	}
}

bool Arena::stretch(u64 required_size) {

	if (min_block_size == 0)
		return false;

	const u64 new_block_size = max(min_block_size, required_size)
		+ sizeof(_ArenaBlockHdr);

	// Save current block variables
	const umm prev_memory      = memory;
	const u64 prev_capacity    = capacity;
	const u64 prev_used        = used;
	const u64 prev_last_offset = last_offset;

	const umm new_memory = base->reserve(new_block_size);
	if (!new_memory) {
		return false;
	}
	memory      = new_memory;
	capacity    = new_block_size;
	used        = 0;
	last_offset = 0;

	// Store to new block header
	_ArenaBlockHdr *header = (_ArenaBlockHdr*)push(sizeof(_ArenaBlockHdr));
	header->memory 		= prev_memory;
	header->capacity    = prev_capacity;
	header->used        = prev_used;
	header->last_offset = prev_last_offset;

	return true;
}


PROC_MEMORY_RELEASE_BASE(Arena::release_base) {

	if (min_block_size == 0) {
		base->release(memory);
		return;
	}

	auto *header = GET_ARENA_BLOCK_HEADER(memory);
	umm last_memory = header->memory;
	base->release(memory);

	while (last_memory != 0) {
		header = GET_ARENA_BLOCK_HEADER(last_memory);
		const umm memory_to_delete = last_memory;
		last_memory = header->memory;
		base->release(memory_to_delete);
	}

}

umm Arena::get_block_data() {
	return memory + sizeof(_ArenaBlockHdr);
}