#pragma once
#include "Node.h"
#include "ast/BlockStatement.h"
#include "ast/Expression.h"
#include "token.h"
#include <memory>
#include <string>

namespace ast {
class IfExpression : public Expression {
  public:
    std::unique_ptr<ast::Expression> condition;
    std::unique_ptr<ast::BlockStatement> consiquence;
    std::unique_ptr<ast::BlockStatement> alternative;

    std::string tokenLiteral() const override;
    std::string toString() const override;
};
} // namespace ast
