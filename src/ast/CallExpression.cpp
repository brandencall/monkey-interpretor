#include "ast/CallExpression.h"
#include <sstream>
#include <string>

namespace ast {

std::string CallExpression::tokenLiteral() const { return token.literal; }

std::string CallExpression::toString() const {
    std::stringstream ss;
    ss << function->toString();
    ss << "(";
    for (size_t i = 0; i < arguments.size(); i++) {
        if (i == arguments.size() - 1) {
            ss << arguments[i]->toString();
        } else {
            ss << arguments[i]->toString() << ", ";
        }
    }
    ss << ")";
    return ss.str();
}
} // namespace ast
