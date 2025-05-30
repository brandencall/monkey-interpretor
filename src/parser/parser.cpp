#include "parser.h"
#include "ast/Expression.h"
#include "ast/ExpressionStatement.h"
#include "ast/Identifier.h"
#include "ast/Program.h"
#include "ast/ReturnStatement.h"
#include "ast/Statement.h"
#include "lexer.h"
#include "token.h"
#include <functional>
#include <memory>
#include <string>
#include <utility>
#include <vector>

namespace parser {

Parser::Parser(std::unique_ptr<lexer::Lexer> lexer) : lexer_(std::move(lexer)) {
    nextToken();
    nextToken();

    registerPrefix(token::TokenType::IDENT,
                   [this]() { return parseIdentifier(); });
}

void Parser::nextToken() {
    currentToken_ = peekToken_;
    peekToken_ = lexer_->nextToken();
}

std::unique_ptr<ast::Program> Parser::parseProgram() {
    std::unique_ptr<ast::Program> program = std::make_unique<ast::Program>();

    while (!curTokenIs(token::TokenType::END_OF_FILE)) {
        std::unique_ptr<ast::Statement> statement = parseStatement();
        if (statement != nullptr) {
            program->statements.push_back(std::move(statement));
        }
        nextToken();
    }
    return program;
}

std::vector<std::string> *Parser::errors() { return &errors_; }

std::unique_ptr<ast::Statement> Parser::parseStatement() {
    switch (currentToken_.type) {
    case token::TokenType::LET:
        return parseLetStatement();
    case token::TokenType::RETURN:
        return parseReturnStatement();
    default:
        return parseExpressionStatement();
    }
}

std::unique_ptr<ast::LetStatement> Parser::parseLetStatement() {
    std::unique_ptr<ast::LetStatement> statement =
        std::make_unique<ast::LetStatement>();
    statement->token = currentToken_;

    if (!expectPeek(token::TokenType::IDENT)) {
        return nullptr;
    }

    statement->name = std::make_unique<ast::Identifier>();
    statement->name->token = currentToken_;
    statement->name->value = currentToken_.literal;

    if (!expectPeek(token::TokenType::ASSIGN)) {
        return nullptr;
    }

    // TODO: this is not write but throwing it in for now
    while (!curTokenIs(token::TokenType::SEMICOLON)) {
        nextToken();
    }

    return statement;
}

std::unique_ptr<ast::ReturnStatement> Parser::parseReturnStatement() {
    std::unique_ptr<ast::ReturnStatement> statement =
        std::make_unique<ast::ReturnStatement>();
    statement->token = currentToken_;

    nextToken();

    // TODO: this is not write but throwing it in for now
    while (!curTokenIs(token::TokenType::SEMICOLON)) {
        nextToken();
    }

    return statement;
}

std::unique_ptr<ast::ExpressionStatement> Parser::parseExpressionStatement() {
    std::unique_ptr<ast::ExpressionStatement> statement =
        std::make_unique<ast::ExpressionStatement>();
    statement->token = currentToken_;
    statement->expression = parseExpression(Precedence::LOWEST);

    if (peekTokenIs(token::TokenType::SEMICOLON)) {
        nextToken();
    }

    return statement;
}

bool Parser::curTokenIs(token::TokenType tokenType) {
    return currentToken_.type == tokenType;
}

bool Parser::peekTokenIs(token::TokenType tokenType) {
    return peekToken_.type == tokenType;
}

bool Parser::expectPeek(token::TokenType tokenType) {
    if (peekTokenIs(tokenType)) {
        nextToken();
        return true;
    } else {
        peekError(tokenType);
        return false;
    }
}

void Parser::peekError(token::TokenType tokenType) {
    std::string err = "expected next token to be " +
                      token::tokenTypeToString(tokenType) + ", got " +
                      token::tokenTypeToString(peekToken_.type);
    errors_.push_back(err);
}

void Parser::registerPrefix(token::TokenType tokenType, prefixParseFn fn) {
    prefixParseFns[tokenType] = fn;
}

void Parser::registerInfix(token::TokenType tokenType, infixParseFn fn) {
    infixParseFns[tokenType] = fn;
}

std::unique_ptr<ast::Expression>
Parser::parseExpression(Parser::Precedence precedence) {
    auto prefix = prefixParseFns.find(currentToken_.type);
    if (prefix == prefixParseFns.end()) {
        return nullptr;
    }
    std::unique_ptr<ast::Expression> leftExpression = prefix->second();
    return leftExpression;
}

std::unique_ptr<ast::Expression> Parser::parseIdentifier() {
    std::unique_ptr<ast::Identifier> identifier =
        std::make_unique<ast::Identifier>();
    identifier->token = currentToken_;
    identifier->value = currentToken_.literal;
    return identifier;
}

} // namespace parser
