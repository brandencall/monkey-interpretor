#include "ast/Boolean.h"
#include "ast/Expression.h"
#include "ast/ExpressionStatement.h"
#include "ast/Identifier.h"
#include "ast/InfixExpression.h"
#include "ast/IntegerLiteral.h"
#include "ast/LetStatement.h"
#include "ast/PrefixExpression.h"
#include "ast/Program.h"
#include "ast/ReturnStatement.h"
#include "ast/Statement.h"
#include "lexer.h"
#include "parser.h"
#include "token.h"
#include <cstddef>
#include <gtest/gtest.h>
#include <iostream>
#include <memory>
#include <sstream>
#include <string>
#include <type_traits>
#include <utility>
#include <variant>
#include <vector>

void testLetStatement(ast::Statement *statement, std::string &name);
void checkParserErrors(parser::Parser *parser);
std::string joinErrors(const std::vector<std::string> &errors);
void testIntegerLiteral(const ast::Expression *expression, int value);
void testIdentifier(const ast::Expression *expression, std::string value);
void testBooleanLiteral(const ast::Expression *expression, bool value);
void testLiteralExpression(const ast::Expression *expression, const std::variant<int, std::string, bool> &expected);
void testInfixExpression(const ast::Expression *exp, const std::variant<int, std::string, bool> &left,
                         const std::string &operator_, const std::variant<int, std::string, bool> &right);

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

