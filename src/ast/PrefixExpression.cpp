#include "ast/PrefixExpression.h"
#include <sstream>
#include <string>

namespace ast {

std::string PrefixExpression::tokenLiteral() const { return token.literal; }

std::string PrefixExpression::toString() const {
    std::stringstream ss;
    ss << "(" << oper << right->toString() << ")";
    return ss.str();
}
} // namespace ast
