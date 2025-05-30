#include "ast/Identifier.h"
#include <string>

namespace ast {

std::string Identifier::tokenLiteral() const { return token.literal; }

std::string Identifier::toString() const { return value; }

} // namespace ast
