#include "ast/Program.h"
#include "ast/Statement.h"
#include <string>

namespace ast {

std::string Program::tokenLiteral() const {
    if (!statements.empty()) {
        return statements[0]->tokenLiteral();
    }
    return "";
}

} // namespace ast
