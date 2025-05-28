#pragma once

#include "ast/Program.h"
#include "ast/Statement.h"
#include "ast/LetStatement.h"
#include "lexer.h"
#include "token.h"
#include <cstddef>
#include <memory>
#include <string>
#include <vector>

namespace parser {
class Parser {
  public:
    Parser(std::unique_ptr<lexer::Lexer> lexer);
    std::unique_ptr<ast::Program> parseProgram();
    std::vector<std::string>* errors();

  private:
    std::unique_ptr<lexer::Lexer>  lexer_;
    token::Token currentToken_;
    token::Token peekToken_;
    std::vector<std::string> errors_;

    void nextToken();
    std::unique_ptr<ast::Statement> parseStatement();
    std::unique_ptr<ast::LetStatement> parseLetStatement();
    bool curTokenIs(token::TokenType tokenType);
    bool peekTokenIs(token::TokenType tokenType);
    bool expectPeek(token::TokenType tokenType);
    void peekError(token::TokenType tokenType);
};
} // namespace parser
