#pragma once
#include "ast/BlockStatement.h"
#include "ast/Identifier.h"
#include "object/Environment.h"
#include "object/object.h"
#include <memory>
#include <string>
#include <vector>

namespace object {
class Function : public Object {
  public:
    ObjectType objectType = ObjectType::FUNCTION_OBJ;
    std::vector<std::unique_ptr<ast::Identifier>> parameters;
    std::unique_ptr<ast::BlockStatement > body;
    Environment *env;

    ObjectType type() const override;
    std::string inspect() const override;
    std::string typeToString() const override;
};

} // namespace object
