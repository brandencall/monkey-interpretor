#include "parser.h"
#include "ast/ArrayLiteral.h"
#include "ast/BlockStatement.h"
#include "ast/Boolean.h"
#include "ast/CallExpression.h"
#include "ast/Expression.h"
#include "ast/ExpressionStatement.h"
#include "ast/FunctionLiteral.h"
#include "ast/Identifier.h"
#include "ast/IfExpression.h"
#include "ast/InfixExpression.h"
#include "ast/IntegerLiteral.h"
#include "ast/PrefixExpression.h"
#include "ast/Program.h"
#include "ast/ReturnStatement.h"
#include "ast/Statement.h"
#include "ast/StringLiteral.h"
#include "lexer.h"
#include "token.h"
#include <exception>
#include <functional>
#include <memory>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

namespace parser {

Parser::Parser(std::unique_ptr<lexer::Lexer> lexer) : lexer_(std::move(lexer)) {
    nextToken();
    nextToken();

    registerPrefix(token::TokenType::IDENT, [this]() { return parseIdentifier(); });
    registerPrefix(token::TokenType::INT, [this]() { return parseIntegerLiteral(); });
    registerPrefix(token::TokenType::BANG, [this]() { return parsePrefixExpression(); });
    registerPrefix(token::TokenType::MINUS, [this]() { return parsePrefixExpression(); });
    registerPrefix(token::TokenType::TRUE, [this]() { return parseBoolean(); });
    registerPrefix(token::TokenType::FALSE, [this]() { return parseBoolean(); });
    registerPrefix(token::TokenType::LPAREN, [this]() { return parseGroupedExpression(); });
    registerPrefix(token::TokenType::IF, [this]() { return parseIfExpression(); });
    registerPrefix(token::TokenType::FUNCTION, [this]() { return parseFunctionLiteral(); });
    registerPrefix(token::TokenType::STRING, [this]() { return parseStringLiteral(); });
    registerPrefix(token::TokenType::LBRACKET, [this]() { return parseArrayLiteral(); });
    registerInfix(token::TokenType::PLUS,
                  [this](std::unique_ptr<ast::Expression> left) { return parseInfixExpression(std::move(left)); });
    registerInfix(token::TokenType::MINUS,
                  [this](std::unique_ptr<ast::Expression> left) { return parseInfixExpression(std::move(left)); });
    registerInfix(token::TokenType::SLASH,
                  [this](std::unique_ptr<ast::Expression> left) { return parseInfixExpression(std::move(left)); });
    registerInfix(token::TokenType::ASTERISK,
                  [this](std::unique_ptr<ast::Expression> left) { return parseInfixExpression(std::move(left)); });
    registerInfix(token::TokenType::EQ,
                  [this](std::unique_ptr<ast::Expression> left) { return parseInfixExpression(std::move(left)); });
    registerInfix(token::TokenType::NOT_EQ,
                  [this](std::unique_ptr<ast::Expression> left) { return parseInfixExpression(std::move(left)); });
    registerInfix(token::TokenType::LT,
                  [this](std::unique_ptr<ast::Expression> left) { return parseInfixExpression(std::move(left)); });
    registerInfix(token::TokenType::GT,
                  [this](std::unique_ptr<ast::Expression> left) { return parseInfixExpression(std::move(left)); });
    registerInfix(token::TokenType::LPAREN, [this](std::unique_ptr<ast::Expression> function) {
        return parseCallExpression(std::move(function));
    });
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
    std::unique_ptr<ast::LetStatement> statement = std::make_unique<ast::LetStatement>();
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
    nextToken();
    statement->value = parseExpression(Precedence::LOWEST);
    if (peekTokenIs(token::TokenType::SEMICOLON)) {
        nextToken();
    }
    return statement;
}

std::unique_ptr<ast::ReturnStatement> Parser::parseReturnStatement() {
    std::unique_ptr<ast::ReturnStatement> statement = std::make_unique<ast::ReturnStatement>();
    statement->token = currentToken_;
    nextToken();
    statement->returnValue = parseExpression(Precedence::LOWEST);
    if (peekTokenIs(token::TokenType::SEMICOLON)) {
        nextToken();
    }
    return statement;
}

std::unique_ptr<ast::ExpressionStatement> Parser::parseExpressionStatement() {
    std::unique_ptr<ast::ExpressionStatement> statement = std::make_unique<ast::ExpressionStatement>();
    statement->token = currentToken_;
    statement->expression = parseExpression(Precedence::LOWEST);

    if (peekTokenIs(token::TokenType::SEMICOLON)) {
        nextToken();
    }

    return statement;
}

std::unique_ptr<ast::PrefixExpression> Parser::parsePrefixExpression() {
    std::unique_ptr<ast::PrefixExpression> expression = std::make_unique<ast::PrefixExpression>();
    expression->token = currentToken_;
    expression->oper = currentToken_.literal;
    nextToken();
    expression->right = parseExpression(Precedence::PREFIX);
    return expression;
}

std::unique_ptr<ast::InfixExpression> Parser::parseInfixExpression(std::unique_ptr<ast::Expression> left) {
    std::unique_ptr<ast::InfixExpression> expression = std::make_unique<ast::InfixExpression>();
    expression->token = currentToken_;
    expression->oper = currentToken_.literal;
    expression->left = std::move(left);

    Precedence precedence = curPrecedence();
    nextToken();
    expression->right = parseExpression(precedence);
    return expression;
}

bool Parser::curTokenIs(token::TokenType tokenType) { return currentToken_.type == tokenType; }

bool Parser::peekTokenIs(token::TokenType tokenType) { return peekToken_.type == tokenType; }

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
    std::string err = "expected next token to be " + token::tokenTypeToString(tokenType) + ", got " +
                      token::tokenTypeToString(peekToken_.type);
    errors_.push_back(err);
}

