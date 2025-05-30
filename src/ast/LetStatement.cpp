#include "ast/LetStatement.h"
#include <sstream>
#include <string>

namespace ast {

std::string LetStatement::tokenLiteral() const { return token.literal; }

std::string LetStatement::toString() const {
    std::stringstream ss;
    ss << tokenLiteral() << " ";
    ss << name->toString();
    ss << " = ";

    if (value != nullptr) {
        ss << value->toString();
    }
    ss << ";";
    return ss.str();
}

} // namespace ast
