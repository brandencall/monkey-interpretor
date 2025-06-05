#include "evaluator/evaluator.h"
#include "lexer.h"
#include "object/Array.h"
#include "object/Boolean.h"
#include "object/Environment.h"
#include "object/Error.h"
#include "object/Function.h"
#include "object/Hash.h"
#include "object/Integer.h"
#include "object/String.h"
#include "object/object.h"
#include "parser.h"
#include <gtest/gtest.h>
#include <iostream>
#include <optional>
#include <string>
#include <vector>

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

TEST(EvaluatorTest, ReturnStatements) {
    struct ReturnTest {
        std::string input;
        int expected;
    };
    ReturnTest tests[4] = {
        {"return 10;", 10},
        {"return 10; 9;", 10},
        {"return 2 * 5; 9;", 10},
        {"9; return 2 * 5; 9;", 10},
    };
    for (ReturnTest test : tests) {
        auto evaluated = testEval(test.input);
        testIntegerObject(evaluated, test.expected);
    }
}

TEST(EvaluatorTest, ErrorHandling) {
    struct ErrTest {
        std::string input;
        std::string expectedMessage;
    };
    ErrTest tests[9] = {
        {
            "5 + true;",
            "type mismatch: INTEGER + BOOLEAN",
        },
        {
            "5 + true; 5;",
            "type mismatch: INTEGER + BOOLEAN",
        },
        {
            "-true",
            "unknown operator: -BOOLEAN",
        },
        {
            "true + false;",
            "unknown operator: BOOLEAN + BOOLEAN",
        },
        {
            "5; true + false; 5",
            "unknown operator: BOOLEAN + BOOLEAN",
        },
        {
            "if (10 > 1) { true + false; }",
            "unknown operator: BOOLEAN + BOOLEAN",
        },
        {
            R"(
                if (10 > 1) {
                    if (10 > 1) {
                        return true + false;
                    }
                    return 1;
                }
            )",
            "unknown operator: BOOLEAN + BOOLEAN",
        },
        {
            "foobar",
            "identifier not found: foobar",
        },
        {
            "\"Hello\" - \"World\"",
            "unknown operator: STRING - STRING",
        },
    };
    for (ErrTest test : tests) {
        auto evaluated = testEval(test.input);
        auto *errObj = dynamic_cast<object::Error *>(evaluated);
        EXPECT_NE(errObj, nullptr) << "object is not an Error. got=" << evaluated << '\n';
        EXPECT_EQ(errObj->message, test.expectedMessage)
            << "wrong error message. expected=" << test.expectedMessage << ", got=" << errObj->message << '\n';
    }
}

TEST(EvaluatorTest, LetStatement) {
    struct LetTest {
        std::string input;
        int expected;
    };
    LetTest tests[4] = {{"let a = 5; a;", 5},
                        {"let a = 5 * 5; a;", 25},
                        {"let a = 5; let b = a; b;", 5},
                        {"let a = 5; let b = a; let c = a + b + 5; c;", 15}};
    for (LetTest test : tests) {
        auto evaluated = testEval(test.input);
        testIntegerObject(evaluated, test.expected);
    }
}

TEST(EvaluatorTest, FunctionObject) {
    std::string input = "fn(x) { x + 2; };";
    auto evaluated = testEval(input);
    auto *func = dynamic_cast<object::Function *>(evaluated);
    EXPECT_NE(func, nullptr) << "function is not a Function. got=" << func << '\n';
    EXPECT_EQ(func->parameters.size(), 1)
        << "function parameters are no the right size. got=" << func->parameters.size() << " expected=1" << '\n';
    EXPECT_EQ(func->parameters[0]->toString(), "x")
        << "param is not x. got=" << func->parameters[0]->toString() << '\n';
    std::string expectedBody = "(x + 2)";
    EXPECT_EQ(func->body->toString(), expectedBody)
        << "body is not " << expectedBody << ", got=" << func->body->toString() << '\n';
}

