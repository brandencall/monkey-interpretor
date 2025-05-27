#include "ast/LetStatement.h"
#include "ast/Program.h"
#include "ast/Statement.h"
#include "lexer.h"
#include "parser.h"
#include <cstddef>
#include <gtest/gtest.h>
#include <iostream>
#include <memory>
#include <utility>

bool testLetStatement(ast::Statement *statement, std::string& name);

TEST(ParserTest, LetStatements) {
    std::string input = R"(
    let x = 5;
    let y = 10;
    let foobar = 838383;
    )";

    auto lexer = std::make_unique<lexer::Lexer>(input);
    parser::Parser parser = parser::Parser(std::move(lexer));

    std::unique_ptr<ast::Program> program = parser.parseProgram();
    EXPECT_NE(program, nullptr) << "program is a nullptr :(";
    EXPECT_EQ(program->statements.size(), 3) << "program size isn't correct";

    std::vector<std::string> expected = {
        "x",
        "y",
        "foobar",
    };

    for (size_t i = 0; i < expected.size(); ++i) {
        ast::Statement *statement = program->statements[i];
        if (!testLetStatement(statement, expected[i])) {
            return;
        }
    }
}

bool testLetStatement(ast::Statement *statement, std::string& name) {

    EXPECT_EQ(statement->tokenLiteral(), "let") << "statement.tokenLiteral() not 'let'. got=" << statement->tokenLiteral();

    auto* letStatement = dynamic_cast<ast::LetStatement*>(statement);
    EXPECT_NE(letStatement, nullptr) << "statement not ast::LetStatement. got=" << typeid(*statement).name();

    EXPECT_EQ(letStatement->name->value, name) << "letStatement->name->value not '" << name << "' got=" << letStatement->name->value;

    EXPECT_EQ(letStatement->name->tokenLiteral(), name) << "letStatement->name->tokenLiteral not '" << name << "' got=" << letStatement->name->tokenLiteral();

    return true;
}








