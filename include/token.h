#pragma once
#include <string>

namespace token {

enum class TokenType {
    ILLEGAL,
    END_OF_FILE,
    IDENT,
    INT,
    ASSIGN,
    PLUS,
    COMMA,
    SEMICOLON,
    LPAREN,
    RPAREN,
    LBRACE,
    RBRACE,
    FUNCTION,
    LET
};

struct Token {
    TokenType type;
    std::string literal;
};

std::string tokenTypeToString(TokenType type);

TokenType lookUpIdentifier(std::string identifier);

} // namespace token
