#include "object/ReturnValue.h"
#include "object/object.h"
#include <string>

namespace object {

std::string ReturnValue::inspect() const { return value->inspect(); }
ObjectType ReturnValue::type() const { return objectType; }
std::string ReturnValue::typeToString() const { return "RETURN_VALUE"; }

} // namespace object
