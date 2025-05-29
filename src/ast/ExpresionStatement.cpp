#include "ast/ExpressionStatement.h"
#include <string>
#include <sstream>

namespace ast {

void ExpressionStatement::statementNode() const {}

std::string ExpressionStatement::tokenLiteral() const { return token.literal; }

std::string ExpressionStatement::toString() const {
    if (expression != nullptr) {
        return expression->toString();
    }
    return "";
}

} // namespace ast
