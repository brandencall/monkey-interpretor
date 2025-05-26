#include "token.h"
#include <map>

namespace token {


std::map<std::string, TokenType> keywords {
    {"fn", token::TokenType::FUNCTION}, 
    {"let", token::TokenType::LET} 
};

std::string tokenTypeToString(TokenType type) {
    switch (type) {
    case token::TokenType::ILLEGAL:
        return "ILLEGAL";
    case token::TokenType::END_OF_FILE:
        return "END_OF_FILE";
    case token::TokenType::IDENT:
        return "IDENT";
    case token::TokenType::INT:
        return "INT";
    case token::TokenType::ASSIGN:
        return "=";
    case token::TokenType::PLUS:
        return "+";
    case token::TokenType::COMMA:
        return ",";
    case token::TokenType::SEMICOLON:
        return ";";
    case token::TokenType::LPAREN:
        return "(";
    case token::TokenType::RPAREN:
        return ")";
    case token::TokenType::LBRACE:
        return "{";
    case token::TokenType::RBRACE:
        return "}";
    case token::TokenType::FUNCTION:
        return "FUNCTION";
    case token::TokenType::LET:
        return "LET";
    default:
        return "UNKNOWN";
    }
}

TokenType lookUpIdentifier(std::string identifier){
    if (keywords.count(identifier)){
        return keywords[identifier];
    }
    return token::TokenType::IDENT;
}

} // namespace token
