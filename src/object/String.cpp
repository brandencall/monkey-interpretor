#include "object/String.h"
#include "object/object.h"
#include <string>

namespace object {

std::string String::inspect() const { return value; }
ObjectType String::type() const { return objectType; }
std::string String::typeToString() const { return "STRING"; }

} // namespace object
