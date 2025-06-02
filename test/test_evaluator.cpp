#include "evaluator/evaluator.h"
#include "lexer.h"
#include "object/Boolean.h"
#include "object/Integer.h"
#include "object/object.h"
#include "parser.h"
#include <gtest/gtest.h>
#include <string>

object::Object *testEval(std::string input);
void testIntegerObject(object::Object *obj, int expected);
void testBooleanObject(object::Object *obj, bool expected);

TEST(EvaluatorTest, EvalIntegerExpression) {
    struct IntegerTest {
        std::string input;
        int expected;
    };
    IntegerTest tests[4] = {
        {"5", 5},
        {"10", 10},
        {"-5", -5},
        {"-10", -10},
    };
    for (IntegerTest test : tests) {
        auto evaluated = testEval(test.input);
        testIntegerObject(evaluated, test.expected);
    }
}

TEST(EvaluatorTest, EvalBooleanExpression) {
    struct IntegerTest {
        std::string input;
        bool expected;
    };
    IntegerTest tests[2] = {
        {"true", true},
        {"false", false},
    };

    for (IntegerTest test : tests) {
        auto evaluated = testEval(test.input);
        testBooleanObject(evaluated, test.expected);
    }
}

TEST(EvaluatorTest, BangOperator) {
    struct IntegerTest {
        std::string input;
        bool expected;
    };
    IntegerTest tests[6] = {
        {"!true", false}, {"!false", true}, {"!5", false}, {"!!true", true}, {"!!false", false}, {"!!5", true},
    };

    for (IntegerTest test : tests) {
        auto evaluated = testEval(test.input);
        testBooleanObject(evaluated, test.expected);
    }
}

object::Object *testEval(std::string input) {
    auto lexer = std::make_unique<lexer::Lexer>(input);
    parser::Parser parser = parser::Parser(std::move(lexer));
    std::unique_ptr<ast::Program> program = parser.parseProgram();
    return evaluator::eval(program.get());
}

void testIntegerObject(object::Object *obj, int expected) {
    auto *result = dynamic_cast<object::Integer *>(obj);
    EXPECT_NE(result, nullptr) << "object is not an Integer. got=" << result << '\n';
    EXPECT_EQ(result->value, expected) << "object has wrong value. got=" << std::to_string(result->value)
                                       << " wanted=" << expected << '\n';
}

void testBooleanObject(object::Object *obj, bool expected) {
    auto *result = dynamic_cast<object::Boolean *>(obj);
    EXPECT_NE(result, nullptr) << "object is not a Boolean. got=" << result << '\n';
    EXPECT_EQ(result->value, expected) << "object has wrong value. got=" << std::to_string(result->value)
                                       << " wanted=" << expected << '\n';
}
