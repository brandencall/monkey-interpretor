#pragma once
#include "Node.h"
#include "ast/Expression.h"
#include "ast/Identifier.h"
#include "ast/Statement.h"
#include "token.h"
#include <memory>
#include <string>

namespace ast {
class ReturnStatement: public Statement {
  public:
    std::string tokenLiteral() const override;
    std::string toString() const override;
    token::Token token;
    std::unique_ptr<Expression> returnValue;
};
} // namespace ast
