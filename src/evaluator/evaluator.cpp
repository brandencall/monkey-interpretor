#include "evaluator/evaluator.h"
#include "ast/BlockStatement.h"
#include "ast/Boolean.h"
#include "ast/CallExpression.h"
#include "ast/ExpressionStatement.h"
#include "ast/FunctionLiteral.h"
#include "ast/IfExpression.h"
#include "ast/InfixExpression.h"
#include "ast/IntegerLiteral.h"
#include "ast/LetStatement.h"
#include "ast/PrefixExpression.h"
#include "ast/Program.h"
#include "ast/ReturnStatement.h"
#include "ast/StringLiteral.h"
#include "object/Boolean.h"
#include "object/Builtin.h"
#include "object/Environment.h"
#include "object/Function.h"
#include "object/Integer.h"
#include "object/Null.h"
#include "object/ReturnValue.h"
#include "object/String.h"
#include "object/object.h"
#include <algorithm>
#include <cstddef>
#include <iostream>
#include <memory>
#include <sstream>
#include <string>
#include <utility>
#include <vector>

namespace evaluator {

object::Null NULL_OBJECT{};
object::Boolean TRUE{true};
object::Boolean FALSE{false};
std::map<std::string, object::Builtin *> builtins = {{"len", new object::Builtin(lenFunction)}};

object::Object *eval(ast::Node *node, object::Environment *env) {

    if (auto program = dynamic_cast<ast::Program *>(node)) {
        return evalProgram(program, env);
    } else if (auto expression = dynamic_cast<ast::ExpressionStatement *>(node)) {
        return eval(expression->expression.get(), env);
    } else if (auto intLiteral = dynamic_cast<ast::IntegerLiteral *>(node)) {
        object::Integer *integerObj = new object::Integer(intLiteral->valueInt);
        return integerObj;
    } else if (auto boolLiteral = dynamic_cast<ast::Boolean *>(node)) {
        return nativeBoolToBooleanObject(boolLiteral->valueBool);
    } else if (auto prefixExpression = dynamic_cast<ast::PrefixExpression *>(node)) {
        auto right = eval(prefixExpression->right.get(), env);
        if (isError(right)) {
            return right;
        }
        return evalPrefixExpression(prefixExpression->oper, right);
    } else if (auto infixExpression = dynamic_cast<ast::InfixExpression *>(node)) {
        auto left = eval(infixExpression->left.get(), env);
        if (isError(left)) {
            return left;
        }
        auto right = eval(infixExpression->right.get(), env);
        if (isError(right)) {
            return right;
        }
        return evalInfixExpression(infixExpression->oper, left, right);
    } else if (auto blockStatement = dynamic_cast<ast::BlockStatement *>(node)) {
        return evalBlockStatement(blockStatement, env);
    } else if (auto ifExpression = dynamic_cast<ast::IfExpression *>(node)) {
        return evalIfExpression(ifExpression, env);
    } else if (auto returnStatement = dynamic_cast<ast::ReturnStatement *>(node)) {
        auto val = eval(returnStatement->returnValue.get(), env);
        if (isError(val)) {
            return val;
        }
        return new object::ReturnValue(val);
    } else if (auto letStatement = dynamic_cast<ast::LetStatement *>(node)) {
        auto val = eval(letStatement->value.get(), env);
        if (isError(val)) {
            return val;
        }
        std::unique_ptr<object::Object> saveObject(val);
        env->set(letStatement->name->value, std::move(saveObject));
    } else if (auto ident = dynamic_cast<ast::Identifier *>(node)) {
        return evalIdentifier(ident, env);
    } else if (auto funcLit = dynamic_cast<ast::FunctionLiteral *>(node)) {
        object::Function *func = new object::Function();
        func->parameters = std::move(funcLit->parameters);
        func->body = std::move(funcLit->body);
        func->env = env;
        return func;
    } else if (auto call = dynamic_cast<ast::CallExpression *>(node)) {
        auto func = eval(call->function.get(), env);
        if (isError(func)) {
            return func;
        }
        std::vector<ast::Expression *> args;
        args.reserve(call->arguments.size());
        for (const auto &arg : call->arguments) {
            args.push_back(arg.get());
        }
        std::vector<object::Object *> evaluatedArgs = evalExpression(args, env);
        if (evaluatedArgs.size() == 1 && isError(evaluatedArgs[0])) {
            return evaluatedArgs[0];
        }
        return applyFunction(func, evaluatedArgs);
    } else if (auto stringLit = dynamic_cast<ast::StringLiteral *>(node)) {
        object::String *stringObj = new object::String(stringLit->valueString);
        return stringObj;
    }

    return nullptr;
}

object::Object *evalProgram(ast::Program *program, object::Environment *env) {
    object::Object *result;
    for (auto const &statement : program->statements) {
        result = eval(statement.get(), env);
        if (auto returnValue = dynamic_cast<object::ReturnValue *>(result)) {
            return returnValue->value;
        } else if (auto err = dynamic_cast<object::Error *>(result)) {
            return err;
        }
    }
    return result;
}

object::Object *evalBlockStatement(ast::BlockStatement *block, object::Environment *env) {
    object::Object *result;
    for (auto const &statement : block->statements) {
        result = eval(statement.get(), env);
        if (result != nullptr) {
            if (result->type() == object::ObjectType::ERROR_OBJ || result->type() == object::ObjectType::RETURN_VALUE) {
                return result;
            }
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
        return newError("unknown operator: ", oper, right->typeToString());
    }
}

object::Object *evalInfixExpression(std::string oper, object::Object *left, object::Object *right) {

