#ifndef ABS63_H
#define ABS63_H

#include <iostream>
#include <cstdlib>
#include <climits>

#define abs63(n) abs63(n, __FILE__, __LINE__)

template <typename T>
int64_t (abs63)(T, const char* filename, int linenb);

template <>
inline int64_t (abs63)(uint64_t n, const char* filename, int linenb) {
    if (n > LLONG_MAX) {
        std::cerr << "abs63() failed in " << filename \
                    << " line " << linenb << std::endl; \
        std::terminate();
    }
    return llabs(n);
}

template <>
inline int64_t (abs63)(int64_t n, const char*, int) {
    return llabs(n);
}

template <typename T>
int64_t (abs63)(T, const char*, int) {
    SHOULD_NOT_HAPPEN();
}

#endif // ABS63_H
