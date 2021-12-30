#pragma once
#include "../Memory/Base.h"
#include "../Base.h"
#include "../Debugging/Base.h"
#include "Memory/Arena.h"
#include <iterator>
#include <cstddef>
#include <initializer_list>

template <typename T>
struct TArray {

	_inline TArray(void) {
		this->data = 0;
		this->capacity = 0;
		this->size = 0;
	}

	_inline TArray(IAllocator *alloc) {
		this->alloc = alloc;
		this->data = 0;
		this->capacity = 0;
		this->size = 0;
	}

	_inline TArray(IAllocator *alloc, std::initializer_list<T> init_list) : TArray(alloc) {
		for (const T &elem : init_list) {
			add(elem);
		}
	}

	int32 add(const T &item) {
		if (!data) {
			if (!init(Init_Capacity)) return -1;
		}

		if (capacity == (size)) {
			if (!stretch()) return -1;
		}

		data[size++] = item;
		return size - 1;
	}

	T *add() {
		if (!data) {
			if (!init(Init_Capacity)) return 0;
		}

		if (capacity == (size)) {
			if (!stretch()) return 0;
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

	void release() {
		empty();
		alloc->release((umm)data);
	}

	bool init(int32 amount) {
		data = (T*)alloc->reserve(amount * sizeof(T));
		if (!data) {
			return false;
		}

		capacity = amount;
		size = 0;
		return true;
	}

	bool stretch() {
		T *new_data = (T*)alloc->resize((umm)data, capacity * sizeof(T), capacity * 2 * sizeof(T));
		if (!new_data) return false;
		data = new_data;
		capacity *= 2;
		return true;
	}

	T *last() const {
		if (size > 0)
			return ((T*)data) + (size - 1);
		else
			return 0;
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

	i32 index_of(const T &query) const {
		for (i32 i = 0; i < size; ++i) {
			if (query == data[i]) {
				return i;
			}
		}

		return -1;
	}

	T *data;
	IAllocator *alloc;
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

	Iterator begin() const { return Iterator(data, 0); }
	Iterator end()   const { return Iterator(data, size); }
};

template <typename T, uint32 Count>
struct TInlineArray : TArray<T> {
	TInlineArray()
		: alloc_arena(memory, (sizeof(T) * Count))
		, TArray<T>(&alloc_arena)
	{ this->init(Count); }

	TInlineArray(std::initializer_list<T> init_list) : TInlineArray() {
		for (const T &elem : init_list) {
			add(elem);
		}
	}

	u8 memory[(sizeof(T) * Count)];
	Arena alloc_arena;

};