TEST(ParserTest, BooleanExpression) {
    std::string input = "true;";

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

    auto *boolean = dynamic_cast<ast::Boolean *>(expression->expression.get());
    EXPECT_NE(boolean, nullptr) << "the expression->expression.get is not of type Boolean" << '\n';

    EXPECT_EQ(boolean->valueBool, true) << "the identifier value is not true it is: " << boolean->value << '\n';

    EXPECT_EQ(boolean->tokenLiteral(), "true")
        << "the identifier tokenLiteral is not true it is: " << boolean->tokenLiteral() << '\n';
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

TEST(ParserTest, InfixExpressions) {
    struct Infix {
        std::string input;
        std::variant<int, std::string, bool> leftValue;
        std::string oper;
        std::variant<int, std::string, bool> rightValue;
    };
    Infix infixTests[11] = {
        {"5 + 5;", 5, "+", 5},
        {"5 - 5;", 5, "-", 5},
        {"5 * 5;", 5, "*", 5},
        {"5 / 5;", 5, "/", 5},
        {"5 > 5;", 5, ">", 5},
        {"5 < 5;", 5, "<", 5},
        {"5 == 5;", 5, "==", 5},
        {"5 != 5;", 5, "!=", 5},
        {"true == true", true, "==", true},
        {"true != false", true, "!=", false},
        {"false == false", false, "==", false},
    };

    for (Infix infix : infixTests) {
        auto lexer = std::make_unique<lexer::Lexer>(infix.input);
        parser::Parser parser = parser::Parser(std::move(lexer));

        std::unique_ptr<ast::Program> program = parser.parseProgram();
        checkParserErrors(&parser);

        EXPECT_NE(program, nullptr) << "program is a nullptr :(" << '\n';
        EXPECT_EQ(program->statements.size(), 1)
            << "program statement size isn't correct: " << program->statements.size() << '\n';

        auto *statement = program->statements[0].get();
        auto *expression = dynamic_cast<ast::ExpressionStatement *>(statement);
        EXPECT_NE(expression, nullptr) << "the statement[0] is not an ExpressionStatement" << '\n';

        auto *exp = dynamic_cast<ast::InfixExpression *>(expression->expression.get());
        EXPECT_NE(exp, nullptr) << "the expression->expression.get is not of type InfixExpression" << '\n';

        EXPECT_EQ(exp->oper, infix.oper) << "exp.oper is not " << exp->oper << ". got=" << exp->oper << '\n';

        ast::Expression *leftExp = exp->left.get();
        testLiteralExpression(leftExp, infix.leftValue);

        EXPECT_EQ(exp->oper, infix.oper) << "exp.oper is not " << infix.oper << ". got=" << exp->oper << '\n';

        ast::Expression *rightExp = exp->right.get();
        testLiteralExpression(rightExp, infix.rightValue);
    }
}

TEST(ParserTest, OperatorPrecedenceParsing) {
    struct TestStruct {
        std::string input;
        std::string expected;
    };
    TestStruct tests[17] = {
        {
            "true",
            "true\n",
        },
        {
            "false",
            "false\n",
        },
        {
            "3 > 5 == false",
            "((3 > 5) == false)\n",
        },
        {
            "3 < 5 == true",
            "((3 < 5) == true)\n",
        },
        {
            "-a * b",
            "((-a) * b)\n",
        },
        {
            "!-a",
            "(!(-a))\n",
        },
        {
            "a + b + c",
            "((a + b) + c)\n",
        },
        {
            "a + b - c",
            "((a + b) - c)\n",
        },
        {
            "a * b * c",
            "((a * b) * c)\n",
        },
        {
            "a * b / c",
            "((a * b) / c)\n",
        },
        {
            "a + b / c",
            "(a + (b / c))\n",
        },
        {
            "a + b * c + d / e - f",
            "(((a + (b * c)) + (d / e)) - f)\n",
        },
        {
            "3 + 4; -5 * 5",
            "(3 + 4)\n((-5) * 5)\n",
        },
        {
            "5 > 4 == 3 < 4",
            "((5 > 4) == (3 < 4))\n",
        },
        {
            "5 < 4 != 3 > 4",
            "((5 < 4) != (3 > 4))\n",
        },
        {
            "3 + 4 * 5 == 3 * 1 + 4 * 5",
            "((3 + (4 * 5)) == ((3 * 1) + (4 * 5)))\n",
        },
        {
            "3 + 4 * 5 == 3 * 1 + 4 * 5",
            "((3 + (4 * 5)) == ((3 * 1) + (4 * 5)))\n",
        },
    };

    for (TestStruct test : tests) {
        auto lexer = std::make_unique<lexer::Lexer>(test.input);
        parser::Parser parser = parser::Parser(std::move(lexer));
        std::unique_ptr<ast::Program> program = parser.parseProgram();
        checkParserErrors(&parser);

        std::string actual = program->toString();
        EXPECT_EQ(actual, test.expected) << "expected=" << test.expected << ", got=" << actual << '\n';
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

void testIntegerLiteral(const ast::Expression *expression, int value) {

    auto *integer = dynamic_cast<const ast::IntegerLiteral *>(expression);
    EXPECT_NE(integer, nullptr) << "expression is not an IntegerLiteral. got=" << typeid(*expression).name() << '\n';

    EXPECT_EQ(integer->valueInt, value) << "integer->valueInt is not " << value << ". got=" << integer->valueInt
                                        << '\n';
    EXPECT_EQ(integer->tokenLiteral(), std::to_string(value))
        << "integer->tokenLiteral() is not " << value << ". got=" << integer->tokenLiteral() << '\n';
}

void testIdentifier(const ast::Expression *expression, std::string value) {
    auto *ident = dynamic_cast<const ast::Identifier *>(expression);
    EXPECT_NE(ident, nullptr) << "expression is not an Identifier. got=" << typeid(*expression).name() << '\n';
    EXPECT_EQ(ident->value, value) << "ident->value is not " << value << ". got=" << ident->value << '\n';
    EXPECT_EQ(ident->tokenLiteral(), value) << "ident->value is not " << value << ". got=" << ident->value << '\n';
}

void testBooleanLiteral(const ast::Expression *expression, bool value) {
    auto *boolean = dynamic_cast<const ast::Boolean *>(expression);
    EXPECT_NE(boolean, nullptr) << "expression is not Boolean. got=" << typeid(*expression).name() << '\n';
    EXPECT_EQ(boolean->valueBool, value) << "boolean->valueBool is not " << value << ". got=" << boolean->valueBool
                                         << '\n';
}

void testLiteralExpression(const ast::Expression *expression, const std::variant<int, std::string, bool> &expected) {
    std::visit(
        [&](const auto &value) {
            using T = std::decay_t<decltype(value)>;
            if constexpr (std::is_same_v<T, int>) {
                testIntegerLiteral(expression, value);
            } else if constexpr (std::is_same_v<T, std::string>) {
                testIdentifier(expression, value);
            } else if (std::is_same_v<T, bool>) {
                testBooleanLiteral(expression, value);
            } else {
                ADD_FAILURE() << "Type of expected not handled. Got type: " << typeid(value).name();
            }
        },
        expected);
}

void testInfixExpression(const ast::Expression *exp, const std::variant<int, std::string, bool> &left,
                         const std::string &operator_, const std::variant<int, std::string, bool> &right) {
    auto *opExp = dynamic_cast<const ast::InfixExpression *>(exp);
    EXPECT_NE(opExp, nullptr) << "expression is not an InfixExpression. got=" << typeid(*opExp).name() << '\n';
    testLiteralExpression(opExp->left.get(), left);
    EXPECT_EQ(opExp->oper, operator_) << "Operator mismatch. Got: " << opExp->oper << ", Expected: " << operator_;
    testLiteralExpression(opExp->right.get(), right);
}
