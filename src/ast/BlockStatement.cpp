#include "ast/BlockStatement.h"
#include <sstream>
#include <string>

namespace ast {

std::string BlockStatement::tokenLiteral() const { return token.literal; }

std::string BlockStatement::toString() const {
    std::stringstream ss;
    for (const auto &statement : statements) {
        ss << statement->toString();
    }
    return ss.str();
}
} // namespace ast
