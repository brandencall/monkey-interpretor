#include "ast/IndexExpression.h"
#include <sstream>
#include <string>

namespace ast {

std::string IndexExpression::tokenLiteral() const { return token.literal; }

std::string IndexExpression::toString() const {
    std::stringstream ss;
    ss << "(";
    ss << left->toString(); 
    ss << "[";
    ss << index->toString(); 
    ss << "])";
    return ss.str();
}
} // namespace ast
