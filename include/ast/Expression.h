#pragma once
#include "Node.h"

namespace ast {
class Expression : public Node {
  public:
    virtual ~Expression() = default;
    virtual void expressionNode() const = 0;
};
} // namespace ast
