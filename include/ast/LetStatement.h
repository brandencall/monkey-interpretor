#pragma once
#include "Node.h"
#include "ast/Expression.h"
#include "ast/Identifier.h"
#include "ast/Statement.h"
#include "token.h"
#include <memory>
#include <string>

namespace ast {
class LetStatement : public Statement {
  public:
    std::string tokenLiteral() const override;
    void statementNode() const override;
    token::Token token;
    std::unique_ptr<Identifier> name;
    std::unique_ptr<Expression> value;
};
} // namespace ast
