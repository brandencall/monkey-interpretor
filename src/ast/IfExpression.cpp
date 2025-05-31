#include "ast/IfExpression.h"
#include <sstream>
#include <string>

namespace ast {

std::string IfExpression::tokenLiteral() const { return token.literal; }

std::string IfExpression::toString() const {
    std::stringstream ss;
    ss << "if" << condition->toString() << " " << consiquence->toString();
    if (alternative != nullptr) {
        ss << "else " << alternative->toString();
    }
    return ss.str();
}
} // namespace ast
