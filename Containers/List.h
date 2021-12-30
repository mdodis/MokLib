#pragma once
#include "../Memory/Base.h"
#include "../Memory/Extras.h"
#include <iterator>
#include <cstddef>

template <typename T>
struct TList {

	struct Node {
		T data;
		Node* next;
		Node* prev;
	};

	_inline TList(IAllocator *allocator) {
		alloc = allocator;
		root = (Node*)alloc->reserve(sizeof(Node));
		root->next = root;
		root->prev = root;
	}

	_inline T *last() {
		return &root->prev->data;
	}

	void append(T item) {
		Node *new_node = PUSH_STRUCT(alloc, Node);
		new_node->data = item;
		new_node->next = new_node->prev = new_node;
		append_node(new_node);
	}

	void append_node(Node *node) {
		node->next = root;
		node->prev = root->prev;
		root->prev->next = node;
		root->prev = node;
	}
	
	IAllocator *alloc;
	Node *root;

	/** Iterators */
	struct Iterator {
		using iterator_category = std::forward_iterator_tag;
		using difference_type   = std::ptrdiff_t;
		using Val = T;
		using Ptr = T*;
		using Ref = T&;

		Iterator(Node *c) : curr(c) {}

		Ref operator*()  const { return curr->data; }
		Ptr operator->() const { return &curr->data; }

		Iterator &operator++() {
			curr = curr->next;
			return *this;
		}

		friend bool operator==(const Iterator &a, const Iterator &b) { return a.curr == b.curr; }
		friend bool operator!=(const Iterator &a, const Iterator &b) { return a.curr != b.curr; }
	private:
		Node *curr;
	};

	Iterator begin() { return Iterator(root->next); }
	Iterator end()   { return Iterator(root); }

	const Iterator begin() const { return Iterator(root->next); }
	const Iterator end()   const { return Iterator(root); }

};


#define LIST_FOR_EACH(list, iterator) \
    for ( \
        auto *(iterator) = (list)->root->next; \
        (iterator) && iterator != (list)->root; \
        iterator = iterator->next)

