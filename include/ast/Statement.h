#pragma once
#include "Node.h"

namespace ast {
class Statement: public Node{
public: 
    virtual ~Statement() = default;
    virtual void statementNode() const = 0;
};
} // namespace ast
