// stackoverflow dot com /questions/32432450/what-is-standard-defer-finalizer-implementation-in-c

#ifndef DEFER_UTIL_H

template <class F>
struct __Deferrer {
    F f;
    ~__Deferrer() {
        f();
    }
};

struct __defer_dummy {};

template <class F>
__Deferrer<F> operator*(__defer_dummy, F f) {
    return {f};
}

#define __DEFER_(x) __defer##x
#define __DEFER(x) __DEFER_(x)
#define defer auto __DEFER(__LINE__) = __defer_dummy{} * [&]()

#endif // DEFER_UTIL_H
