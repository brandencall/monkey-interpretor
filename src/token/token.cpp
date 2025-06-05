#include "token.h"
#include <map>

namespace token {

std::map<std::string, TokenType> keywords{
    {"fn", token::TokenType::FUNCTION},   {"let", token::TokenType::LET}, {"true", token::TokenType::TRUE},
    {"false", token::TokenType::FALSE},   {"if", token::TokenType::IF},   {"else", token::TokenType::ELSE},
    {"return", token::TokenType::RETURN},
};

std::string tokenTypeToString(TokenType type) {
    switch (type) {
    case TokenType::ILLEGAL:
        return "ILLEGAL";
    case TokenType::END_OF_FILE:
        return "END_OF_FILE";
    case TokenType::IDENT:
        return "IDENT";
    case TokenType::INT:
        return "INT";
    case TokenType::ASSIGN:
        return "=";
    case TokenType::PLUS:
        return "+";
    case TokenType::MINUS:
        return "-";
    case TokenType::BANG:
        return "!";
    case TokenType::ASTERISK:
        return "*";
    case TokenType::SLASH:
        return "/";
    case TokenType::LT:
        return "<";
    case TokenType::GT:
        return ">";
    case TokenType::EQ:
        return "==";
    case TokenType::NOT_EQ:
        return "!=";
    case TokenType::COMMA:
        return ",";
    case TokenType::SEMICOLON:
        return ";";
    case TokenType::LPAREN:
        return "(";
    case TokenType::RPAREN:
        return ")";
    case TokenType::LBRACE:
        return "{";
    case TokenType::RBRACE:
        return "}";
    case TokenType::FUNCTION:
        return "FUNCTION";
    case TokenType::LET:
        return "LET";
    case TokenType::TRUE:
        return "TRUE";
    case TokenType::FALSE:
        return "FALSE";
    case TokenType::IF:
        return "IF";
    case TokenType::ELSE:
        return "ELSE";
    case TokenType::RETURN:
        return "RETURN";
    case TokenType::STRING:
        return "STRING";
    case TokenType::LBRACKET:
        return "LBRACKET";
    case TokenType::RBRACKET:
        return "RBRACKET";
    case TokenType::COLON:
        return ":";
    default:
        return "UNKNOWN";
    }
}

TokenType lookUpIdentifier(std::string identifier) {
    if (keywords.count(identifier)) {
        return keywords[identifier];
    }
    return TokenType::IDENT;
}

} // namespace token
