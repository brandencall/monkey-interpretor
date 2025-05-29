#pragma once
#include "Node.h"
#include "ast/Expression.h"
#include "token.h"
#include <memory>
#include <string>

namespace ast {
class Identifier : public Expression {
  public:
    std::string tokenLiteral() const override;
    std::string toString() const override;
};
} // namespace ast
