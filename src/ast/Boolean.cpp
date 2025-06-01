#include "ast/Boolean.h"
#include "token.h"
#include <string>

namespace ast {

std::string Boolean::tokenLiteral() const { return token.literal; }

std::string Boolean::toString() const { return token.literal; }

} // namespace ast
