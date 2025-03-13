#ifndef LV1_AST_BUILDER_H
#define LV1_AST_BUILDER_H

#include <monlang-LV1/ast/visitors/visitor.h>
#include <monlang-LV1/ast/Program.h>
#include <monlang-LV1/ast/Term.h>
#include <monlang-LV1/ast/SquareBracketsTerm.h>
#include <monlang-LV1/ast/Association.h>
#include <monlang-LV1/ast/PostfixParenthesesGroup.h>
#include <monlang-LV1/ast/PostfixSquareBracketsGroup.h>
#include <monlang-LV1/ast/CurlyBracketsGroup.h>
#include <monlang-LV1/ast/Atom.h>
#include <monlang-LV1/ast/Quotation.h>

#include <any>

class LV1AstBuilder {
  public:
    explicit LV1AstBuilder(const std::string& input);

    LV1::Ast buildAst();

    LV1::Program buildProgram();
    ProgramSentence buildProgramSentence();
    ProgramWord buildProgramWord();
    Term buildTerm();
    Word buildWord();

    SquareBracketsTerm buildSquareBracketsTerm();
    Association buildAssociation();
    PostfixParenthesesGroup buildPostfixParenthesesGroup();
    PostfixSquareBracketsGroup buildPostfixSquareBracketsGroup();
    ParenthesesGroup buildParenthesesGroup();
    SquareBracketsGroup buildSquareBracketsGroup();
    CurlyBracketsGroup buildCurlyBracketsGroup();
    Atom buildAtom();
    Quotation buildQuotation();

  private:
    static constexpr std::string LINE_PREFIX = "-> ";
    std::any tis; // TreeInputStream*
};

#endif // LV1_AST_BUILDER_H
