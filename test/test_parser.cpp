#include "ast/Expression.h"
#include "ast/Identifier.h"
#include "ast/LetStatement.h"
#include "ast/Program.h"
#include "ast/ReturnStatement.h"
#include "ast/Statement.h"
#include "lexer.h"
#include "parser.h"
#include "token.h"
#include <cstddef>
#include <gtest/gtest.h>
#include <memory>
#include <sstream>
#include <string>
#include <utility>
#include <vector>

void testLetStatement(ast::Statement *statement, std::string &name);
void checkParserErrors(parser::Parser *parser);
std::string joinErrors(const std::vector<std::string> &errors);

TEST(ParserTest, LetStatements) {
    std::string input = R"(
    let x = 5;
    let y = 10;
    let foobar = 838383;
    )";

    auto lexer = std::make_unique<lexer::Lexer>(input);
    parser::Parser parser = parser::Parser(std::move(lexer));

    std::unique_ptr<ast::Program> program = parser.parseProgram();
    checkParserErrors(&parser);

    EXPECT_NE(program, nullptr) << "program is a nullptr :(";
    EXPECT_EQ(program->statements.size(), 3) << "program size isn't correct";

    std::vector<std::string> expected = {
        "x",
        "y",
        "foobar",
    };

    for (size_t i = 0; i < expected.size(); ++i) {
        ast::Statement *statement = program->statements[i].get();
        testLetStatement(statement, expected[i]);
    }
}

TEST(ParserTest, ReturnStatement) {
    std::string input = R"(
    return 5;
    return 10;
    return 993322;
    )";

    auto lexer = std::make_unique<lexer::Lexer>(input);
    parser::Parser parser = parser::Parser(std::move(lexer));

    std::unique_ptr<ast::Program> program = parser.parseProgram();
    checkParserErrors(&parser);

    EXPECT_NE(program, nullptr) << "program is a nullptr :(";
    EXPECT_EQ(program->statements.size(), 3) << "program size isn't correct";

    for (const auto &statement : program->statements) {
        auto *returnStatement =
            dynamic_cast<ast::ReturnStatement *>(statement.get());
        EXPECT_NE(returnStatement, nullptr)
            << "statement not ast::ReturnStatement. got="
            << typeid(statement).name();
        EXPECT_EQ(returnStatement->tokenLiteral(), "return")
            << "returnStatement.tokenLiteral() not 'return' got"
            << returnStatement->tokenLiteral();
    }
}

TEST(ParserTest, TestString) {
    ast::Program program = ast::Program();
    std::unique_ptr<ast::LetStatement> letStatement =
        std::make_unique<ast::LetStatement>();
    letStatement->name = std::make_unique<ast::Identifier>();
    letStatement->value = std::make_unique<ast::Identifier>();

    letStatement->token.type = token::TokenType::LET;
    letStatement->token.literal = "let";

    letStatement->name->token.type = token::TokenType::IDENT;
    letStatement->name->token.literal = "myvar";
    letStatement->name->value = "myvar";

    letStatement->value->token.type = token::TokenType::IDENT;
    letStatement->value->token.literal = "anotherVar";
    letStatement->value->value = "anotherVar";

    //    auto *ident = dynamic_cast<ast::Identifier
    //    *>(letStatement->value.get()); if (ident) {
    //        ident->value = "anotherVar";
    //    }

    program.statements.push_back(std::move(letStatement));

    EXPECT_EQ(program.toString(), "let myvar = anotherVar;\n")
        << "program.toString() wrong. got= " << program.toString();
}

void checkParserErrors(parser::Parser *parser) {
    const std::vector<std::string> *errors = parser->errors();
    EXPECT_NE(errors, nullptr) << "How is the errors null?" << '\n';
    EXPECT_EQ(errors->size(), 0) << "Parser had errrors:\n"
                                 << joinErrors(*errors);
}

std::string joinErrors(const std::vector<std::string> &errors) {

    std::stringstream ss;
    for (const auto &error : errors) {
        ss << error << "\n";
    }
    return ss.str();
}

void testLetStatement(ast::Statement *statement, std::string &name) {

    EXPECT_EQ(statement->tokenLiteral(), "let")
        << "statement.tokenLiteral() not 'let'. got="
        << statement->tokenLiteral();

    auto *letStatement = dynamic_cast<ast::LetStatement *>(statement);
    EXPECT_NE(letStatement, nullptr)
        << "statement not ast::LetStatement. got=" << typeid(*statement).name();

    EXPECT_EQ(letStatement->name->value, name)
        << "letStatement->name->value not '" << name
        << "' got=" << letStatement->name->value;

    EXPECT_EQ(letStatement->name->tokenLiteral(), name)
        << "letStatement->name->tokenLiteral not '" << name
        << "' got=" << letStatement->name->tokenLiteral();
}
