#pragma once

#include "ast/Program.h"
#include "lexer.h"
#include "token.h"
#include <cstddef>
#include <string>

namespace parser {
class Parser {
  public:
    Parser(lexer::Lexer *lexer);
    ast::Program parseProgram();

  private:
    lexer::Lexer *lexer_;
    token::Token currentToken_;
    token::Token peekToken_;

    void nextToken();
};
} // namespace parser
