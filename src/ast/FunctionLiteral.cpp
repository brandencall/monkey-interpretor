#include "ast/FunctionLiteral.h"
#include <sstream>
#include <string>

namespace ast {

std::string FunctionLiteral::tokenLiteral() const { return token.literal; }

std::string FunctionLiteral::toString() const {
    std::stringstream ss;
    ss << "(";
    for (size_t i = 0; i < parameters.size(); i++) {
        if (i == parameters.size() - 1) {
            ss << parameters[i]->toString();
        } else {
            ss << parameters[i]->toString() << ", ";
        }
    }
    ss << ") ";
    ss << body->toString();
    return ss.str();
}
} // namespace ast
