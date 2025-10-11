#ifndef VEC_UTILS_H
#define VEC_UTILS_H

#include <initializer_list>
#include <vector>
#include <set>
#include <algorithm>

template <typename T>
bool vec_contains(const std::vector<T>& vec, const T& elem) {
    auto res = std::find(vec.begin(), vec.end(), elem);
    return res == vec.end()? false : true;
}

template <typename T>
std::vector<T> vec_union(const std::initializer_list<std::vector<T>>& vecs) {
    std::set<T> tmp;
    for (auto vec: vecs) {
        tmp.insert(vec.begin(), vec.end());
    }
    return std::vector<T>(tmp.begin(), tmp.end());
}

template <typename T>
std::vector<T> vec_concat(const std::initializer_list<std::vector<T>>& vecs) {
    std::vector<T> res;
    for (auto vec: vecs) {
        res.insert(res.end(), vec.begin(), vec.end());
    }
    return res;
}

template <typename R, typename T>
std::vector<R> vec_cast(const std::vector<T>& input) {
    std::vector<R> res;
    for (auto t: input) {
        res.push_back(R(t));
    }
    return res;
}

#if __has_include(<utils/assert-utils.h>)
#include <utils/assert-utils.h>
template <typename T>
void safe_pop_back(T& container) {
    ASSERT (!container.empty());
    container.pop_back();
}
#endif

#endif // VEC_UTILS_H
