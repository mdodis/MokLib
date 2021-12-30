#include "StringBuilder.h"

TList<Str> StringBuilder::to_list(void) {
	TList<Str> output(allocator);
	LIST_FOR_EACH(&list, iter) {
		iter->data.type.stringify(iter->data.ptr, output, *output.alloc);
	}
	return output;
}


char *StringBuilder::_cstr(bool with_null_term, u64 *out_size) {

	TList<Str> output = to_list();

	u64 total_size = 0;
	LIST_FOR_EACH(&output, iter) {
		total_size += iter->data.len;
	}

	char *buffer = (char*)allocator->reserve(total_size + with_null_term);

	u64 offset = 0;
	LIST_FOR_EACH(&output, iter) {
		memcpy(buffer + offset, iter->data.data, iter->data.len);
		offset += iter->data.len;
	}

	if (with_null_term) {
		buffer[total_size] = '\0';
	}

	if (out_size) {
		*out_size = total_size;
	}

	return buffer;
}
