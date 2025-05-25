#include "lexer.h"
#include "token.h"
#include <string>

namespace lexer {

Lexer::Lexer(const std::string &input)
    : input_(input), position_(0), readPosition_(0), ch_(0) {
    readChar();
}

void Lexer::readChar() {

    if (readPosition_ >= input_.length()) {
        ch_ = 0;
    } else {
        ch_ = input_[readPosition_];
    }
    position_ = readPosition_;
    readPosition_ += 1;
}

token::Token Lexer::nextToken() {
    token::Token token;

    switch (ch_) {
    case '=':
        token = newToken(token::TokenType::ASSIGN, ch_);
        break;
    case ';':
        token = newToken(token::TokenType::SEMICOLON, ch_);
        break;
    case '(':
        token = newToken(token::TokenType::LPAREN, ch_);
        break;
    case ')':
        token = newToken(token::TokenType::RPAREN, ch_);
        break;
    case ',':
        token = newToken(token::TokenType::COMMA, ch_);
        break;
    case '+':
        token = newToken(token::TokenType::PLUS, ch_);
        break;
    case '{':
        token = newToken(token::TokenType::LBRACE, ch_);
        break;
    case '}':
        token = newToken(token::TokenType::RBRACE, ch_);
        break;
    case 0:
        token.literal = "";
        token.type = token::TokenType::END_OF_FILE;
    default:
        if (isLetter(ch_)) {
            token.literal = readIdentifier();
        } else {
            token = newToken(token::TokenType::ILLEGAL, ch_);
        }
    }
    readChar();
    return token;
}

token::Token Lexer::newToken(token::TokenType tokenType, char character) {
    token::Token token;
    token.type = tokenType;
    token.literal = character;
    return token;
}
//TODO: implement
std::string Lexer::readIdentifier() {
    return "";
}

bool Lexer::isLetter(char character) {
    return ('a' <= character && character  <= 'z') || ('A' <= character  && character  <= 'Z') || character  == '_';
}

} // namespace lexer
