#pragma once
#include "../Memory/Base.h"
#include "../Base.h"
#include "../Debugging.h"
#include "Memory/Arena.h"
#include <iterator>
#include <cstddef>

template <typename T>
struct TArray {

	_inline TArray(void) {
		this->data = 0;
		this->capacity = 0;
		this->size = 0;
	}

	_inline TArray(const IAllocator &alloc) {
		this->alloc = alloc;
		this->data = 0;
		this->capacity = 0;
		this->size = 0;
	}

	int32 add(const T &item) {
		if (!data) {
			init(Init_Capacity);
		}

		if (capacity == (size + 1)) {
			strech();
		}

		data[size++] = item;
		return size - 1;
	}

	T *add() {
		if (!data) {
			init(Init_Capacity);
		}

		if (capacity == (size + 1)) {
			strech();
		}

		size++;

		return &data[size - 1];
	}

	void del(int32 index, bool keep_order = false) {
		if (!is_index_valid(index))
			return;

		if (keep_order) {
			UNIMPLEMENTED();
		} else {

			data[index] = data[size - 1];
			size--;
		}
	}

	void empty() {
		size = 0;
	}

	void init(int32 amount) {
		data = (T*)alloc.reserve(alloc.context, amount * sizeof(T));
		capacity = amount;
		size = 0;
	}

	void strech() {
		data = (T*)alloc.resize(alloc.context, (umm)data, capacity * sizeof(T), capacity * 2 * sizeof(T));
		capacity *= 2;
	}

	_inline bool is_index_valid(int32 i) {
		return (i >= 0) && (i < size);
	}

	_inline T &operator[](int32 idx) {
		ASSERT(is_index_valid(idx));
		return data[idx];
	}

	_inline const T &operator[](int32 idx) const {
		ASSERT(is_index_valid(idx));
		return data[idx];
	}

	T *data;
	IAllocator alloc;
	int32 capacity, size;

	static constexpr int32 Init_Capacity = 4;


	/** Iterators */
	struct Iterator {
		using iterator_category = std::forward_iterator_tag;
		using difference_type   = std::ptrdiff_t;
		using Val = T;
		using Ptr = T*;
		using Ref = T&;

		Iterator(T *data, int32 curr_index) : data(data), curr_index(curr_index) {}

		Ref operator*()  const { return data[curr_index]; }
		Ptr operator->() const { return data + curr_index; }

		Iterator &operator++() {
			curr_index++;
			return *this;
		}

		friend bool operator==(const Iterator &a, const Iterator &b) {
			return a.curr_index == b.curr_index;
		}

		friend bool operator!=(const Iterator &a, const Iterator &b) {
			return a.curr_index != b.curr_index;
		}

	private:
		T *data;
		int32 curr_index;
	};

	Iterator begin() { return Iterator(data, 0); }
	Iterator end()   { return Iterator(data, size); }
};

template <typename T, uint32 Count>
struct TInlineArray : TArray<T> {
	TInlineArray() :TArray<T>(alloc_arena.to_alloc()) {

	}

	uint8 memory[(sizeof(T) * Count)];
	Arena alloc_arena = {
	    IAllocator{0},
	    memory,
	    (sizeof(T) * Count),
	    0,
	};

};