#pragma once

template <typename T>
struct DeferPrivate {
    T t;
    DeferPrivate(T t) : t(t) {}
    ~DeferPrivate() {
        t();
    }
};

template <typename T>
DeferPrivate<T> defer_func(T t) {
    return DeferPrivate<T>(t);
}

#define DEFER_1(x, y)   x##y
#define DEFER_2(x, y)   DEFER_1(x, y)
#define DEFER_3(x)      DEFER_2(x, __COUNTER__)
#define DEFER(code) auto DEFER_3(_defer_) = defer_func([&](){code;})