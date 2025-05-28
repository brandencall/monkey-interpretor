#pragma once
#include "Node.h"
#include "ast/Statement.h"
#include <memory>
#include <vector>

namespace ast {
class Program : public Node {
  public:
    Program() = default;
    std::string tokenLiteral() const override;
    std::vector<std::unique_ptr<Statement>> statements;
};
} // namespace ast
