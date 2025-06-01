#pragma once
#include "Node.h"
#include "ast/Expression.h"
#include "token.h"
#include <memory>
#include <string>
#include <vector>

namespace ast {
class CallExpression : public Expression {
  public:
    std::unique_ptr<ast::Expression> function;
    std::vector<std::unique_ptr<ast::Expression>> arguments;

    std::string tokenLiteral() const override;
    std::string toString() const override;
};
} // namespace ast
