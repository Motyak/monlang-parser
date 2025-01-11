//TODO: separate decl and def

#ifndef ENTITY_MAP_H
#define ENTITY_MAP_H

#include <monlang-LV1/visitors/visitor.h>
#include <monlang-LV2/visitors/visitor.h>

#include <unordered_map>

//////////////////////////////////////////////////////////////
// LV1 entities
//////////////////////////////////////////////////////////////

template <>
struct std::hash<MayFail<ProgramWord_>> {
    std::size_t operator()(const MayFail<ProgramWord_>& pw) const {
        auto key = std::string("");
        key += "pw";
        key += std::to_string(pw.val.index());
        key += (pw.has_error()? "M" : "_");
        key += std::visit(
            [](auto* mf_){return std::to_string(reinterpret_cast<std::uintptr_t>(mf_));},
            pw.val
        );
        auto str_hash = std::hash<std::string>();
        return str_hash(key);
    }
};

template <>
struct std::hash<MayFail<MayFail_<ProgramSentence>>> {
    std::size_t operator()(const MayFail<MayFail_<ProgramSentence>>& sen) const {
        auto key = std::string("");
        key += "sen";
        key += (sen.has_error()? "M" : "_");
        auto pw_hash = std::hash<MayFail<ProgramWord_>>();
        for (auto pw: sen.val.programWords) {
            key += pw_hash(pw);
        }
        auto str_hash = std::hash<std::string>();
        return str_hash(key);
    }
};

template <>
struct std::hash<MayFail<MayFail_<Program>>> {
    std::size_t operator()(const MayFail<MayFail_<Program>>& prog) const {
        auto key = std::string("");
        key += "prog";
        key += (prog.has_error()? "M" : "_");
        auto sen_hash = std::hash<MayFail<MayFail_<ProgramSentence>>>();
        for (auto sen: prog.val.sentences) {
            key += sen_hash(sen);
        }
        auto str_hash = std::hash<std::string>();
        return str_hash(key);
    }
};

template <>
struct std::hash<MayFail<Word_>> {
    std::size_t operator()(const MayFail<Word_>& word) const {
        auto key = std::string("");
        key += "word";
        key += std::to_string(word.val.index());
        key += (word.has_error()? "M" : "_");
        key += std::visit(
            [](auto* mf_){return std::to_string(reinterpret_cast<std::uintptr_t>(mf_));},
            word.val
        );
        auto str_hash = std::hash<std::string>();
        return str_hash(key);
    }
};

template <>
struct std::hash<MayFail<MayFail_<Term>>> {
    std::size_t operator()(const MayFail<MayFail_<Term>>& term) const {
        auto key = std::string("");
        key += "term";
        key += (term.has_error()? "M" : "_");
        auto word_hash = std::hash<MayFail<Word_>>();
        for (auto word: term.val.words) {
            key += word_hash(word);
        }
        auto str_hash = std::hash<std::string>();
        return str_hash(key);
    }
};

//////////////////////////////////////////////////////////////
// LV2 entities
//////////////////////////////////////////////////////////////

// TODO
// ...

//////////////////////////////////////////////////////////////

template <typename T>
struct EqualByHash {
    bool operator()(const T& a, const T& b) const {
        auto hash = std::hash<T>();
        return hash(a) == hash(b);
    }
};

template <typename K, typename V>
using EntityMap = std::unordered_map<K, V, std::hash<K>, EqualByHash<K>>;

#endif // ENTITY_MAP_H
