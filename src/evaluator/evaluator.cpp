#include "evaluator/evaluator.h"
#include "ast/ArrayLiteral.h"
#include "ast/BlockStatement.h"
#include "ast/Boolean.h"
#include "ast/CallExpression.h"
#include "ast/ExpressionStatement.h"
#include "ast/FunctionLiteral.h"
#include "ast/HashLiteral.h"
#include "ast/IfExpression.h"
#include "ast/IndexExpression.h"
#include "ast/InfixExpression.h"
#include "ast/IntegerLiteral.h"
#include "ast/LetStatement.h"
#include "ast/PrefixExpression.h"
#include "ast/Program.h"
#include "ast/ReturnStatement.h"
#include "ast/StringLiteral.h"
#include "object/Array.h"
#include "object/Boolean.h"
#include "object/Builtin.h"
#include "object/Environment.h"
#include "object/Function.h"
#include "object/Hash.h"
#include "object/Hashable.h"
#include "object/Integer.h"
#include "object/Null.h"
#include "object/ReturnValue.h"
#include "object/String.h"
#include "object/object.h"
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
std::map<std::string, object::Builtin *> builtins = {{"len", new object::Builtin(lenFunction)},
                                                     {"first", new object::Builtin(firstFunction)},
                                                     {"last", new object::Builtin(lastFunction)},
                                                     {"rest", new object::Builtin(restFunction)},
                                                     {"push", new object::Builtin(pushFunction)},
                                                     {"puts", new object::Builtin(putsFunction)}};

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
    } else if (auto arrayLit = dynamic_cast<ast::ArrayLiteral *>(node)) {
        std::vector<ast::Expression *> elements;
        elements.reserve(arrayLit->elements.size());
        for (const auto &elm : arrayLit->elements) {
            elements.push_back(elm.get());
        }
        std::vector<object::Object *> evaluatedElms = evalExpression(elements, env);
        if (evaluatedElms.size() == 1 && isError(evaluatedElms[0])) {
            return evaluatedElms[0];
        }
        object::Array *array = new object::Array();
        array->elements = evaluatedElms;
        return array;
    } else if (auto indexExpression = dynamic_cast<ast::IndexExpression *>(node)) {
        auto left = eval(indexExpression->left.get(), env);
        if (isError(left)) {
            return left;
        }
        auto index = eval(indexExpression->index.get(), env);
        if (isError(index)) {
            return index;
        }
        return evalIndexExpression(left, index);
    } else if (auto hash = dynamic_cast<ast::HashLiteral *>(node)) {
        return evalHashLiteral(hash, env);
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
    if (auto funcObj = dynamic_cast<object::Function *>(func)) {
        object::Environment *extendedEnv = extendFunctionEnvironment(funcObj, args);
        object::Object *evaluated = eval(funcObj->body.get(), extendedEnv);
        return unwrapReturnValue(evaluated);
    } else if (auto builtin = dynamic_cast<object::Builtin *>(func)) {
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
    if (args.size() != 1) {
        return newError("wrong number of arguments. want=1 but got=", args.size());
    }
    if (auto arg = dynamic_cast<object::String *>(args[0])) {
        return new object::Integer(arg->value.size());
    } else if (auto arg = dynamic_cast<object::Array *>(args[0])) {
        return new object::Integer(arg->elements.size());
    }
    return newError("argument to `len` not supported, got ", args[0]->typeToString());
}

object::Object *firstFunction(const std::vector<object::Object *> &args) {
    if (args.size() != 1) {
        return newError("wrong number of arguments. want=1 but got=", args.size());
    }
    if (args[0]->type() != object::ObjectType::ARRAY_OBJ) {
        return newError("argument to `first` must be ARRAY, got ", args[0]->typeToString());
    }
    auto arr = dynamic_cast<object::Array *>(args[0]);
    if (arr->elements.size() > 0) {
        return arr->elements[0];
    }
    return &NULL_OBJECT;
}

object::Object *lastFunction(const std::vector<object::Object *> &args) {
    if (args.size() != 1) {
        return newError("wrong number of arguments. want=1 but got=", args.size());
    }
    if (args[0]->type() != object::ObjectType::ARRAY_OBJ) {
        return newError("argument to `last` must be ARRAY, got ", args[0]->typeToString());
    }
    auto arr = dynamic_cast<object::Array *>(args[0]);
    int length = arr->elements.size();
    if (length > 0) {
        return arr->elements[length - 1];
    }
    return &NULL_OBJECT;
}

object::Object *restFunction(const std::vector<object::Object *> &args) {
    if (args.size() != 1) {
        return newError("wrong number of arguments. want=1 but got=", args.size());
    }
    if (args[0]->type() != object::ObjectType::ARRAY_OBJ) {
        return newError("argument to `rest` must be ARRAY, got ", args[0]->typeToString());
    }
    auto arr = dynamic_cast<object::Array *>(args[0]);
    int length = arr->elements.size();
    if (length > 0) {

        std::vector<object::Object *> newElements;
        newElements.reserve(length - 1);
        for (int i = 1; i < length; i++) {
            newElements.push_back(arr->elements[i]);
        }
        object::Array *newArray = new object::Array();
        newArray->elements = newElements;
        return newArray;
    }
    return &NULL_OBJECT;
}

object::Object *pushFunction(const std::vector<object::Object *> &args) {
    if (args.size() != 2) {
        return newError("wrong number of arguments. want=2 but got=", args.size());
    }
    if (args[0]->type() != object::ObjectType::ARRAY_OBJ) {
        return newError("argument to `push` must be ARRAY, got ", args[0]->typeToString());
    }
    auto arr = dynamic_cast<object::Array *>(args[0]);
    int length = arr->elements.size();

    std::vector<object::Object *> newElements;
    newElements.reserve(length + 1);
    for (int i = 0; i < length; i++) {
        newElements.push_back(arr->elements[i]);
    }
    newElements.push_back(args[1]);
    object::Array *newArray = new object::Array();
    newArray->elements = newElements;
    return newArray;
}

object::Object *putsFunction(const std::vector<object::Object *> &args) {
    for (const auto& arg : args){
        std::cout << arg->inspect() << '\n';
    }
    return &NULL_OBJECT;
}

object::Object *evalIndexExpression(object::Object *left, object::Object *index) {
    if (left->type() == object::ObjectType::ARRAY_OBJ && index->type() == object::ObjectType::INTEGER_OBJ) {
        return evalArrayIndexExpression(left, index);
    } else if (left->type() == object::ObjectType::HASH_OBJ) {
        return evalHashIndexExpression(left, index);
    } else {
        return newError("index operator not supported: ", left->typeToString());
    }
}

object::Object *evalArrayIndexExpression(object::Object *array, object::Object *index) {

    auto arrayObject = dynamic_cast<object::Array *>(array);
    auto intObject = dynamic_cast<object::Integer *>(index);
    int idx = intObject->value;
    int max = arrayObject->elements.size() - 1;

    if (idx < 0 || idx > max) {
        return &NULL_OBJECT;
    }

    return arrayObject->elements[idx];
}

object::Object *evalHashLiteral(ast::HashLiteral *hash, object::Environment *env) {
    std::map<object::HashKey, object::HashPair> resultPair;
    for (const auto &pair : hash->pairs) {
        auto key = eval(pair.first.get(), env);
        if (isError(key)) {
            return key;
        }
        auto hashKey = dynamic_cast<object::Hashable *>(key);
        if (hashKey == nullptr) {
            return newError("unusable as hashkey: ", key->typeToString());
        }
        auto value = eval(pair.second.get(), env);
        if (isError(value)) {
            return value;
        }
        auto hashed = hashKey->hashKey();
        resultPair[hashed] = object::HashPair{key, value};
    }
    object::Hash *result = new object::Hash();
    result->pairs = resultPair;
    return result;
}

object::Object *evalHashIndexExpression(object::Object *hash, object::Object *index) {
    auto hashObject = dynamic_cast<object::Hash *>(hash);
    auto key = dynamic_cast<object::Hashable *>(index);
    if (key == nullptr) {
        return newError("unusable as hash key: ", index->typeToString());
    }
    auto pair = hashObject->pairs.find(key->hashKey());
    if (pair == hashObject->pairs.end()){
        return &NULL_OBJECT;
    }
    return pair->second.value;
}

} // namespace evaluator
