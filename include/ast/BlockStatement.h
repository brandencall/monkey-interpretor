#pragma once
#include "Node.h"
#include "ast/Statement.h"
#include "token.h"
#include <memory>
#include <vector>

namespace ast {
class BlockStatement : public Statement {
  public:
    std::vector<std::unique_ptr<Statement>> statements;
    token::Token token;

    std::string tokenLiteral() const override;
    std::string toString() const override;
};
} // namespace ast