    if (left->type() == object::ObjectType::INTEGER_OBJ && right->type() == object::ObjectType::INTEGER_OBJ) {
        return evalIntegerInfixExpression(oper, left, right);
    } else if (left->type() == object::ObjectType::STRING_OBJ && right->type() == object::ObjectType::STRING_OBJ) {
        return evalStringInfixExpression(oper, left, right);
    } else if (oper == "==") {
        return nativeBoolToBooleanObject(left == right);
    } else if (oper == "!=") {
        return nativeBoolToBooleanObject(left != right);
    } else if (left->type() != right->type()) {
        return newError("type mismatch: ", left->typeToString(), oper, right->typeToString());
    } else {
        return newError("unknown operator: ", left->typeToString(), oper, right->typeToString());
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
        return newError("unknown operator: -", right->typeToString());
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
        return newError("unknown operator: ", left->typeToString(), oper, right->typeToString());
    }
}

object::Object *evalStringInfixExpression(std::string oper, object::Object *left, object::Object *right) {
    if (oper != "+") {
        return newError("unknown operator: ", left->typeToString(), oper, right->typeToString());
    }
    auto leftObj = dynamic_cast<object::String *>(left);
    auto rightObj = dynamic_cast<object::String *>(right);
    std::string leftVal = leftObj->value;
    std::string rightVal = rightObj->value;
    return new object::String(leftVal + rightVal);
}

object::Object *evalIfExpression(ast::IfExpression *ifExpression, object::Environment *env) {
    object::Object *condition = eval(ifExpression->condition.get(), env);
    if (isError(condition)) {
        return condition;
    }
    if (isTruthy(condition)) {
        return eval(ifExpression->consiquence.get(), env);
    } else if (ifExpression->alternative != nullptr) {
        return eval(ifExpression->alternative.get(), env);
    } else {
        return &NULL_OBJECT;
    }
}

object::Object *evalIdentifier(ast::Identifier *ident, object::Environment *env) {
    auto val = env->get(ident->value);
    if (val != nullptr) {
        return val;
    }
    auto builtin = builtins.find(ident->value);
    if (builtin != builtins.end()) {
        return builtin->second;
    }
    return newError("identifier not found: ", ident->value);
}

std::vector<object::Object *> evalExpression(std::vector<ast::Expression *> exps, object::Environment *env) {
    std::vector<object::Object *> result;
    for (const auto &exp : exps) {
        object::Object *evaluated = eval(exp, env);
        if (isError(evaluated)) {
            return std::vector<object::Object *>{evaluated};
        }
        result.push_back(evaluated);
    }
    return result;
}

object::Object *applyFunction(object::Object *func, std::vector<object::Object *> args) {
    if (auto funcObj = dynamic_cast<object::Function *>(func)){
        object::Environment *extendedEnv = extendFunctionEnvironment(funcObj, args);
        object::Object *evaluated = eval(funcObj->body.get(), extendedEnv);
        return unwrapReturnValue(evaluated);
    } else if (auto builtin = dynamic_cast<object::Builtin *>(func)){
        return builtin->fn(args);
    }
    return newError("not a function: ", func->typeToString());
}

object::Environment *extendFunctionEnvironment(object::Function *func, std::vector<object::Object *> args) {
    object::Environment *env = new object::Environment(func->env);
    for (size_t i = 0; i < func->parameters.size(); i++) {
        std::unique_ptr<object::Object> saveArg(args[i]);
        env->set(func->parameters[i]->value, std::move(saveArg));
    }
    return env;
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

template <typename... Args> object::Error *newError(const std::string &format, Args &&...args) {
    std::ostringstream oss;
    oss << format;
    ((oss << std::forward<Args>(args) << ' '), ...);
    std::string msg = oss.str();
    if (!msg.empty() && msg.back() == ' ')
        msg.pop_back();

    return new object::Error(msg);
}

bool isError(object::Object *object) {
    if (object != nullptr) {
        return object->type() == object::ObjectType::ERROR_OBJ;
    }
    return false;
}

object::Object *unwrapReturnValue(object::Object *obj) {

    auto returnValue = dynamic_cast<object::ReturnValue *>(obj);
    if (returnValue != nullptr) {
        return returnValue->value;
    }
    return obj;
}

object::Object *lenFunction(const std::vector<object::Object *> &args) { 
    if (args.size() != 1){
        return newError("wrong number of arguments. want=1 but got=", args.size());
    }
    auto arg = dynamic_cast<object::String*>(args[0]);
    if (arg == nullptr){
        return newError("argument to `len` not supported, got ", args[0]->typeToString());
    }
    return new object::Integer(arg->value.size());

}

} // namespace evaluator
