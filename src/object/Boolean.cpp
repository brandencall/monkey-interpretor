#include "object/Boolean.h"
#include "object/object.h"
#include <string>

namespace object {

std::string Boolean::inspect() const { return std::to_string(value); }
Object::ObjectType Boolean::type() const { return objectType; }

} // namespace object
