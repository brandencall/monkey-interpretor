#include "evaluator/evaluator.h"
#include "lexer.h"
#include "object/Boolean.h"
#include "object/Integer.h"
#include "object/object.h"
#include "parser.h"
#include <gtest/gtest.h>
#include <optional>
#include <string>

object::Object *testEval(std::string input);
void testIntegerObject(object::Object *obj, int expected);
void testBooleanObject(object::Object *obj, bool expected);
void testNullObject(object::Object *obj);

TEST(EvaluatorTest, EvalIntegerExpression) {
    struct IntegerTest {
        std::string input;
        int expected;
    };
    IntegerTest tests[15] = {
        {"5", 5},
        {"10", 10},
        {"-5", -5},
        {"-10", -10},
        {"5 + 5 + 5 + 5 - 10", 10},
        {"2 * 2 * 2 * 2 * 2", 32},
        {"-50 + 100 + -50", 0},
        {"5 * 2 + 10", 20},
        {"5 + 2 * 10", 25},
        {"20 + 2 * -10", 0},
        {"50 / 2 * 2 + 10", 60},
        {"2 * (5 + 10)", 30},
        {"3 * 3 * 3 + 10", 37},
        {"3 * (3 * 3) + 10", 37},
        {"(5 + 10 * 2 + 15 / 3) * 2 + -10", 50},
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
    IntegerTest tests[19] = {
        {"true", true},
        {"false", false},
        {"1 < 2", true},
        {"1 > 2", false},
        {"1 < 1", false},
        {"1 > 1", false},
        {"1 == 1", true},
        {"1 != 1", false},
        {"1 == 2", false},
        {"1 != 2", true},
        {"true == true", true},
        {"false == false", true},
        {"true == false", false},
        {"true != false", true},
        {"false != true", true},
        {"(1 < 2) == true", true},
        {"(1 < 2) == false", false},
        {"(1 > 2) == true", false},
        {"(1 > 2) == false", true},
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

TEST(EvaluatorTest, IfElseExpressions) {
    struct IfElseTest {
        std::string input;
        std::optional<int> expected;
    };
    IfElseTest tests[7] = {
        {"if (true) { 10 }", 10},
        {"if (false) { 10 }", std::nullopt},
        {"if (1) { 10 }", 10},
        {"if (1 < 2) { 10 }", 10},
        {"if (1 > 2) { 10 }", std::nullopt},
        {"if (1 > 2) { 10 } else { 20 }", 20},
        {"if (1 < 2) { 10 } else { 20 }", 10},
    };
    for (IfElseTest test : tests) {
        auto evaluated = testEval(test.input);
        if (test.expected.has_value()) {
            testIntegerObject(evaluated, test.expected.value());
        } else {
            testNullObject(evaluated);
        }
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

void testNullObject(object::Object *obj) { EXPECT_EQ(obj->type(), object::ObjectType::NULL_OBJ) << "object is not nullptr." << '\n'; }






