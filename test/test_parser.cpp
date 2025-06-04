#include "ast/ArrayLiteral.h"
#include "ast/Boolean.h"
#include "ast/CallExpression.h"
#include "ast/Expression.h"
#include "ast/ExpressionStatement.h"
#include "ast/FunctionLiteral.h"
#include "ast/Identifier.h"
#include "ast/IfExpression.h"
#include "ast/InfixExpression.h"
#include "ast/IntegerLiteral.h"
#include "ast/LetStatement.h"
#include "ast/PrefixExpression.h"
#include "ast/Program.h"
#include "ast/ReturnStatement.h"
#include "ast/Statement.h"
#include "ast/StringLiteral.h"
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
    struct LetStatements {
        std::string input;
        std::string expectedIdentifier;
        std::variant<int, std::string, bool> expectedValue;
    };
    LetStatements tests[3] = {
        {"let x = 5;", "x", 5},
        {"let y = true;", "y", true},
        {"let foobar = y;", "foobar", "y"},
    };

    for (LetStatements test : tests) {
        auto lexer = std::make_unique<lexer::Lexer>(test.input);
        parser::Parser parser = parser::Parser(std::move(lexer));

        std::unique_ptr<ast::Program> program = parser.parseProgram();
        checkParserErrors(&parser);

        EXPECT_NE(program, nullptr) << "program is a nullptr :(" << '\n';
        EXPECT_EQ(program->statements.size(), 1)
            << "program statement size isn't correct: " << program->statements.size() << '\n';

        auto *statement = program->statements[0].get();
        testLetStatement(statement, test.expectedIdentifier);
        auto *letStatement = dynamic_cast<ast::LetStatement *>(statement);
        testLiteralExpression(letStatement->value.get(), test.expectedValue);
    }
}

