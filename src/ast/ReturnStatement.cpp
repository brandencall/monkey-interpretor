#include "ast/ReturnStatement.h"
#include <sstream>
#include <string>

namespace ast {

std::string ReturnStatement::tokenLiteral() const { return token.literal; }

std::string ReturnStatement::toString() const {
    std::stringstream ss;
    ss << tokenLiteral() << " ";

    if (returnValue != nullptr) {
        ss << returnValue->toString();
    }
    ss << " ; ";
    return ss.str();
}

} // namespace ast
