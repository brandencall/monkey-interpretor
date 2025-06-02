#include "object/Integer.h"
#include "object/object.h"
#include <string>

namespace object {

std::string Integer::inspect() const { return std::to_string(value); }
ObjectType Integer::type() const { return objectType; }

} // namespace object
