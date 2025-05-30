#include "ast/InfixExpression.h"
#include <sstream>
#include <string>

namespace ast {

std::string InfixExpression::tokenLiteral() const { return token.literal; }

std::string InfixExpression::toString() const {
    std::stringstream ss;
    ss << "(" << left->toString() << " " << oper << " " << right->toString() << ")";
    return ss.str();
}
} // namespace ast
