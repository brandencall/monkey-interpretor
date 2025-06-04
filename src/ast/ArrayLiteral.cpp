#include "ast/ArrayLiteral.h"
#include <sstream>
#include <string>

namespace ast {

std::string ArrayLiteral::tokenLiteral() const { return token.literal; }

std::string ArrayLiteral::toString() const {
    std::stringstream ss;
    ss << "[";
    for (size_t i = 0; i < elements.size(); i++) {
        if (i == elements.size() - 1) {
            ss << elements[i]->toString();
        } else {
            ss << elements[i]->toString() << ", ";
        }
    }
    ss << "]";
    return ss.str();
}
} // namespace ast