Parser::Precedence Parser::peekPrecedence() {
    auto precedence = precedences_.find(peekToken_.type);
    if (precedence != precedences_.end()) {
        return precedence->second;
    }
    return Precedence::LOWEST;
}

Parser::Precedence Parser::curPrecedence() {
    auto precedence = precedences_.find(currentToken_.type);
    if (precedence != precedences_.end()) {
        return precedence->second;
    }
    return Precedence::LOWEST;
}

void Parser::registerPrefix(token::TokenType tokenType, prefixParseFn fn) { prefixParseFns[tokenType] = fn; }

void Parser::registerInfix(token::TokenType tokenType, infixParseFn fn) { infixParseFns[tokenType] = fn; }

std::unique_ptr<ast::Expression> Parser::parseExpression(Parser::Precedence precedence) {
    auto prefix = prefixParseFns.find(currentToken_.type);
    if (prefix == prefixParseFns.end()) {
        noPrefixError(currentToken_.type);
        return nullptr;
    }
    std::unique_ptr<ast::Expression> leftExpression = prefix->second();

    while (!peekTokenIs(token::TokenType::SEMICOLON) && precedence < peekPrecedence()) {
        auto infix = infixParseFns.find(peekToken_.type);
        if (infix == infixParseFns.end()) {
            return leftExpression;
        }
        nextToken();
        leftExpression = infix->second(std::move(leftExpression));
    }

    return leftExpression;
}

void Parser::noPrefixError(token::TokenType tokenType) {
    std::string err = "no prefix parse function for " + token::tokenTypeToString(tokenType) + " found";
    errors_.push_back(err);
}

std::unique_ptr<ast::Expression> Parser::parseIdentifier() {
    std::unique_ptr<ast::Identifier> identifier = std::make_unique<ast::Identifier>();
    identifier->token = currentToken_;
    identifier->value = currentToken_.literal;
    return identifier;
}

std::unique_ptr<ast::Expression> Parser::parseIntegerLiteral() {
    std::unique_ptr<ast::IntegerLiteral> literal = std::make_unique<ast::IntegerLiteral>();
    literal->token = currentToken_;
    literal->value = currentToken_.literal;
    try {
        literal->valueInt = std::stoi(currentToken_.literal);
    } catch (const std::exception &e) {
        throw std::runtime_error("Invalid integer value in parseIntegerLiteral");
    }
    return literal;
}

std::unique_ptr<ast::Expression> Parser::parseBoolean() {
    std::unique_ptr<ast::Boolean> boolean = std::make_unique<ast::Boolean>();
    boolean->token = currentToken_;
    boolean->value = currentToken_.literal;
    if (currentToken_.literal == "true") {
        boolean->valueBool = true;
    } else if (currentToken_.literal == "false") {
        boolean->valueBool = false;
    } else {
        throw std::runtime_error("Invalid bool value in parseBoolean. got=" + currentToken_.literal);
    }
    return boolean;
}

std::unique_ptr<ast::Expression> Parser::parseGroupedExpression() {
    nextToken();
    std::unique_ptr<ast::Expression> expression = parseExpression(Parser::Precedence::LOWEST);
    if (!expectPeek(token::TokenType::RPAREN)) {
        return nullptr;
    }
    return expression;
}

