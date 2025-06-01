#include "evaluator/evaluator.h"
#include "ast/ExpressionStatement.h"
#include "ast/IntegerLiteral.h"
#include "ast/Program.h"
#include "object/Integer.h"

namespace evaluator {

std::unique_ptr<object::Object> Eval(ast::Node *node) {
    if (auto program = dynamic_cast<ast::Program *>(node)) {
        return evalStatements(std::move(program->statements));
    } else if(auto expression = dynamic_cast<ast::ExpressionStatement *>(node)){
        return Eval(expression->expression.get());
    } else if (auto intLiteral = dynamic_cast<ast::IntegerLiteral *>(node)) {
        std::unique_ptr<object::Integer> integerObj = std::make_unique<object::Integer>();
        integerObj->value = intLiteral->valueInt;
        return integerObj;
    }
    return nullptr;
}

std::unique_ptr<object::Object> evalStatements(std::vector<std::unique_ptr<ast::Statement>> statements) {
    std::unique_ptr<object::Object> result;
    for (auto const &statement : statements) {
        result = Eval(statement.get());
    }
    return result;
}
} // namespace evaluator