TEST(EvaluatorTest, FunctionApplication) {
    struct FuncTest {
        std::string input;
        int expected;
    };
    FuncTest tests[6] = {
        {"let identity = fn(x) { x; }; identity(5);", 5},
        {"let identity = fn(x) { return x; }; identity(5);", 5},
        {"let double = fn(x) { x * 2; }; double(5);", 10},
        {"let add = fn(x, y) { x + y; }; add(5, 5);", 10},
        {"let add = fn(x, y) { x + y; }; add(5 + 5, add(5, 5));", 20},
        {"fn(x) { x; }(5)", 5},
    };
    for (FuncTest test : tests) {
        auto evaluated = testEval(test.input);
        testIntegerObject(evaluated, test.expected);
    }
}

TEST(EvaluatorTest, StringLiteral) {
    std::string input = "\"Hello World!\"";
    auto evaluated = testEval(input);
    auto *result = dynamic_cast<object::String *>(evaluated);
    EXPECT_NE(result, nullptr) << "object is not a String. got=" << evaluated << '\n';
    EXPECT_EQ(result->value, "Hello World!")
        << "object has wrong value. got=" << result->value << " wanted=Hello World!" << '\n';
}

TEST(EvaluatorTest, StringConcatenation) {
    std::string input = "\"Hello\" + \" \" + \"World!\"";
    auto evaluated = testEval(input);
    auto *result = dynamic_cast<object::String *>(evaluated);
    EXPECT_NE(result, nullptr) << "object is not a String. got=" << evaluated << '\n';
    EXPECT_EQ(result->value, "Hello World!")
        << "object has wrong value. got=" << result->value << " wanted=Hello World!" << '\n';
}

TEST(EvaluatorTest, BuiltinFunctions) {
    struct BuiltinTest {
        std::string input;
        const std::variant<int, std::string, std::vector<int>> expected;
    };
    BuiltinTest tests[14] = {
        {"len(\"\")", 0},
        {"len(\"four\")", 4},
        {"len(\"hello world\")", 11},
        {"len(1)", "argument to `len` not supported, got INTEGER"},
        {"len(\"one\", \"two\")", "wrong number of arguments. want=1 but got=2"},
        {"len([1, 2, 3])", 3},
        {"len([])", 0},
        {"first([1, 2, 3])", 1},
        {"first(1)", "argument to `first` must be ARRAY, got INTEGER"},
        {"last([1, 2, 3])", 3},
        {"last(1)", "argument to `last` must be ARRAY, got INTEGER"},
        {"rest([1, 2, 3])", std::vector<int>{2, 3}},
        {"push([], 1)", std::vector<int>{1}},
        {"push(1, 1)", "argument to `push` must be ARRAY, got INTEGER"},
    };
    for (BuiltinTest test : tests) {
        auto evaluated = testEval(test.input);
        std::visit(
            [&](const auto &expected) {
                using T = std::decay_t<decltype(expected)>;
                if constexpr (std::is_same_v<T, int>) {
                    testIntegerObject(evaluated, expected);
                } else if constexpr (std::is_same_v<T, std::string>) {
                    auto *err = dynamic_cast<object::Error *>(evaluated);
                    EXPECT_NE(err, nullptr) << "object is not an Error. got=" << err << '\n';
                    EXPECT_EQ(err->message, expected)
                        << "object has wrong value. got=" << err->message << " wanted=" << expected << '\n';
                }
            },
            test.expected);
    }
}

TEST(EvaluatorTest, ArrayLiterals) {
    std::string input = "[1, 2 * 2, 3 + 3]";
    auto evaluated = testEval(input);
    auto *result = dynamic_cast<object::Array *>(evaluated);
    ASSERT_NE(result, nullptr) << "object is not an Array. got=" << evaluated << '\n';
    EXPECT_EQ(result->elements.size(), 3) << "array was wrong size. got=" << result->elements.size();
    testIntegerObject(result->elements[0], 1);
    testIntegerObject(result->elements[1], 4);
    testIntegerObject(result->elements[2], 6);
}

