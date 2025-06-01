#pragma once

#include "ast/Node.h"
#include "ast/Statement.h"
#include "object/object.h"
#include <memory>
#include <vector>
namespace evaluator {

std::unique_ptr<object::Object> Eval(ast::Node *node);
std::unique_ptr<object::Object> evalStatements(std::vector<std::unique_ptr<ast::Statement>> statements);

}
