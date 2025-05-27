#pragma once
#include "Node.h"
#include "ast/Statement.h"
#include <vector>

namespace ast {
class Program : public Node {
  public:
    Program() = default;
    ~Program();
    std::string tokenLiteral() const override;
    std::vector<Statement *> statements;
};
} // namespace ast
