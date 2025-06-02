#include "object/Error.h"
#include "object/object.h"
#include <string>

namespace object {

std::string Error::inspect() const { return "Error: " + message; }
ObjectType Error::type() const { return objectType; }
std::string Error::typeToString() const { return "ERROR"; }

} // namespace object
