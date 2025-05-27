#pragma once

#include "ast/Program.h"
#include "lexer.h"
#include "token.h"
#include <cstddef>
#include <memory>
#include <string>

namespace parser {
class Parser {
  public:
    Parser(std::unique_ptr<lexer::Lexer> lexer);
    std::unique_ptr<ast::Program> parseProgram();

  private:
    std::unique_ptr<lexer::Lexer>  lexer_;
    token::Token currentToken_;
    token::Token peekToken_;

    void nextToken();
};
} // namespace parser
