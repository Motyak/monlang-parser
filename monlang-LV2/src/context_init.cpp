#include <monlang-LV2/context_init.h>

#include <any>

context_init_t::context_init_t() : statements(), sentence(), term(), malformed_stmt(), fallthrough(false){}

context_init_t::operator context_t() {
    return context_t{
        &statements,
        &sentence,
        &term,
        &malformed_stmt,
        &fallthrough
    };
}

// TODO: dtor that calls deepfree on all words
