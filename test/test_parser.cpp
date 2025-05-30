#include "ast/Expression.h"
#include "ast/ExpressionStatement.h"
#include "ast/Identifier.h"
#include "ast/IntegerLiteral.h"
#include "ast/LetStatement.h"
#include "ast/PrefixExpression.h"
#include "ast/Program.h"
#include "ast/ReturnStatement.h"
#include "ast/Statement.h"
#include "lexer.h"
#include "parser.h"
#include "token.h"
#include <cmath>
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
void testIntegerLiteral(ast::Expression *expression, int value);

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
        auto *returnStatement = dynamic_cast<ast::ReturnStatement *>(statement.get());
        EXPECT_NE(returnStatement, nullptr) << "statement not ast::ReturnStatement. got=" << typeid(statement).name();
        EXPECT_EQ(returnStatement->tokenLiteral(), "return")
            << "returnStatement.tokenLiteral() not 'return' got" << returnStatement->tokenLiteral();
    }
}

TEST(ParserTest, TestString) {
    ast::Program program = ast::Program();
    std::unique_ptr<ast::LetStatement> letStatement = std::make_unique<ast::LetStatement>();
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

    program.statements.push_back(std::move(letStatement));

    EXPECT_EQ(program.toString(), "let myvar = anotherVar;\n")
        << "program.toString() wrong. got= " << program.toString();
}

TEST(ParserTest, IdentifierExpression) {
    std::string input = "foobar;";

    auto lexer = std::make_unique<lexer::Lexer>(input);
    parser::Parser parser = parser::Parser(std::move(lexer));

    std::unique_ptr<ast::Program> program = parser.parseProgram();
    checkParserErrors(&parser);

    EXPECT_NE(program, nullptr) << "program is a nullptr :(" << '\n';
    EXPECT_EQ(program->statements.size(), 1)
        << "program statement size isn't correct: " << program->statements.size() << '\n';

    auto *statement = program->statements[0].get();
    auto *expression = dynamic_cast<ast::ExpressionStatement *>(statement);
    EXPECT_NE(expression, nullptr) << "the statement[0] is not an ExpressionStatement" << '\n';

    auto *ident = dynamic_cast<ast::Identifier *>(expression->expression.get());
    EXPECT_NE(ident, nullptr) << "the expression->expression.get is not of type Identifier" << '\n';

    EXPECT_EQ(ident->value, "foobar") << "the identifier value is not foobar it is: " << ident->value << '\n';

    EXPECT_EQ(ident->tokenLiteral(), "foobar")
        << "the identifier tokenLiteral is not foobar it is: " << ident->tokenLiteral() << '\n';
}

TEST(ParserTest, LiteralExpression) {
    std::string input = "5;";

    auto lexer = std::make_unique<lexer::Lexer>(input);
    parser::Parser parser = parser::Parser(std::move(lexer));

    std::unique_ptr<ast::Program> program = parser.parseProgram();
    checkParserErrors(&parser);

    EXPECT_NE(program, nullptr) << "program is a nullptr :(" << '\n';
    EXPECT_EQ(program->statements.size(), 1)
        << "program statement size isn't correct: " << program->statements.size() << '\n';

    auto *statement = program->statements[0].get();
    auto *expression = dynamic_cast<ast::ExpressionStatement *>(statement);
    EXPECT_NE(expression, nullptr) << "the statement[0] is not an ExpressionStatement" << '\n';

    auto *ident = dynamic_cast<ast::IntegerLiteral *>(expression->expression.get());
    EXPECT_NE(ident, nullptr) << "the expression->expression.get is not of type IntegerLiteral" << '\n';

    EXPECT_EQ(ident->valueInt, 5) << "the identifier value is not 5 it is: " << ident->valueInt << '\n';

    EXPECT_EQ(ident->tokenLiteral(), "5")
        << "the identifier tokenLiteral is not 5 it is: " << ident->tokenLiteral() << '\n';
}

TEST(ParserTest, PrefixExpressions) {
    struct Prefix {
        std::string input;
        std::string oper;
        int integerValue;
    };
    Prefix prefixTests[2];
    prefixTests[0].input = "!5;";
    prefixTests[0].oper = "!";
    prefixTests[0].integerValue = 5;
    prefixTests[1].input = "-15;";
    prefixTests[1].oper = "-";
    prefixTests[1].integerValue = 15;

    for (Prefix pre : prefixTests) {
        auto lexer = std::make_unique<lexer::Lexer>(pre.input);
        parser::Parser parser = parser::Parser(std::move(lexer));

        std::unique_ptr<ast::Program> program = parser.parseProgram();
        checkParserErrors(&parser);

        EXPECT_NE(program, nullptr) << "program is a nullptr :(" << '\n';
        EXPECT_EQ(program->statements.size(), 1)
            << "program statement size isn't correct: " << program->statements.size() << '\n';

        auto *statement = program->statements[0].get();
        auto *expression = dynamic_cast<ast::ExpressionStatement *>(statement);
        EXPECT_NE(expression, nullptr) << "the statement[0] is not an ExpressionStatement" << '\n';

        auto *exp = dynamic_cast<ast::PrefixExpression *>(expression->expression.get());
        EXPECT_NE(exp, nullptr) << "the expression->expression.get is not of type PrefixExpression" << '\n';

        EXPECT_EQ(exp->oper, pre.oper) << "exp.oper is not " << exp->oper << ". got=" << exp->oper << '\n';

        ast::Expression *rightExp = exp->right.get();
        testIntegerLiteral(rightExp, pre.integerValue);
    }
}

void checkParserErrors(parser::Parser *parser) {
    const std::vector<std::string> *errors = parser->errors();
    EXPECT_NE(errors, nullptr) << "How is the errors null?" << '\n';
    EXPECT_EQ(errors->size(), 0) << "Parser had errrors:\n" << joinErrors(*errors);
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
        << "statement.tokenLiteral() not 'let'. got=" << statement->tokenLiteral() << '\n';

    auto *letStatement = dynamic_cast<ast::LetStatement *>(statement);
    EXPECT_NE(letStatement, nullptr) << "statement not ast::LetStatement. got=" << typeid(*statement).name() << '\n';

    EXPECT_EQ(letStatement->name->value, name)
        << "letStatement->name->value not '" << name << "' got=" << letStatement->name->value << '\n';

    EXPECT_EQ(letStatement->name->tokenLiteral(), name)
        << "letStatement->name->tokenLiteral not '" << name << "' got=" << letStatement->name->tokenLiteral() << '\n';
}

void testIntegerLiteral(ast::Expression *expression, int value) {

    auto *integer = dynamic_cast<ast::IntegerLiteral *>(expression);
    EXPECT_NE(integer, nullptr) << "expression is not an IntegerLiteral. got=" << typeid(*expression).name() << '\n';

    EXPECT_EQ(integer->valueInt, value) << "integer->valueInt is not " << value << ". got=" << integer->valueInt
                                        << '\n';
    EXPECT_EQ(integer->tokenLiteral(), std::to_string(value))
       << "integer->tokenLiteral() is not " << value << ". got=" << integer->tokenLiteral() << '\n';
}
