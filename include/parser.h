#pragma once

#include "ast/Expression.h"
#include "ast/Program.h"
#include "ast/ReturnStatement.h"
#include "ast/Statement.h"
#include "ast/LetStatement.h"
#include "lexer.h"
#include "token.h"
#include <cstddef>
#include <functional>
#include <map>
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
    using prefixParseFn = std::function<std::unique_ptr<ast::Expression>()>;
    using infixParseFn = std::function<std::unique_ptr<ast::Expression>(std::unique_ptr<ast::Expression>)>;
    std::map<token::TokenType, prefixParseFn> prefixParseFns;
    std::map<token::TokenType, infixParseFn> infixParseFns;

    void nextToken();
    std::unique_ptr<ast::Statement> parseStatement();
    std::unique_ptr<ast::LetStatement> parseLetStatement();
    std::unique_ptr<ast::ReturnStatement> parseReturnStatement();
    bool curTokenIs(token::TokenType tokenType);
    bool peekTokenIs(token::TokenType tokenType);
    bool expectPeek(token::TokenType tokenType);
    void peekError(token::TokenType tokenType);
    void registerPrefix(token::TokenType tokenType, prefixParseFn fn); 
    void registerInfix(token::TokenType tokenType, infixParseFn fn); 
};
} // namespace parser
