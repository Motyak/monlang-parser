/*
    THIS IS A HEADER-ONLY FILE
*/

#ifndef LV2_COMMON_H
#define LV2_COMMON_H

#include <utils/MayFail.h>

template <typename... Targs>
size_t group_nesting(const std::variant<Targs...>& entityVariant) {
    return std::visit(
        [](auto* entity){return entity->_groupNesting;},
        entityVariant
    );
}

template <typename T>
size_t group_nesting(const T& entity) {
    return entity._groupNesting;
}

template <typename... Targs>
void set_group_nesting(const std::variant<Targs...>& entityVariant, size_t groupNesting) {
    std::visit(
        [groupNesting](auto* entity){entity->_groupNesting = groupNesting;},
        entityVariant
    );
}

#endif // LV2_COMMON_H
