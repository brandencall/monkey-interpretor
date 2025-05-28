#include "parser.h"
#include "ast/Identifier.h"
#include "ast/Program.h"
#include "ast/Statement.h"
#include "lexer.h"
#include "token.h"
#include <iostream>
#include <memory>
#include <utility>

namespace parser {

parser::Parser::Parser(std::unique_ptr<lexer::Lexer> lexer)
    : lexer_(std::move(lexer)) {
    nextToken();
    nextToken();
}

void parser::Parser::nextToken() {
    currentToken_ = peekToken_;
    peekToken_ = lexer_->nextToken();
}

std::unique_ptr<ast::Program> parser::Parser::parseProgram() {
    std::unique_ptr<ast::Program> program = std::make_unique<ast::Program>();

    while (currentToken_.type != token::TokenType::END_OF_FILE) {
        std::unique_ptr<ast::Statement> statement = parseStatement();
        if (statement != nullptr) {
            program->statements.push_back(std::move(statement));
        }
        nextToken();
    }
    return program;
}

std::unique_ptr<ast::Statement> parser::Parser::parseStatement() {
    switch (currentToken_.type) {
    case token::TokenType::LET:
        return parseLetStatement();
    default:
        return nullptr;
    }
}

std::unique_ptr<ast::LetStatement> parser::Parser::parseLetStatement() {
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

bool parser::Parser::curTokenIs(token::TokenType tokenType) {
    return currentToken_.type == tokenType;
}

bool parser::Parser::peekTokenIs(token::TokenType tokenType) {
    return peekToken_.type == tokenType;
}

bool parser::Parser::expectPeek(token::TokenType tokenType) {
    if (peekTokenIs(tokenType)) {
        nextToken();
        return true;
    } else {
        return false;
    }
}

} // namespace parser
