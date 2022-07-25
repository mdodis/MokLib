#pragma once
#include "../Config.h"
#include "../Memory/Base.h"
#include "../Memory/Extras.h"
#include "../Debugging/Assertions.h"
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


struct ListNode {
	ListNode *next;
	ListNode *prev;
};

static _inline bool is_list_node_empty(ListNode *node) {
	return (node == node->next) && (node == node->prev);
}

static _inline void make_list_node(ListNode *node) {
	node->next = node;
	node->prev = node;
}

static _inline void _list_add(ListNode *entry, ListNode *prev, ListNode *next) {
	ASSERT(is_list_node_empty(entry));
	next->prev = entry;
	entry->next = next;
	entry->prev = prev;
	prev->next = entry;
}

static _inline ListNode *__list_remove(ListNode *prev, ListNode *next) {
	next->prev = prev;
	prev->next = next;
	return prev;
}

static _inline ListNode *list_node_remove(ListNode *entry) {
	return __list_remove(entry->prev, entry->next);
}

static _inline void append_list_node(ListNode *source, ListNode *destination) {
	_list_add(source, destination->prev, destination);
}

static _inline void prepend_list_node(ListNode *source, ListNode *destination) {
	_list_add(source, destination, destination->next);
}

#define FOR_EACH_NODE(head, it) for (	\
	(it) = (head)->next;                \
	(it) != (head);                     \
	it = (it)->next)
