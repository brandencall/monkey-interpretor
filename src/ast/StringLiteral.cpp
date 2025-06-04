#include "ast/StringLiteral.h"
#include <string>

namespace ast {

std::string StringLiteral::tokenLiteral() const { return token.literal; }

std::string StringLiteral::toString() const { return token.literal; }

} // namespace ast
