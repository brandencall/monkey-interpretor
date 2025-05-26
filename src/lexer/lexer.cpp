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

    skipWhitespace();

    switch (ch_) {
    case '=':
        if (peekChar() == '=') {
            char peekedCh = ch_;
            readChar();
            std::string combinedChars;
            combinedChars += peekedCh;
            combinedChars += ch_;
            token.literal = combinedChars; 
            token.type = token::TokenType::EQ;
        } else {
            token = newToken(token::TokenType::ASSIGN, ch_);
        }
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
    case '-':
        token = newToken(token::TokenType::MINUS, ch_);
        break;
    case '!':
        if (peekChar() == '=') {
            char peekedCh = ch_;
            readChar();
            std::string combinedChars;
            combinedChars += peekedCh;
            combinedChars += ch_;
            token.literal = combinedChars; 
            token.type = token::TokenType::NOT_EQ;
        } else {
            token = newToken(token::TokenType::BANG, ch_);
        }
        break;
    case '/':
        token = newToken(token::TokenType::SLASH, ch_);
        break;
    case '*':
        token = newToken(token::TokenType::ASTERISK, ch_);
        break;
    case '<':
        token = newToken(token::TokenType::LT, ch_);
        break;
    case '>':
        token = newToken(token::TokenType::GT, ch_);
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
        break;
    default:
        if (isLetter(ch_)) {
            token.literal = readIdentifier();
            token.type = token::lookUpIdentifier(token.literal);
            return token;
        } else if (isDigit(ch_)) {
            token.type = token::TokenType::INT;
            token.literal = readNumber();
            return token;
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

std::string Lexer::readIdentifier() {
    int pos = position_;
    while (isLetter(ch_)) {
        readChar();
    }
    int len = position_ - pos;
    return input_.substr(pos, len);
}

std::string Lexer::readNumber() {
    int pos = position_;
    while (isDigit(ch_)) {
        readChar();
    }
    int len = position_ - pos;
    return input_.substr(pos, len);
}

bool Lexer::isLetter(char character) {
    return ('a' <= character && character <= 'z') ||
           ('A' <= character && character <= 'Z') || character == '_';
}

bool Lexer::isDigit(char character) {
    return '0' <= character && character <= '9';
}

void Lexer::skipWhitespace() {
    while (ch_ == ' ' || ch_ == '\t' || ch_ == '\n' || ch_ == '\r') {
        readChar();
    }
}

char Lexer::peekChar() {
    if (readPosition_ >= input_.length()) {
        return 0;
    } else {
        return input_[readPosition_];
    }
}

} // namespace lexer
