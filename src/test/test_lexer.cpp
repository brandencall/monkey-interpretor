#include <gtest/gtest.h>
#include "lexer.h"
#include "token.h"

TEST(LexerTest, NextToken) {
    std::string input = "=+(){},;";

    std::vector<std::pair<token::TokenType, std::string>> expected = {
        {token::TokenType::ASSIGN, "="},
        {token::TokenType::PLUS, "+"},
        {token::TokenType::LPAREN, "("},
        {token::TokenType::RPAREN, ")"},
        {token::TokenType::LBRACE, "{"},
        {token::TokenType::RBRACE, "}"},
        {token::TokenType::COMMA, ","},
        {token::TokenType::SEMICOLON, ";"},
        {token::TokenType::END_OF_FILE, ""}
    };

    lexer::Lexer lexer(input);
    for (size_t i = 0; i < expected.size(); ++i) {
        token::Token tok = lexer.nextToken();
        EXPECT_EQ(tok.type, expected[i].first) << "Test[" << i << "] - tokentype wrong";
        EXPECT_EQ(tok.literal, expected[i].second) << "Test[" << i << "] - literal wrong";
    }
}
