#include "lexer.h"
#include "token.h"
#include <gtest/gtest.h>

TEST(LexerTest, NextToken) {
    std::string input = R"(let five = 5;
    let ten = 10;
    let add = fn(x, y) {
        x + y;
    };
    let result = add(five, ten);
    !-/*5;
    5 < 10 > 5;

    if (5 < 10) {
        return true;
    } else {
        return false;
    }
    10 == 10;
    10 != 9;
    )";

    std::vector<std::pair<token::TokenType, std::string>> expected = {
        {token::TokenType::LET, "let"},
        {token::TokenType::IDENT, "five"},
        {token::TokenType::ASSIGN, "="},
        {token::TokenType::INT, "5"},
        {token::TokenType::SEMICOLON, ";"},
        {token::TokenType::LET, "let"},
        {token::TokenType::IDENT, "ten"},
        {token::TokenType::ASSIGN, "="},
        {token::TokenType::INT, "10"},
        {token::TokenType::SEMICOLON, ";"},
        {token::TokenType::LET, "let"},
        {token::TokenType::IDENT, "add"},
        {token::TokenType::ASSIGN, "="},
        {token::TokenType::FUNCTION, "fn"},
        {token::TokenType::LPAREN, "("},
        {token::TokenType::IDENT, "x"},
        {token::TokenType::COMMA, ","},
        {token::TokenType::IDENT, "y"},
        {token::TokenType::RPAREN, ")"},
        {token::TokenType::LBRACE, "{"},
        {token::TokenType::IDENT, "x"},
        {token::TokenType::PLUS, "+"},
        {token::TokenType::IDENT, "y"},
        {token::TokenType::SEMICOLON, ";"},
        {token::TokenType::RBRACE, "}"},
        {token::TokenType::SEMICOLON, ";"},
        {token::TokenType::LET, "let"},
        {token::TokenType::IDENT, "result"},
        {token::TokenType::ASSIGN, "="},
        {token::TokenType::IDENT, "add"},
        {token::TokenType::LPAREN, "("},
        {token::TokenType::IDENT, "five"},
        {token::TokenType::COMMA, ","},
        {token::TokenType::IDENT, "ten"},
        {token::TokenType::RPAREN, ")"},
        {token::TokenType::SEMICOLON, ";"},
		{token::TokenType::BANG, "!"},
		{token::TokenType::MINUS, "-"},
		{token::TokenType::SLASH, "/"},
		{token::TokenType::ASTERISK, "*"},
		{token::TokenType::INT, "5"},
		{token::TokenType::SEMICOLON, ";"},
		{token::TokenType::INT, "5"},
		{token::TokenType::LT, "<"},
		{token::TokenType::INT, "10"},
		{token::TokenType::GT, ">"},
		{token::TokenType::INT, "5"},
		{token::TokenType::SEMICOLON, ";"},
		{token::TokenType::IF, "if"},
		{token::TokenType::LPAREN, "("},
		{token::TokenType::INT, "5"},
		{token::TokenType::LT, "<"},
		{token::TokenType::INT, "10"},
		{token::TokenType::RPAREN, ")"},
		{token::TokenType::LBRACE, "{"},
		{token::TokenType::RETURN, "return"},
		{token::TokenType::TRUE, "true"},
		{token::TokenType::SEMICOLON, ";"},
		{token::TokenType::RBRACE, "}"},
		{token::TokenType::ELSE, "else"},
		{token::TokenType::LBRACE, "{"},
		{token::TokenType::RETURN, "return"},
		{token::TokenType::FALSE, "false"},
		{token::TokenType::SEMICOLON, ";"},
		{token::TokenType::RBRACE, "}"},
		{token::TokenType::INT, "10"},
		{token::TokenType::EQ, "=="},
		{token::TokenType::INT, "10"},
		{token::TokenType::SEMICOLON, ";"},
		{token::TokenType::INT, "10"},
		{token::TokenType::NOT_EQ, "!="},
		{token::TokenType::INT, "9"},
		{token::TokenType::SEMICOLON, ";"},
		{token::TokenType::END_OF_FILE, ""},
    };

    lexer::Lexer lexer(input);
    for (size_t i = 0; i < expected.size(); ++i) {
        token::Token tok = lexer.nextToken();
        EXPECT_EQ(tok.type, expected[i].first)
            << "Test[" << i << "] - tokentype wrong. " << '\n'
            << "token type given: " << token::tokenTypeToString(tok.type)
            << '\n'
            << "token type expected: "
            << token::tokenTypeToString(expected[i].first);
        EXPECT_EQ(tok.literal, expected[i].second)
            << "Test[" << i << "] - literal wrong.";
    }
}
