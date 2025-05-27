#include "ast/Identifier.h"
#include "ast/Statement.h"
#include <string>

namespace ast {

void Identifier::expressionNode() const {}

std::string Identifier::tokenLiteral() const { return token.literal; }

} // namespace ast
