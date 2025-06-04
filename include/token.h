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
    MINUS,
    BANG,
    ASTERISK,
    SLASH,

    LT,
    GT,
    EQ,
    NOT_EQ,

    COMMA,
    SEMICOLON,

    LPAREN,
    RPAREN,
    LBRACE,
    RBRACE,

    FUNCTION,
    LET,
    TRUE,
    FALSE,
    IF,
    ELSE,
    RETURN,

    STRING,

    LBRACKET,
    RBRACKET,
};

struct Token {
    TokenType type;
    std::string literal;
};

std::string tokenTypeToString(TokenType type);

TokenType lookUpIdentifier(std::string identifier);

} // namespace token
