#include "ast/ReturnStatement.h"
#include "ast/Statement.h"
#include <string>

namespace ast {

void ReturnStatement::statementNode() const {}

std::string ReturnStatement::tokenLiteral() const { return token.literal; }

} // namespace ast
