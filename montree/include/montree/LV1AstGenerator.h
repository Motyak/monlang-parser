#ifndef LV1_AST_GENERATOR_H
#define LV1_AST_GENERATOR_H

#include <monlang-LV1/ast/Program.h>
#include <monlang-LV1/ast/Term.h>
#include <monlang-LV1/ast/SquareBracketsTerm.h>
#include <monlang-LV1/ast/Association.h>
#include <monlang-LV1/ast/PostfixParenthesesGroup.h>
#include <monlang-LV1/ast/PostfixSquareBracketsGroup.h>
#include <monlang-LV1/ast/CurlyBracketsGroup.h>
#include <monlang-LV1/ast/Atom.h>

#include <random>
#include <map>

#define ulong(x) ((unsigned long)x)

class LV1AstGenerator {
  public:
    explicit LV1AstGenerator(int seed);
    explicit LV1AstGenerator();

    LV1::Program generateProgram();
    ProgramSentence generateProgramSentence();
    ProgramWord generateProgramWord();
    Term generateTerm();
    Word generateWord();

    SquareBracketsTerm generateSquareBracketsTerm();
    Association generateAssociation();
    PostfixParenthesesGroup generatePostfixParenthesesGroup();
    PostfixSquareBracketsGroup generatePostfixSquareBracketsGroup();
    ParenthesesGroup generateParenthesesGroup();
    SquareBracketsGroup generateSquareBracketsGroup();
    CurlyBracketsGroup generateCurlyBracketsGroup();
    Atom generateAtom();

  private:
    std::mt19937 rng;
    std::uniform_int_distribution<unsigned long> rand;
    std::map<std::string, unsigned long> proba;
    bool is_parent_call = true;

    // friend class RoutineRAII;
    class RoutineRAII {
      public:
        RoutineRAII(LV1AstGenerator*);
        ~RoutineRAII();

      private:
        LV1AstGenerator* gen;
        const bool is_parent_call_val;
    };
};

#endif // LV1_AST_GENERATOR_H