TEST(EvaluatorTest, ArrayIndexExpressions) {
    struct ArrayTest {
        std::string input;
        std::optional<int> expected;
    };
    ArrayTest tests[10] = {
        {
            "[1, 2, 3][0]",
            1,
        },
        {
            "[1, 2, 3][1]",
            2,
        },
        {
            "[1, 2, 3][2]",
            3,
        },
        {
            "let i = 0; [1][i];",
            1,
        },
        {
            "[1, 2, 3][1 + 1];",
            3,
        },
        {
            "let myArray = [1, 2, 3]; myArray[2];",
            3,
        },
        {
            "let myArray = [1, 2, 3]; myArray[0] + myArray[1] + myArray[2];",
            6,
        },
        {
            "let myArray = [1, 2, 3]; let i = myArray[0]; myArray[i]",
            2,
        },
        {
            "[1, 2, 3][3]",
            std::nullopt,
        },
        {
            "[1, 2, 3][-1]",
            std::nullopt,
        },
    };
    for (ArrayTest test : tests) {
        auto evaluated = testEval(test.input);
        if (test.expected.has_value()) {
            testIntegerObject(evaluated, test.expected.value());
        } else {
            testNullObject(evaluated);
        }
    }
}

TEST(EvaluatorTest, HashLiterals) {
    std::string input = R"(let two = "two";
                        {
                            "one": 10 - 9,
                            two: 1 + 1,
                            "thr" + "ee": 6 / 2,
                            4: 4,
                            true: 5,
                            false: 6
                        })";
    auto evaluated = testEval(input);
    auto *result = dynamic_cast<object::Hash *>(evaluated);
    ASSERT_NE(result, nullptr) << "object is not a Hash. got=" << evaluated << '\n';
    object::Boolean* TRUE = new object::Boolean(true);
    object::Boolean* FALSE = new object::Boolean(false);

    std::map<object::HashKey, int> expected = {
        {object::String("one").hashKey(), 1},
        {object::String("two").hashKey(), 2},
        {object::String("three").hashKey(), 3},
        {object::Integer(4).hashKey(), 4},
        {TRUE->hashKey(), 5},
        {FALSE->hashKey(), 6},
    };
    EXPECT_EQ(result->pairs.size(), expected.size()) << "hash is wrong size" << '\n';

    for (const auto& e : expected){
        auto pair = result->pairs.find(e.first);
        EXPECT_NE(pair, result->pairs.end()) << "no pair for given key" << '\n';
        testIntegerObject(pair->second.value, e.second);
    }
}

object::Object *testEval(std::string input) {
    auto lexer = std::make_unique<lexer::Lexer>(input);
    parser::Parser parser = parser::Parser(std::move(lexer));
    std::unique_ptr<ast::Program> program = parser.parseProgram();
    object::Environment *env = new object::Environment();
    return evaluator::eval(program.get(), env);
}

void testIntegerObject(object::Object *obj, int expected) {
    auto *result = dynamic_cast<object::Integer *>(obj);
    ASSERT_NE(result, nullptr) << "object is not an Integer. got=" << obj << '\n';
    EXPECT_EQ(result->value, expected) << "object has wrong value. got=" << std::to_string(result->value)
                                       << " wanted=" << expected << '\n';
}

void testBooleanObject(object::Object *obj, bool expected) {
    auto *result = dynamic_cast<object::Boolean *>(obj);
    EXPECT_NE(result, nullptr) << "object is not a Boolean. got=" << result << '\n';
    EXPECT_EQ(result->value, expected) << "object has wrong value. got=" << std::to_string(result->value)
                                       << " wanted=" << expected << '\n';
}

void testNullObject(object::Object *obj) {
    EXPECT_EQ(obj->type(), object::ObjectType::NULL_OBJ) << "object is not nullptr." << '\n';
}
