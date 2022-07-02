#pragma once
#include <iterator>

template <typename T, bool Forward = true>
struct SliceIterator {
    using iterator_category = std::forward_iterator_tag;
    using difference_type   = std::ptrdiff_t;
    using Val = T;
    using Ptr = T*;
    using Ref = T&;

    Ptr ptr;
    i32 index;
    bool fwd;

    SliceIterator(Ptr ptr, i32 index)
        : ptr(ptr)
        , index(index)
        {}

    Ref operator*()  const { return ptr[index]; }
    Ptr operator->() const { return ptr + index; }

    SliceIterator &operator++() {

        if (Forward)
            index++;
        else
            index--;

        return *this;
    }

    friend bool operator==(const SliceIterator &a, const SliceIterator &b) {
        return a.index == b.index;
    }

    friend bool operator!=(const SliceIterator &a, const SliceIterator &b) {
        return !(a == b);
    }
};

template <typename T>
struct TIteratorReverse { T &iterable; };

template <typename T>
auto begin(TIteratorReverse<T> r) { return r.iterable.rbegin(); }
template <typename T>
auto end(TIteratorReverse<T> r)   { return r.iterable.rend(); }

template <typename T>
TIteratorReverse<T> reverse(T &&iterable) { return {iterable}; }
