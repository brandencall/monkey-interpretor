#pragma once
#include <string>

namespace ast {
class Node {
public: 
    virtual ~Node() = default;
    virtual std::string tokenLiteral() const = 0;
    virtual std::string toString() const = 0;
};
} // namespace ast
