#pragma once
#include "Containers/List.h"
#include "Types.h"

/**
 * Builds strings
 *
 * @todo: possible allocation optimization - split allocation into 2 separate
 * arenas and deallocate the first one (the list stuff) upon destruction
 */
struct StringBuilder {

	_inline StringBuilder(const IAllocator& allocator)
		:allocator(allocator), list(allocator) {}

	template <typename T>
	StringBuilder &add(const T &item) {
		Node new_node;
		new_node.type = type_of(item);
		new_node.ptr = (umm)&item;
		list.append(new_node);
		return *this;
	}

	template <typename T>
	StringBuilder &operator+(const T &item) {
		return add(item);
	}

	_inline Str str() {
		uint32 sz;
		char *b = _cstr(false, &sz);
		return Str(b, sz);
	}

	TList<Str> to_list(void);

	_inline char *cstr() { return _cstr(true, 0); }

	char *_cstr(bool with_null_term, uint32 *out_size);

	struct Node {
		IType type;
		umm ptr;
	};

	IAllocator allocator;
	TList<Node> list;
};