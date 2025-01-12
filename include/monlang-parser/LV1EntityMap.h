#ifndef LV1_ENTITY_MAP_H
#define LV1_ENTITY_MAP_H

#include <monlang-LV1/visitors/visitor.h>

template <typename T>
struct EntityEqual;

template <>
struct EntityEqual<LV1::Ast_> {
    // the entrypoint
    bool operator()(const LV1::Ast_& a,
                    const LV1::Ast_& b) const;

    ///

    bool operator()(const MayFail<MayFail_<Program>>& prog_a,
                    const MayFail<MayFail_<Program>>& prog_b) const;

    bool operator()(const MayFail<MayFail_<ProgramSentence>>& sen_a,
                    const MayFail<MayFail_<ProgramSentence>>& sen_b) const;

    bool operator()(const MayFail<ProgramWord_>& pw_a,
                    const MayFail<ProgramWord_>& pw_b) const;

    ///

    bool operator()(const MayFail<MayFail_<Term>>& term_a,
                    const MayFail<MayFail_<Term>>& term_b) const;

    bool operator()(const MayFail<Word_>& word_a,
                    const MayFail<Word_>& word_b) const;
};

//////////////////////////////////////////////////////////////

template <>
struct std::hash<MayFail<ProgramWord_>> {
    size_t operator()(const MayFail<ProgramWord_>&) const;
};

template <>
struct std::hash<MayFail<MayFail_<ProgramSentence>>> {
    std::hash<MayFail<ProgramWord_>> hash_pw;

    size_t operator()(const MayFail<MayFail_<ProgramSentence>>&) const;
};

template <>
struct std::hash<MayFail<MayFail_<Program>>> {
    std::hash<MayFail<MayFail_<ProgramSentence>>> hash_sen;

    size_t operator()(const MayFail<MayFail_<Program>>&) const;
};

///

template <>
struct std::hash<MayFail<Word_>> {
    size_t operator()(const MayFail<Word_>&) const;
};

template <>
struct std::hash<MayFail<MayFail_<Term>>> {
    std::hash<MayFail<Word_>> hash_word;

    size_t operator()(const MayFail<MayFail_<Term>>&) const;
};

#endif // LV1_ENTITY_MAP_H