std::unique_ptr<ast::Expression> Parser::parseIfExpression() {
    std::unique_ptr<ast::IfExpression> expression = std::make_unique<ast::IfExpression>();
    expression->token = currentToken_;
    if (!expectPeek(token::TokenType::LPAREN)) {
        return nullptr;
    }
    nextToken();
    expression->condition = parseExpression(Precedence::LOWEST);
    if (!expectPeek(token::TokenType::RPAREN)) {
        return nullptr;
    }
    if (!expectPeek(token::TokenType::LBRACE)) {
        return nullptr;
    }
    expression->consiquence = parseBlockStatement();
    if (peekTokenIs(token::TokenType::ELSE)) {
        nextToken();
        if (!expectPeek(token::TokenType::LBRACE)) {
            return nullptr;
        }
        expression->alternative = parseBlockStatement();
    }
    return expression;
}

std::unique_ptr<ast::BlockStatement> Parser::parseBlockStatement() {
    std::unique_ptr<ast::BlockStatement> block = std::make_unique<ast::BlockStatement>();
    nextToken();
    while (!curTokenIs(token::TokenType::RBRACE) && !curTokenIs(token::TokenType::END_OF_FILE)) {
        std::unique_ptr<ast::Statement> statement = parseStatement();
        if (statement != nullptr) {
            block->statements.push_back(std::move(statement));
        }
        nextToken();
    }
    return block;
}

std::unique_ptr<ast::Expression> Parser::parseFunctionLiteral() {
    std::unique_ptr<ast::FunctionLiteral> literal = std::make_unique<ast::FunctionLiteral>();
    if (!expectPeek(token::TokenType::LPAREN)) {
        return nullptr;
    }
    literal->parameters = parseFunctionParameters();
    if (!expectPeek(token::TokenType::LBRACE)) {
        return nullptr;
    }
    literal->body = parseBlockStatement();
    return literal;
}

std::vector<std::unique_ptr<ast::Identifier>> Parser::parseFunctionParameters() {
    std::vector<std::unique_ptr<ast::Identifier>> identifiers;
    if (peekTokenIs(token::TokenType::RPAREN)) {
        nextToken();
        return identifiers;
    }
    nextToken();
    std::unique_ptr<ast::Identifier> identifier = std::make_unique<ast::Identifier>();
    identifier->token = currentToken_;
    identifier->value = currentToken_.literal;
    identifiers.push_back(std::move(identifier));
    while (peekTokenIs(token::TokenType::COMMA)) {
        nextToken();
        nextToken();
        std::unique_ptr<ast::Identifier> identifier = std::make_unique<ast::Identifier>();
        identifier->token = currentToken_;
        identifier->value = currentToken_.literal;
        identifiers.push_back(std::move(identifier));
    }
    if (!expectPeek(token::TokenType::RPAREN)) {
        // cant return null or nullptr; going to throw an exception;
        throw std::runtime_error("parseFunctionParameters didn't have a RPAREN in the correct spot");
    }
    return identifiers;
}

std::unique_ptr<ast::Expression> Parser::parseCallExpression(std::unique_ptr<ast::Expression> function) {
    std::unique_ptr<ast::CallExpression> expression = std::make_unique<ast::CallExpression>();
    expression->token = currentToken_;
    expression->function = std::move(function);
    expression->arguments = parseExpressionList(token::TokenType::RPAREN);
    return expression;
}

std::vector<std::unique_ptr<ast::Expression>> Parser::parseExpressionList(token::TokenType end) {
    std::vector<std::unique_ptr<ast::Expression>> list;
    if (peekTokenIs(end)) {
        nextToken();
        return list;
    }
    nextToken();
    std::unique_ptr<ast::Expression> exp = parseExpression(Precedence::LOWEST);
    list.push_back(std::move(exp));

    while (peekTokenIs(token::TokenType::COMMA)) {
        nextToken();
        nextToken();
        std::unique_ptr<ast::Expression> exp = parseExpression(Precedence::LOWEST);
        list.push_back(std::move(exp));
    }
    if (!expectPeek(end)) {
        throw std::runtime_error("parseExpression list didn't have correct closing token in the correct spot");
    }
    return list;
}

std::unique_ptr<ast::Expression> Parser::parseStringLiteral() {
    std::unique_ptr<ast::StringLiteral> literal = std::make_unique<ast::StringLiteral>();
    literal->token = currentToken_;
    literal->value = currentToken_.literal;
    literal->valueString = currentToken_.literal;
    return literal;
}

std::unique_ptr<ast::Expression> Parser::parseArrayLiteral() {
    std::unique_ptr<ast::ArrayLiteral> literal = std::make_unique<ast::ArrayLiteral>();
    literal->token = currentToken_;
    literal->elements = parseExpressionList(token::TokenType::RBRACKET);
    return literal;
}
} // namespace parser
