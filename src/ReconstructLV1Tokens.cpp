#include <monlang-parser/ReconstructLV1Tokens.h>

#include <monlang-LV1/Program.h>
#include <monlang-LV1/ProgramSentence.h>
#include <monlang-LV1/Term.h>
#include <monlang-LV1/Word.h>

/* impl only */
#include <monlang-LV1/Atom.h>
#include <monlang-LV1/SquareBracketsTerm.h>
#include <monlang-LV1/SquareBracketsGroup.h>
#include <monlang-LV1/ParenthesesGroup.h>
#include <monlang-LV1/CurlyBracketsGroup.h>
#include <monlang-LV1/PostfixSquareBracketsGroup.h>
#include <monlang-LV1/PostfixParenthesesGroup.h>
#include <monlang-LV1/Association.h>

#include <utils/assert-utils.h>

void ReconstructLV1Tokens::operator()(const MayFail<MayFail_<Program>>&) {
    TODO();
}

void ReconstructLV1Tokens::operator()(const MayFail<MayFail_<ProgramSentence>>&) {
    TODO();
}

void ReconstructLV1Tokens::operator()(const MayFail<ProgramWord_>&) {
    TODO();
}

void ReconstructLV1Tokens::operator()(const MayFail<MayFail_<Term>>&) {
    TODO();
}

void ReconstructLV1Tokens::operator()(const MayFail<Word_>&) {
    TODO();
}

///////////////////////////////////////////////////////////////

void ReconstructLV1Tokens::operator()(Atom*) {
    TODO();
}

void ReconstructLV1Tokens::operator()(MayFail_<SquareBracketsTerm>*) {
    TODO();
}

void ReconstructLV1Tokens::operator()(MayFail_<SquareBracketsGroup>*) {
    TODO();
}

void ReconstructLV1Tokens::operator()(MayFail_<ParenthesesGroup>*) {
    TODO();
}

void ReconstructLV1Tokens::operator()(MayFail_<CurlyBracketsGroup>*) {
    TODO();
}

void ReconstructLV1Tokens::operator()(MayFail_<PostfixSquareBracketsGroup>*) {
    TODO();
}

void ReconstructLV1Tokens::operator()(MayFail_<PostfixParenthesesGroup>*) {
    TODO();
}

void ReconstructLV1Tokens::operator()(MayFail_<Association>*) {
    TODO();
}


///////////////////////////////////////////////////////////////

ReconstructLV1Tokens::ReconstructLV1Tokens(LV1Tokens& tokens) : tokens(tokens){}
