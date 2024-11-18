#ifndef NB_UTILS_H
#define NB_UTILS_H

#include <vector>

template <typename T, typename U>
inline std::vector<int> range(T from, U to) {
    auto from_ = static_cast<int>(from);
    auto to_ = static_cast<int>(to);

    std::vector<int> res;
    if (from_ <= to_) {
        for (int i = from_; i < to_; i++) {
            res.push_back(i);
        }
        return res;
    }

    for (int i = from_; i > to_; --i) {
        res.push_back(i);
    }
    return res;
}

#endif // NB_UTILS_H
