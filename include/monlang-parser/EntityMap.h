/*
    standalone header, no .cpp
*/

#ifndef ENTITY_MAP_H
#define ENTITY_MAP_H

#include <unordered_map>

template <typename T>
struct EntityEqual;

template <typename K, typename V>
using EntityMap = std::unordered_map<K, V, std::hash<K>, EntityEqual<K>>;

// EntityEqual and std::hash template specializations for LV1::Ast_
#include <monlang-parser/LV1EntityMap.h>

#endif // ENTITY_MAP_H
