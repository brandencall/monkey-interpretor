#include "evaluator/evaluator.h"
#include "ast/Boolean.h"
#include "ast/ExpressionStatement.h"
#include "ast/IntegerLiteral.h"
#include "ast/Program.h"
#include "object/Boolean.h"
#include "object/Integer.h"
#include "object/object.h"

namespace evaluator {

object::Boolean TRUE { true };
object::Boolean FALSE { false };

object::Object* eval(ast::Node *node) {
    
    if (auto program = dynamic_cast<ast::Program *>(node)) {
        return evalStatements(std::move(program->statements));
    } else if(auto expression = dynamic_cast<ast::ExpressionStatement *>(node)){
        return eval(expression->expression.get());
    } else if (auto intLiteral = dynamic_cast<ast::IntegerLiteral *>(node)) {
        object::Integer *integerObj = new object::Integer(intLiteral->valueInt);
        return integerObj;
    } else if (auto boolLiteral = dynamic_cast<ast::Boolean *>(node)) {
        return nativeBoolToBooleanObject(boolLiteral->valueBool);
    }
    return nullptr;
}

object::Object* evalStatements(std::vector<std::unique_ptr<ast::Statement>> statements) {
    object::Object *result;
    for (auto const &statement : statements) {
        result = eval(statement.get());
    }
    return result;
}

object::Boolean* nativeBoolToBooleanObject(bool input){
    if (input) {
        return &TRUE;
    }
    return &FALSE;
}
} // namespace evaluator
