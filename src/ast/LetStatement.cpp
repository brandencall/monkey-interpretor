#include "ast/LetStatement.h"
#include "ast/Statement.h"
#include <string>

namespace ast {

void LetStatement::statementNode() const {}

std::string LetStatement::tokenLiteral() const { return token.literal; }

} // namespace ast