TEST(ParserTest, ReturnStatement) {
    struct Return {
        std::string input;
        std::variant<int, std::string, bool> expectedValue;
    };
    Return tests[3] = {
        {"return 5;", 5},
        {"return true;", true},
        {"return foobar;", "foobar"},
    };

    for (Return test : tests) {
        auto lexer = std::make_unique<lexer::Lexer>(test.input);
        parser::Parser parser = parser::Parser(std::move(lexer));

        std::unique_ptr<ast::Program> program = parser.parseProgram();
        checkParserErrors(&parser);

        EXPECT_NE(program, nullptr) << "program is a nullptr :(" << '\n';
        EXPECT_EQ(program->statements.size(), 1)
            << "program statement size isn't correct: " << program->statements.size() << '\n';

        auto *statement = program->statements[0].get();
        auto *returnStatement = dynamic_cast<ast::ReturnStatement *>(statement);
        EXPECT_NE(returnStatement, nullptr) << "statement is not a ReturnStatement" << '\n';
        EXPECT_EQ(returnStatement->tokenLiteral(), "return")
            << "returnStatement->tokenLiteral() is not 'return', got=" << returnStatement->tokenLiteral() << '\n';
        testLiteralExpression(returnStatement->returnValue.get(), test.expectedValue);
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
        std::variant<int, std::string, bool> value;
    };
    Prefix prefixTests[4] = {
        {"!5;", "!", 5},
        {"-15;", "-", 15},
        {"!true;", "!", true},
        {"!false;", "!", false},
    };

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
        testLiteralExpression(rightExp, pre.value);
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
    TestStruct tests[25] = {
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
        {
            "1 + (2 + 3) + 4",
            "((1 + (2 + 3)) + 4)\n",
        },
        {
            "(5 + 5) * 2",
            "((5 + 5) * 2)\n",
        },
        {
            "2 / (5 + 5)",
            "(2 / (5 + 5))\n",
        },
        {
            "-(5 + 5)",
            "(-(5 + 5))\n",
        },
        {
            "!(true == true)",
            "(!(true == true))\n",
        },
        {
            "a + add(b * c) + d",
            "((a + add((b * c))) + d)\n",
        },
        {
            "add(a, b, 1, 2 * 3, 4 + 5, add(6, 7 * 8))",
            "add(a, b, 1, (2 * 3), (4 + 5), add(6, (7 * 8)))\n",
        },
        {
            "add(a + b + c * d / f + g)",
            "add((((a + b) + ((c * d) / f)) + g))\n",
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

TEST(ParserTest, IfExpression) {
    std::string input = "if (x < y) { x }";
    auto lexer = std::make_unique<lexer::Lexer>(input);
    parser::Parser parser = parser::Parser(std::move(lexer));
    std::unique_ptr<ast::Program> program = parser.parseProgram();
    checkParserErrors(&parser);

    EXPECT_NE(program, nullptr) << "program is a nullptr :(" << '\n';
    EXPECT_EQ(program->statements.size(), 1)
        << "program statement size isn't correct: " << program->statements.size() << '\n';

    auto *statement = program->statements[0].get();
    auto *expressionStatement = dynamic_cast<ast::ExpressionStatement *>(statement);
    EXPECT_NE(expressionStatement, nullptr) << "the statement[0] is not an ExpressionStatement" << '\n';

    auto express = expressionStatement->expression.get();
    auto *expression = dynamic_cast<ast::IfExpression *>(express);
    EXPECT_NE(expression, nullptr) << "the expression is not an IfExpression" << '\n';

    testInfixExpression(expression->condition.get(), "x", "<", "y");
    EXPECT_EQ(expression->consiquence->statements.size(), 1)
        << "consiquence statement size isn't correct: " << expression->consiquence->statements.size() << '\n';

    auto *consiquence = dynamic_cast<ast::ExpressionStatement *>(expression->consiquence->statements[0].get());
    EXPECT_NE(consiquence, nullptr) << "the consiquence is not an ExpressionStatement" << '\n';

    testIdentifier(consiquence->expression.get(), "x");
    EXPECT_EQ(expression->alternative, nullptr) << "expression->alternative should be a nullptr here" << '\n';
}

TEST(ParserTest, IfElseExpression) {
    std::string input = "if (x > y) { x } else { y }";
    auto lexer = std::make_unique<lexer::Lexer>(input);
    parser::Parser parser = parser::Parser(std::move(lexer));
    std::unique_ptr<ast::Program> program = parser.parseProgram();
    checkParserErrors(&parser);

    EXPECT_NE(program, nullptr) << "program is a nullptr :(" << '\n';
    EXPECT_EQ(program->statements.size(), 1)
        << "program statement size isn't correct: " << program->statements.size() << '\n';

    auto *statement = program->statements[0].get();
    auto *expressionStatement = dynamic_cast<ast::ExpressionStatement *>(statement);
    EXPECT_NE(expressionStatement, nullptr) << "the statement[0] is not an ExpressionStatement" << '\n';

    auto express = expressionStatement->expression.get();
    auto *expression = dynamic_cast<ast::IfExpression *>(express);
    EXPECT_NE(expression, nullptr) << "the expression is not an IfExpression" << '\n';

    testInfixExpression(expression->condition.get(), "x", ">", "y");
    EXPECT_EQ(expression->consiquence->statements.size(), 1)
        << "consiquence statement size isn't correct: " << expression->consiquence->statements.size() << '\n';

    auto *consiquence = dynamic_cast<ast::ExpressionStatement *>(expression->consiquence->statements[0].get());
    EXPECT_NE(consiquence, nullptr) << "the consiquence is not an ExpressionStatement" << '\n';

    testIdentifier(consiquence->expression.get(), "x");
    EXPECT_EQ(expression->alternative->statements.size(), 1)
        << "alternative statement size isn't correct: " << expression->alternative->statements.size() << '\n';

    auto *alternative = dynamic_cast<ast::ExpressionStatement *>(expression->alternative->statements[0].get());
    EXPECT_NE(alternative, nullptr) << "the alternative is not an ExpressionStatement" << '\n';

    testIdentifier(alternative->expression.get(), "y");
}

TEST(ParserTest, FunctionLiteralParsing) {
    std::string input = "fn(x, y) { x + y; }";
    auto lexer = std::make_unique<lexer::Lexer>(input);
    parser::Parser parser = parser::Parser(std::move(lexer));
    std::unique_ptr<ast::Program> program = parser.parseProgram();
    checkParserErrors(&parser);
    EXPECT_NE(program, nullptr) << "program is a nullptr :(" << '\n';
    EXPECT_EQ(program->statements.size(), 1)
        << "program statement size isn't correct: " << program->statements.size() << '\n';

    auto *statement = program->statements[0].get();
    auto *expressionStatement = dynamic_cast<ast::ExpressionStatement *>(statement);
    EXPECT_NE(expressionStatement, nullptr) << "the statement[0] is not an ExpressionStatement" << '\n';

    auto *function = dynamic_cast<ast::FunctionLiteral *>(expressionStatement->expression.get());
    EXPECT_NE(function, nullptr) << "expressionStatement->expression is not a FunctionLiteral" << '\n';
    EXPECT_EQ(function->parameters.size(), 2)
        << "function parameters wrong. wanted 2 but got=" << function->parameters.size() << '\n';
    testLiteralExpression(function->parameters[0].get(), "x");
    testLiteralExpression(function->parameters[1].get(), "y");

    EXPECT_EQ(function->body->statements.size(), 1)
        << "function body statements is not 1. got=" << function->body->statements.size() << '\n';

    auto *bodyStatement = dynamic_cast<ast::ExpressionStatement *>(function->body->statements[0].get());
    EXPECT_NE(expressionStatement, nullptr) << "function body statement is not of type ExpressionStatement" << '\n';

    testInfixExpression(bodyStatement->expression.get(), "x", "+", "y");
}

TEST(ParserTest, ParameterParsing) {
    struct Params {
        std::string input;
        std::vector<std::string> expectedParams;
    };
    Params tests[3] = {
        {"fn() {};", {}},
        {"fn(x) {};", {"x"}},
        {"fn(x, y, z) {};", {"x", "y", "z"}},
    };

    for (Params test : tests) {
        auto lexer = std::make_unique<lexer::Lexer>(test.input);
        parser::Parser parser = parser::Parser(std::move(lexer));
        std::unique_ptr<ast::Program> program = parser.parseProgram();
        checkParserErrors(&parser);
        auto *statement = program->statements[0].get();
        auto *expressionStatement = dynamic_cast<ast::ExpressionStatement *>(statement);
        auto *function = dynamic_cast<ast::FunctionLiteral *>(expressionStatement->expression.get());
        EXPECT_EQ(function->parameters.size(), test.expectedParams.size())
            << "length of parameters wrong. wanted " << test.expectedParams.size()
            << ", got=" << function->parameters.size() << '\n';
        for (size_t i = 0; i < test.expectedParams.size(); i++) {
            testLiteralExpression(function->parameters[i].get(), test.expectedParams[i]);
        }
    }
}

TEST(ParserTest, ExpressionParsing) {
    std::string input = "add(1, 2 * 3, 4 + 5);";
    auto lexer = std::make_unique<lexer::Lexer>(input);
    parser::Parser parser = parser::Parser(std::move(lexer));
    std::unique_ptr<ast::Program> program = parser.parseProgram();
    checkParserErrors(&parser);

    EXPECT_NE(program, nullptr) << "program is a nullptr :(" << '\n';
    EXPECT_EQ(program->statements.size(), 1)
        << "program statement size isn't correct: " << program->statements.size() << '\n';
    auto *statement = program->statements[0].get();
    auto *expressionStatement = dynamic_cast<ast::ExpressionStatement *>(statement);
    EXPECT_NE(expressionStatement, nullptr) << "the statement[0] is not an ExpressionStatement" << '\n';

    auto *callExpression = dynamic_cast<ast::CallExpression *>(expressionStatement->expression.get());
    EXPECT_NE(callExpression, nullptr) << "expressionStatement->expression is not a CallExpression" << '\n';

    testIdentifier(callExpression->function.get(), "add");
    EXPECT_EQ(callExpression->arguments.size(), 3)
        << "callExpression->arguments is the wrong length. got=" << callExpression->arguments.size() << '\n';
    testLiteralExpression(callExpression->arguments[0].get(), 1);
    testInfixExpression(callExpression->arguments[1].get(), 2, "*", 3);
    testInfixExpression(callExpression->arguments[2].get(), 4, "+", 5);
}

TEST(ParserTest, ExpressionParameterParsing) {
    struct Params {
        std::string input;
        std::string expectedIdent;
        std::vector<std::string> expectedArgs;
    };
    Params tests[3] = {
        {"add();", "add", {}},
        {"add(1);", "add", {"1"}},
        {"add(1, 2 * 3, 4 + 5);", "add", {"1", "(2 * 3)", "(4 + 5)"}},
    };
    for (Params test : tests) {
        auto lexer = std::make_unique<lexer::Lexer>(test.input);
        parser::Parser parser = parser::Parser(std::move(lexer));
        std::unique_ptr<ast::Program> program = parser.parseProgram();
        checkParserErrors(&parser);
        auto *statement = program->statements[0].get();
        auto *expressionStatement = dynamic_cast<ast::ExpressionStatement *>(statement);
        auto *callExpression = dynamic_cast<ast::CallExpression *>(expressionStatement->expression.get());
        EXPECT_NE(callExpression, nullptr) << "expressionStatement->expression is not a CallExpression" << '\n';
        testIdentifier(callExpression->function.get(), test.expectedIdent);
        EXPECT_EQ(callExpression->arguments.size(), test.expectedArgs.size())
            << "callExpression->arguments is the wrong length. got=" << callExpression->arguments.size()
            << ", wanted=" << test.expectedArgs.size() << '\n';
        for (size_t i = 0; i < test.expectedArgs.size(); i++) {
            EXPECT_EQ(callExpression->arguments[i]->toString(), test.expectedArgs[i])
                << "argument " << i << "wrong. want=" << test.expectedArgs[i]
                << ", got=" << callExpression->arguments[i]->toString() << '\n';
        }
    }
}
TEST(ParserTest, StringLiteralExpression) {
    std::string input = "\"hello world\"";
    auto lexer = std::make_unique<lexer::Lexer>(input);
    parser::Parser parser = parser::Parser(std::move(lexer));
    std::unique_ptr<ast::Program> program = parser.parseProgram();
    checkParserErrors(&parser);

    EXPECT_NE(program, nullptr) << "program is a nullptr :(" << '\n';
    EXPECT_EQ(program->statements.size(), 1)
        << "program statement size isn't correct: " << program->statements.size() << '\n';
    auto *statement = program->statements[0].get();
    auto *expressionStatement = dynamic_cast<ast::ExpressionStatement *>(statement);
    auto *stringLit = dynamic_cast<ast::StringLiteral *>(expressionStatement->expression.get());
    EXPECT_NE(stringLit, nullptr) << "the statement[0] is not an StringLiteral" << '\n';

    EXPECT_EQ(stringLit->valueString, "hello world")
        << "literal.value is not of " << "hello world" << " got=" << stringLit->valueString << '\n';
}

TEST(ParserTest, ParsingArrayLiterals) {
    std::string input = "[1, 2 * 2, 3 + 3]";
    auto lexer = std::make_unique<lexer::Lexer>(input);
    parser::Parser parser = parser::Parser(std::move(lexer));
    std::unique_ptr<ast::Program> program = parser.parseProgram();
    checkParserErrors(&parser);

    EXPECT_NE(program, nullptr) << "program is a nullptr :(" << '\n';

    auto *statement = program->statements[0].get();
    auto *expressionStatement = dynamic_cast<ast::ExpressionStatement *>(statement);
    auto *arrayLit = dynamic_cast<ast::ArrayLiteral *>(expressionStatement->expression.get());
    ASSERT_NE(arrayLit, nullptr) << "the statement[0] is not an ArrayLiteral" << '\n';

    EXPECT_EQ(arrayLit->elements.size(), 3) << "arrayLit size is not 3. got=" << arrayLit->elements.size() << '\n';
    testIntegerLiteral(arrayLit->elements[0].get(), 1);
    testInfixExpression(arrayLit->elements[1].get(), 2 , "*", 2);
    testInfixExpression(arrayLit->elements[2].get(), 3 , "+", 3);
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
