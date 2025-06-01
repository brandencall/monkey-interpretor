#pragma once

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

}
