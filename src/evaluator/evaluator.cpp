#include "evaluator/evaluator.h"
#include "ast/BlockStatement.h"
#include "ast/Boolean.h"
#include "ast/ExpressionStatement.h"
#include "ast/IfExpression.h"
#include "ast/InfixExpression.h"
#include "ast/IntegerLiteral.h"
#include "ast/PrefixExpression.h"
#include "ast/Program.h"
#include "ast/ReturnStatement.h"
#include "object/Boolean.h"
#include "object/Integer.h"
#include "object/Null.h"
#include "object/ReturnValue.h"
#include "object/object.h"
#include <string>

namespace evaluator {

object::Null NULL_OBJECT{};
object::Boolean TRUE{true};
object::Boolean FALSE{false};

object::Object *eval(ast::Node *node) {

    if (auto program = dynamic_cast<ast::Program *>(node)) {
        return evalProgram(program);
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
    } else if (auto infixExpression = dynamic_cast<ast::InfixExpression *>(node)) {
        auto left = eval(infixExpression->left.get());
        auto right = eval(infixExpression->right.get());
        return evalInfixExpression(infixExpression->oper, left, right);
    } else if (auto blockStatement = dynamic_cast<ast::BlockStatement *>(node)) {
        return evalBlockStatement(blockStatement);
    } else if (auto ifExpression = dynamic_cast<ast::IfExpression *>(node)) {
        return evalIfExpression(ifExpression);
    } else if (auto returnStatement = dynamic_cast<ast::ReturnStatement *>(node)) {
        auto val = eval(returnStatement->returnValue.get());
        return new object::ReturnValue(val);
    }

    return nullptr;
}

object::Object *evalProgram(ast::Program *program) {
    object::Object *result;
    for (auto const &statement : program->statements) {
        result = eval(statement.get());
        if (auto returnValue = dynamic_cast<object::ReturnValue *>(result)){
            return returnValue->value;
        }
    }
    return result;
}

object::Object *evalBlockStatement(ast::BlockStatement *block) {
    object::Object *result;
    for (auto const &statement : block->statements) {
        result = eval(statement.get());
        if (result != nullptr && result->type() == object::ObjectType::RETURN_VALUE){
            return result;
        }
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

object::Object *evalInfixExpression(std::string oper, object::Object *left, object::Object *right) {
    if (left->type() == object::ObjectType::INTEGER_OBJ && right->type() == object::ObjectType::INTEGER_OBJ) {
        return evalIntegerInfixExpression(oper, left, right);
    } else if (oper == "==") {
        return nativeBoolToBooleanObject(left == right);
    } else if (oper == "!=") {
        return nativeBoolToBooleanObject(left != right);
    } else {
        return nullptr;
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
    auto intObj = dynamic_cast<object::Integer *>(right);
    return new object::Integer(-intObj->value);
}

object::Object *evalIntegerInfixExpression(std::string oper, object::Object *left, object::Object *right) {
    auto leftObj = dynamic_cast<object::Integer *>(left);
    auto rightObj = dynamic_cast<object::Integer *>(right);
    if (oper == "+") {
        return new object::Integer(leftObj->value + rightObj->value);
    } else if (oper == "-") {
        return new object::Integer(leftObj->value - rightObj->value);
    } else if (oper == "*") {
        return new object::Integer(leftObj->value * rightObj->value);
    } else if (oper == "/") {
        return new object::Integer(leftObj->value / rightObj->value);
    } else if (oper == "<") {
        return nativeBoolToBooleanObject(leftObj->value < rightObj->value);
    } else if (oper == ">") {
        return nativeBoolToBooleanObject(leftObj->value > rightObj->value);
    } else if (oper == "==") {
        return nativeBoolToBooleanObject(leftObj->value == rightObj->value);
    } else if (oper == "!=") {
        return nativeBoolToBooleanObject(leftObj->value != rightObj->value);
    } else {
        return nullptr;
    }
}

object::Object *evalIfExpression(ast::IfExpression *ifExpression) {
    object::Object *condition = eval(ifExpression->condition.get());
    if (isTruthy(condition)) {
        return eval(ifExpression->consiquence.get());
    } else if (ifExpression->alternative != nullptr) {
        return eval(ifExpression->alternative.get());
    } else {
        return &NULL_OBJECT;
    }
}

bool isTruthy(object::Object *object) {
    if (object == &NULL_OBJECT) {
        return false;
    } else if (object == &TRUE) {
        return true;
    } else if (object == &FALSE) {
        return false;
    } else {
        return true;
    }
}

} // namespace evaluator
