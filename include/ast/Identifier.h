#pragma once
#include "Node.h"
#include "ast/Expression.h"
#include "token.h"
#include <memory>
#include <string>

namespace ast {
class Identifier : public Expression {
  public:
    void expressionNode() const override;
    std::string tokenLiteral() const override;
    std::string toString() const override;
    token::Token token;
    std::string value;
};
} // namespace ast
