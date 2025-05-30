#pragma once
#include "Node.h"
#include "ast/Expression.h"
#include "token.h"
#include <algorithm>
#include <memory>
#include <string>

namespace ast {
class PrefixExpression : public Expression {
  public:
    std::string oper;
    std::unique_ptr<Expression> right;

    std::string tokenLiteral() const override;
    std::string toString() const override;
};
} // namespace ast
