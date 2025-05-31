#pragma once
#include "Node.h"
#include "ast/BlockStatement.h"
#include "ast/Expression.h"
#include "ast/Identifier.h"
#include "token.h"
#include <memory>
#include <string>
#include <vector>

namespace ast {
class FunctionLiteral : public Expression {
  public:
    std::vector<std::unique_ptr<ast::Identifier>> parameters;
    std::unique_ptr<ast::BlockStatement> body;

    std::string tokenLiteral() const override;
    std::string toString() const override;
};
} // namespace ast
