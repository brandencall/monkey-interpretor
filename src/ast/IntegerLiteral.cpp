#include "ast/IntegerLiteral.h"
#include <string>

namespace ast {

std::string IntegerLiteral::tokenLiteral() const { return token.literal; }

std::string IntegerLiteral::toString() const { return token.literal; }

} // namespace ast
