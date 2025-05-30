#pragma once

#include "ast/Expression.h"
#include "ast/ExpressionStatement.h"
#include "ast/LetStatement.h"
#include "ast/PrefixExpression.h"
#include "ast/Program.h"
#include "ast/ReturnStatement.h"
#include "ast/Statement.h"
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
    std::vector<std::string> *errors();
    enum class Precedence { LOWEST = 0, EQUALS, LESSGREATER, SUM, PRODUCT, PREFIX, CALL };

  private:
    std::unique_ptr<lexer::Lexer> lexer_;
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
    std::unique_ptr<ast::ExpressionStatement> parseExpressionStatement();
    std::unique_ptr<ast::PrefixExpression> parsePrefixExpression();
    bool curTokenIs(token::TokenType tokenType);
    bool peekTokenIs(token::TokenType tokenType);
    bool expectPeek(token::TokenType tokenType);
    void peekError(token::TokenType tokenType);
    void registerPrefix(token::TokenType tokenType, prefixParseFn fn);
    void registerInfix(token::TokenType tokenType, infixParseFn fn);
    std::unique_ptr<ast::Expression> parseExpression(Precedence precidence);
    void noPrefixError(token::TokenType tokenType);
    std::unique_ptr<ast::Expression> parseIdentifier();
    std::unique_ptr<ast::Expression> parseIntegerLiteral();
};
} // namespace parser
