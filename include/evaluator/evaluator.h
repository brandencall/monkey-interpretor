#pragma once

#include "ast/Expression.h"
#include "ast/IfExpression.h"
#include "ast/Node.h"
#include "ast/Statement.h"
#include "object/Boolean.h"
#include "object/object.h"
#include <memory>
#include <vector>
namespace evaluator {

object::Object* eval(ast::Node *node);
object::Object* evalStatements(std::vector<std::unique_ptr<ast::Statement>> statements);
object::Boolean* nativeBoolToBooleanObject(bool input);
object::Object* evalPrefixExpression(std::string oper, object::Object* right);
object::Object* evalInfixExpression(std::string oper, object::Object* left, object::Object* right);
object::Object* evalBangOperatorExpression(object::Object* right);
object::Object* evalMinusOperatorExpression(object::Object* right);
object::Object* evalIntegerInfixExpression(std::string oper, object::Object* left, object::Object* right);
object::Object* evalIfExpression(ast::IfExpression* ifExpression);
bool isTruthy(object::Object* object);

}
