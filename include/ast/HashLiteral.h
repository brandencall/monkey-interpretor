#pragma once
#include "Node.h"
#include "ast/Expression.h"
#include "token.h"
#include <map>
#include <memory>
#include <string>
#include <vector>

namespace ast {
class HashLiteral : public Expression {
  public:
    std::map<std::unique_ptr<ast::Expression>, std::unique_ptr<ast::Expression>> pairs;

    std::string tokenLiteral() const override;
    std::string toString() const override;
};
} // namespace ast
