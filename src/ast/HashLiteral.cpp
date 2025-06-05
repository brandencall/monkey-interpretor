#include "ast/HashLiteral.h"
#include <sstream>
#include <string>

namespace ast {

std::string HashLiteral::tokenLiteral() const { return token.literal; }

std::string HashLiteral::toString() const {
    std::stringstream ss;
    ss << "{";
    //extra , at the end
    for (const auto &pair : pairs) {
        ss << pair.first->toString() + ":" + pair.second->toString() + ", ";
    }
    ss << "}";

    return ss.str();
}
} // namespace ast
