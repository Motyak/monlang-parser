#include <monlang-parser/LV1EntityMap.h>

#include <cstdint>

//////////////////////////////////////////////////////////////
// EntityEqual
//////////////////////////////////////////////////////////////

bool EntityEqual<LV1::Ast_>::operator()(const LV1::Ast_& a, const LV1::Ast_& b) const {
    if (a.index() != b.index()) {
        return false;
    }

    return std::visit(
        [this, &b](auto mf_a){
            auto mf_b = std::get<decltype(mf_a)>(b);
            return operator()(mf_a, mf_b);
        },
        a
    );
}

//////////////////////////////////////////////////////////////

bool EntityEqual<LV1::Ast_>::operator()(const MayFail<MayFail_<Program>>& prog_a, const MayFail<MayFail_<Program>>& prog_b) const {
    if (prog_a.has_error() != prog_b.has_error()) {
        return false;
    }
    if (prog_a.has_error() && prog_a.err->code != prog_b.err->code) {
        return false;
    }
    if (prog_a.val.sentences.size() != prog_b.val.sentences.size()) {
        return false;
    }
    for (size_t i = 0; i < prog_a.val.sentences.size(); ++i) {
        if (false == operator()(prog_a.val.sentences[i], prog_b.val.sentences[i])) {
            return false;
        }
    }
    return true;
}

bool EntityEqual<LV1::Ast_>::operator()(const MayFail<MayFail_<ProgramSentence>>& sen_a, const MayFail<MayFail_<ProgramSentence>>& sen_b) const {
    if (sen_a.has_error() != sen_b.has_error()) {
        return false;
    }
    if (sen_a.has_error() && sen_a.err->code != sen_b.err->code) {
        return false;
    }
    if (sen_a.val.programWords.size() != sen_b.val.programWords.size()) {
        return false;
    }
    for (size_t i = 0; i < sen_a.val.programWords.size(); ++i) {
        if (false == operator()(sen_a.val.programWords[i], sen_b.val.programWords[i])) {
            return false;
        }
    }
    return true;
}

bool EntityEqual<LV1::Ast_>::operator()(const MayFail<ProgramWord_>& pw_a, const MayFail<ProgramWord_>& pw_b) const {
    if (pw_a.has_error() != pw_b.has_error()) {
        return false;
    }
    if (pw_a.has_error() && pw_a.err->code != pw_b.err->code) {
        return false;
    }
    return std::visit(
        [&pw_b](auto* ptr_a){
            auto* ptr_b = std::get<decltype(ptr_a)>(pw_b.val);
            return reinterpret_cast<uintptr_t>(ptr_a)
                    == reinterpret_cast<uintptr_t>(ptr_b);
        },
        pw_a.val
    );
}

//////////////////////////////////////////////////////////////

bool EntityEqual<LV1::Ast_>::operator()(const MayFail<MayFail_<Term>>& term_a, const MayFail<MayFail_<Term>>& term_b) const {
    if (term_a.has_error() != term_b.has_error()) {
        return false;
    }
    if (term_a.has_error() && term_a.err->code != term_b.err->code) {
        return false;
    }
    if (term_a.val.words.size() != term_b.val.words.size()) {
        return false;
    }
    for (size_t i = 0; i < term_a.val.words.size(); ++i) {
        if (false == operator()(term_a.val.words[i], term_b.val.words[i])) {
            return false;
        }
    }
    return true;
}

bool EntityEqual<LV1::Ast_>::operator()(const MayFail<Word_>& word_a, const MayFail<Word_>& word_b) const {
    if (word_a.has_error() != word_b.has_error()) {
        return false;
    }
    if (word_a.has_error() && word_a.err->code != word_b.err->code) {
        return false;
    }
    return std::visit(
        [&word_b](auto* ptr_a){
            auto* ptr_b = std::get<decltype(ptr_a)>(word_b.val);
            return reinterpret_cast<uintptr_t>(ptr_a)
                    == reinterpret_cast<uintptr_t>(ptr_b);
        },
        word_a.val
    );
}

//////////////////////////////////////////////////////////////
// std::hash
//////////////////////////////////////////////////////////////

size_t std::hash<MayFail<MayFail_<Program>>>::operator()(const MayFail<MayFail_<Program>>& prog) const {
    for (auto sen: prog.val.sentences) {
        return hash_sen(sen);
    }
    return (prog.has_error()? 9000 : 1000) + ((LV1::Ast_)prog).index();
}

size_t std::hash<MayFail<MayFail_<ProgramSentence>>>::operator()(const MayFail<MayFail_<ProgramSentence>>& sen) const {
    for (auto pw: sen.val.programWords) {
        return hash_pw(pw);
    }
    return (sen.has_error()? 9000 : 1000) + ((LV1::Ast_)sen).index();
}

size_t std::hash<MayFail<ProgramWord_>>::operator()(const MayFail<ProgramWord_>& pw) const {
    return std::visit(
        [](auto* ptr){return reinterpret_cast<uintptr_t>(ptr);},
        pw.val
    );
}

//////////////////////////////////////////////////////////////

size_t std::hash<MayFail<MayFail_<Term>>>::operator()(const MayFail<MayFail_<Term>>& term) const {
    for (auto word: term.val.words) {
        return hash_word(word);
    }
    return (term.has_error()? 9000 : 1000) + ((LV1::Ast_)term).index();
}

size_t std::hash<MayFail<Word_>>::operator()(const MayFail<Word_>& word) const {
    return std::visit(
        [](auto* ptr){return reinterpret_cast<uintptr_t>(ptr);},
        word.val
    );
}
