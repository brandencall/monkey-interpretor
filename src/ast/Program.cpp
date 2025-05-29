#include "ast/Program.h"
#include "ast/Statement.h"
#include <ostream>
#include <sstream>
#include <string>

namespace ast {

std::string Program::tokenLiteral() const {
    if (!statements.empty()) {
        return statements[0]->tokenLiteral();
    }
    return "";
}

std::string Program::toString() const {
    std::stringstream ss;
    for (const auto &statement : statements) {
        ss << statement->toString() << std::endl;
    }
    return ss.str();
}

} // namespace ast
