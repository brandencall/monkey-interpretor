#include "evaluator/evaluator.h"
#include "ast/Boolean.h"
#include "ast/ExpressionStatement.h"
#include "ast/IntegerLiteral.h"
#include "ast/PrefixExpression.h"
#include "ast/Program.h"
#include "object/Boolean.h"
#include "object/Integer.h"
#include "object/Null.h"
#include "object/object.h"
#include <iostream>
#include <string>

namespace evaluator {

object::Null NULL_OBJECT{};
object::Boolean TRUE{true};
object::Boolean FALSE{false};

object::Object *eval(ast::Node *node) {

    if (auto program = dynamic_cast<ast::Program *>(node)) {
        return evalStatements(std::move(program->statements));
    } else if (auto expression = dynamic_cast<ast::ExpressionStatement *>(node)) {
        return eval(expression->expression.get());
    } else if (auto intLiteral = dynamic_cast<ast::IntegerLiteral *>(node)) {
        object::Integer *integerObj = new object::Integer(intLiteral->valueInt);
        return integerObj;
    } else if (auto boolLiteral = dynamic_cast<ast::Boolean *>(node)) {
        return nativeBoolToBooleanObject(boolLiteral->valueBool);
    } else if (auto prefixExpression = dynamic_cast<ast::PrefixExpression *>(node)) {
        auto right = eval(prefixExpression->right.get());
        return evalPrefixExpression(prefixExpression->oper, right);
    }
    return nullptr;
}

object::Object *evalStatements(std::vector<std::unique_ptr<ast::Statement>> statements) {
    object::Object *result;
    for (auto const &statement : statements) {
        result = eval(statement.get());
    }
    return result;
}

object::Boolean *nativeBoolToBooleanObject(bool input) {
    if (input) {
        return &TRUE;
    }
    return &FALSE;
}

object::Object *evalPrefixExpression(std::string oper, object::Object *right) {
    if (oper == "!") {
        return evalBangOperatorExpression(right);
    } else if (oper == "-") {
        return evalMinusOperatorExpression(right);
    } else {
        return &NULL_OBJECT;
    }
}

object::Object *evalBangOperatorExpression(object::Object *right) {
    if (right == &TRUE) {
        return &FALSE;
    } else if (right == &FALSE) {
        return &TRUE;
    } else if (right == &NULL_OBJECT) {
        return &TRUE;
    } else {
        return &FALSE;
    }
}

object::Object *evalMinusOperatorExpression(object::Object *right) {

    if (right->type() != object::ObjectType::INTEGER_OBJ) {
        return nullptr;
    }
    auto intObj = dynamic_cast<object::Integer*>(right);
    return new object::Integer(-intObj->value);
}
} // namespace evaluator
