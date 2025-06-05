#include "object/Integer.h"
#include "object/object.h"
#include <string>

namespace object {

std::string Integer::inspect() const { return std::to_string(value); }
ObjectType Integer::type() const { return objectType; }
std::string Integer::typeToString() const { return "INTEGER"; }

HashKey Integer::hashKey() {
    HashKey result;
    result.type = type();
    result.value = value;
    result.object = this;
    return result;
}
bool Integer::hashable() const { return true; }

} // namespace object
