#include "object/Null.h"
#include "object/object.h"
#include <string>

namespace object {

std::string Null::inspect() const { return "null"; }
ObjectType Null::type() const { return objectType; }

} // namespace object
