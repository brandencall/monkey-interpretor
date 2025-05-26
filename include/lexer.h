#pragma once

#include "token.h"
#include <cstddef>
#include <string>

namespace lexer {
class Lexer {
  public:
    Lexer(const std::string &input);
    token::Token nextToken();

  private:
    std::string input_;
    size_t position_;
    size_t readPosition_;
    char ch_;

    void readChar();
    token::Token newToken(token::TokenType tokenType, char character);
    std::string readIdentifier();
    std::string readNumber();
    bool isLetter(char character);
    bool isDigit(char character);
    void skipWhitespace();
    char peekChar();
};
} // namespace lexer
