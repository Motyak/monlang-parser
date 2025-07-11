#ifndef STDFUNC_UTILS_H
#define STDFUNC_UTILS_H

#include <cstddef>

template<typename T, typename... U>
size_t get_addr(std::function<T(U...)> f) {
    typedef T(fnType)(U...);
    fnType ** fnPointer = f.template target<fnType*>();
    return (size_t) *fnPointer;
}

#endif // STDFUNC_UTILS_H
