#include "ast/ExpressionStatement.h"
#include <string>

namespace ast {

std::string ExpressionStatement::tokenLiteral() const { return token.literal; }

std::string ExpressionStatement::toString() const {
    if (expression != nullptr) {
        return expression->toString();
    }
    return "";
}

} // namespace ast
