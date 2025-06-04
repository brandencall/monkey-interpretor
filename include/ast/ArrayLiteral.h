#pragma once
#include "Node.h"
#include "ast/Expression.h"
#include "token.h"
#include <memory>
#include <string>
#include <vector>

namespace ast {
class ArrayLiteral : public Expression {
  public:
    std::vector<std::unique_ptr<ast::Expression>> elements;

    std::string tokenLiteral() const override;
    std::string toString() const override;
};
} // namespace ast
