#pragma once
#include "Node.h"
#include "token.h"
#include <string>

namespace ast {
class Expression : public Node {
  public:
    virtual ~Expression() = default;
    virtual std::string tokenLiteral() const = 0;
    virtual std::string toString() const = 0;
    token::Token token;
    std::string value;
};
} // namespace ast
