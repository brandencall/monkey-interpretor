#pragma once
#include "Node.h"

namespace ast {
class Statement : public Node {
  public:
    virtual ~Statement() = default;
    virtual std::string toString() const = 0;
};
} // namespace ast
