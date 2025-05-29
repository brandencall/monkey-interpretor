#include "ast/Identifier.h"
#include <iostream>
#include <string>

namespace ast {

void Identifier::expressionNode() const {}

std::string Identifier::tokenLiteral() const { return token.literal; }

std::string Identifier::toString() const { 

    std::cout << "Identifier::toString(): " << value << '\n';
    return value; 
}

} // namespace ast
