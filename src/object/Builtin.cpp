#include "object/Builtin.h"
#include "object/object.h"
#include <string>

namespace object {

std::string Builtin::inspect() const { return "builtin function"; }
ObjectType Builtin::type() const { return objectType; }
std::string Builtin::typeToString() const { return "BUILTIN"; }

} // namespace object
