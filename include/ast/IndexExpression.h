#pragma once
#include "Node.h"
#include "ast/Expression.h"
#include "token.h"
#include <algorithm>
#include <memory>
#include <string>

namespace ast {
class IndexExpression : public Expression {
  public:
    std::unique_ptr<Expression> left;
    std::unique_ptr<Expression> index;


    std::string tokenLiteral() const override;
    std::string toString() const override;
};
} // namespace ast
