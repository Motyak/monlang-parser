#ifndef MEM_UTILS_H
#define MEM_UTILS_H

#include <utility> // std::move

template <typename T>
T* move_to_heap(T obj) {
    return new T(std::move(obj));
}

#endif // MEM_UTILS_H
