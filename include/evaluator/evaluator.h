#pragma once

#include "ast/BlockStatement.h"
#include "ast/Expression.h"
#include "ast/Identifier.h"
#include "ast/IfExpression.h"
#include "ast/Node.h"
#include "ast/Program.h"
#include "ast/Statement.h"
#include "object/Boolean.h"
#include "object/Environment.h"
#include "object/Error.h"
#include "object/Function.h"
#include "object/object.h"
#include <memory>
#include <string>
#include <vector>
namespace evaluator {
object::Object *eval(ast::Node *node, object::Environment *env);
object::Object *evalProgram(ast::Program *program, object::Environment *env);
object::Object *evalBlockStatement(ast::BlockStatement *block, object::Environment *env);
object::Boolean *nativeBoolToBooleanObject(bool input);
object::Object *evalPrefixExpression(std::string oper, object::Object *right);
object::Object *evalInfixExpression(std::string oper, object::Object *left, object::Object *right);
object::Object *evalBangOperatorExpression(object::Object *right);
object::Object *evalMinusOperatorExpression(object::Object *right);
object::Object *evalIntegerInfixExpression(std::string oper, object::Object *left, object::Object *right);
object::Object *evalStringInfixExpression(std::string oper, object::Object *left, object::Object *right);
object::Object *evalIfExpression(ast::IfExpression *ifExpression, object::Environment *env);
object::Object *evalIdentifier(ast::Identifier *ident, object::Environment *env);
object::Object *applyFunction(object::Object *func, std::vector<object::Object *> args);
object::Environment *extendFunctionEnvironment(object::Function *func, std::vector<object::Object *> args);
bool isTruthy(object::Object *object);
template <typename... Args> object::Error *newError(const std::string &format, Args &&...args);
bool isError(object::Object *object);
std::vector<object::Object *> evalExpression(std::vector<ast::Expression *> exps, object::Environment *env);
object::Object *unwrapReturnValue(object::Object *obj);

} // namespace evaluator
