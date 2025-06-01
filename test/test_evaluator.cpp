#include "lexer.h"
#include "object/Integer.h"
#include "object/object.h"
#include "parser.h"
#include "evaluator/evaluator.h"
#include <gtest/gtest.h>
#include <string>

std::unique_ptr<object::Object> testEval(std::string input);
void testIntegerObject(object::Object *obj, int expected);

TEST(EvaluatorTest, EvalIntegerExpression) {
    struct IntegerTest {
        std::string input;
        int expected;
    };
    IntegerTest tests[2] = {
        {"5", 5},
        {"10", 10},
    };

    for (IntegerTest test : tests) {
        auto evaluated = testEval(test.input);
        testIntegerObject(evaluated.get(), test.expected);
    }
}

std::unique_ptr<object::Object> testEval(std::string input) {
    auto lexer = std::make_unique<lexer::Lexer>(input);
    parser::Parser parser = parser::Parser(std::move(lexer));
    std::unique_ptr<ast::Program> program = parser.parseProgram();
    return evaluator::Eval(program.get());
}

void testIntegerObject(object::Object *obj, int expected) {
    auto *result = dynamic_cast<object::Integer *>(obj);
    EXPECT_NE(result, nullptr) << "object is not an Integer. got=" << result << '\n';
    EXPECT_EQ(result->value, expected) << "object has wrong value. got=" << result->value << " wanted=" << expected
                                       << '\n';
}
